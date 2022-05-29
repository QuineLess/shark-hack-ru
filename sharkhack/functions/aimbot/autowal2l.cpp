#include "autowal2l.h"

#define HITGROUP_GENERIC 0
#define HITGROUP_HEAD 1
#define HITGROUP_CHEST 2
#define HITGROUP_STOMACH 3
#define HITGROUP_LEFTARM 4
#define HITGROUP_RIGHTARM 5
#define HITGROUP_LEFTLEG 6
#define HITGROUP_RIGHTLEG 7
#define HITGROUP_GEAR 10

int last_penetrated_count;

void autowall::trace_line(Vector& abs_start, Vector& abs_end, unsigned int mask, C_BasePlayer* ignore, CGameTrace* ptr) {
	Ray_t ray;
	ray.Init(abs_start, abs_end);
	CTraceFilter filter;
	filter.pSkip = ignore;

	g_engine_trace->TraceRay(ray, mask, &filter, ptr);
}

void autowall::scale_damage(C_BasePlayer* local_player, CGameTrace &enter_trace, CCSWeaponInfo *weapon_data, float& current_damage) {

	if (!enter_trace.hit_entity || !enter_trace.hit_entity->GetClientClass() || enter_trace.hit_entity->GetClientClass()->m_ClassID != classids::get(XORSTR("CCSPlayer")))
		return;

	auto target = reinterpret_cast<C_BasePlayer*>(enter_trace.hit_entity);
	int hitgroup = enter_trace.hitgroup;
	auto is_zeus = local_player->get_weapon_handle()->get_definition_index() == WEAPON_TASER;

	const auto is_armored = [&]() -> bool {
		if (target->get_armor() > 0.f) {
			switch (hitgroup) {
			case HITGROUP_GENERIC:
			case HITGROUP_CHEST:
			case HITGROUP_STOMACH:
			case HITGROUP_LEFTARM:
			case HITGROUP_RIGHTARM:
				return true;
			case HITGROUP_HEAD:
				return target->get_has_helmet();
			default:
				break;
			}
		}

		return false;
	};

	if (!is_zeus) {
		switch (hitgroup) {
		case HITGROUP_HEAD:
			if (target->get_has_heavy_armor())
				current_damage = (current_damage * 4.f) * 0.5f;
			else
				current_damage *= 4.f;
			break;
		case HITGROUP_STOMACH:
			current_damage *= 1.25f;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			current_damage *= .75f;
			break;
		default:
			break;
		}
	}

	if (is_armored()) {
		float modifier = 1.f, armor_bonus_ratio = 0.5f, armor_ratio = weapon_data->m_flArmorRatio * 0.5f;

		if (target->get_has_heavy_armor()) {
			armor_bonus_ratio = 0.33f;
			armor_ratio = (weapon_data->m_flArmorRatio * 0.5f) * 0.5f;
			modifier = 0.33f;
		}

		auto new_damage = current_damage * armor_ratio;

		if (target->get_has_heavy_armor())
			new_damage *= 0.85f;

		if ((current_damage - current_damage * armor_ratio) * (modifier * armor_bonus_ratio) > target->get_armor())
			new_damage = current_damage - target->get_armor() / armor_bonus_ratio;

		current_damage = new_damage;
	}
}

uint32_t get_filter_simple_vtable() {
	static const auto filter_simple = *reinterpret_cast<uint32_t*>(utils::find_signature(XORSTR("client.dll"), XORSTR("55 8B EC 83 E4 F0 83 EC 7C 56 52")) + 0x3d);
	return filter_simple;
}

bool autowall::trace_to_exit(C_BasePlayer* local_player, trace_t& enter_trace, trace_t& exit_trace, const Vector start_position, const Vector direction, const bool is_local) {
	const auto max_distance = is_local ? 200.f : 90.f;
	const auto ray_extension = is_local ? 8.f : 4.f;

	float current_distance = 0;
	auto first_contents = 0;

	while (current_distance <= max_distance) {
		current_distance += ray_extension;

		auto start = start_position + direction * current_distance;

		if (!first_contents)
			first_contents = g_engine_trace->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX);

		const auto point_contents = g_engine_trace->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX);

		if (!(point_contents & MASK_SHOT_HULL) || (point_contents & CONTENTS_HITBOX && point_contents != first_contents)) {
			const auto end = start - direction * ray_extension;

			Ray_t r{};
			r.Init(start, end);
			uint32_t filter[4] = { get_filter_simple_vtable(), uint32_t(local_player), 0, 0 };
			g_engine_trace->TraceRay(r, MASK_SHOT_HULL | CONTENTS_HITBOX, reinterpret_cast<CTraceFilter*>(filter), &exit_trace);

			if (exit_trace.startsolid && exit_trace.surface.flags & SURF_HITBOX) {
				r.Init(start, start_position);
				filter[1] = reinterpret_cast<uint32_t>(exit_trace.hit_entity);
				g_engine_trace->TraceRay(r, MASK_SHOT_HULL | CONTENTS_HITBOX, reinterpret_cast<CTraceFilter*>(filter), &exit_trace);

				if (exit_trace.DidHit() && !exit_trace.startsolid) {
					start = exit_trace.endpos;
					return true;
				}

				continue;
			}

			if (exit_trace.DidHit() && !exit_trace.startsolid) {
				if (enter_trace.hit_entity && exit_trace.hit_entity != nullptr &&  reinterpret_cast<C_BasePlayer*>(exit_trace.hit_entity)->is_breakable())
					return true;

				if (enter_trace.surface.flags & SURF_NODRAW || (!(exit_trace.surface.flags & SURF_NODRAW) && exit_trace.plane.normal.Dot(direction) <= 1.f)) {
					const auto mult_amount = exit_trace.fraction * 4.f;
					start -= direction * mult_amount;
					return true;
				}

				continue;
			}

			if (!exit_trace.DidHit() || exit_trace.startsolid) {
				if (enter_trace.DidHitNonWorldEntity() && exit_trace.hit_entity != nullptr && reinterpret_cast<C_BasePlayer*>(enter_trace.hit_entity)->is_breakable()) {
					exit_trace = enter_trace;
					exit_trace.endpos = start;
					return true;
				}
			}
		}
	}

	return false;
}

bool autowall::handle_bullet_penetration(C_BasePlayer* local_player, CCSWeaponInfo* weapon_data, trace_t& enter_trace, Vector& eye_position, Vector direction, int& possible_hits_remaining, float& current_damage, float penetration_power, float ff_damage_bullet_penetration, bool pskip)
{
	if (&current_damage == nullptr)
		return 0;

	CGameTrace exit_trace;
	C_BasePlayer* enemy = reinterpret_cast<C_BasePlayer*>(enter_trace.hit_entity);
	auto enter_surface_data = g_physprops->GetSurfaceData(enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;

	if (!enter_surface_data || enter_surface_data->game.flPenetrationModifier < 0.1f) 
		return false;

	auto enter_penetration_modifier = enter_surface_data->game.flPenetrationModifier;
	float enter_damage_modifier = enter_surface_data->game.flDamageModifier;
	bool is_solid_surf = (enter_trace.contents & CONTENTS_GRATE);
	bool is_light_surf = (enter_trace.surface.flags & SURF_NODRAW);

	if (enter_trace.surface.name == (const char*)0x2227c261 && exit_trace.surface.name == (const char*)0x2227c868)
		return false;

	if ((!possible_hits_remaining && !is_light_surf && !is_solid_surf && enter_material != CHAR_TEX_GRATE && enter_material != CHAR_TEX_GLASS)
		|| weapon_data->m_flPenetration <= 0.f)
		return false;

	if (!trace_to_exit(local_player, enter_trace, exit_trace, enter_trace.endpos, direction, weapon_data->m_iDamage > 10000.f)) {
		if (!(g_engine_trace->GetPointContents(enter_trace.endpos, 0x600400B, nullptr) & 0x600400B))
			return false;
	}

	auto exit_surface_data = g_physprops->GetSurfaceData(exit_trace.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;
	float exit_surf_penetration_modifier = exit_surface_data->game.flPenetrationModifier;
	float exit_damage_modifier = exit_surface_data->game.flDamageModifier;

	auto combined_damage_modifier = 0.16f;
	auto combined_penetration_modifier = 0.f;

	if (enter_material == CHAR_TEX_GLASS || enter_material == CHAR_TEX_GRATE) {
		combined_damage_modifier = 0.05f;
		combined_penetration_modifier = 3.f;
	}
	else if (is_solid_surf || is_light_surf) {
		combined_penetration_modifier = 1.f;
		combined_damage_modifier = 0.16f;
	}
	else if (enter_material == CHAR_TEX_FLESH && (enemy != nullptr && enemy->is_teammate(local_player))) {
		combined_penetration_modifier = ff_damage_bullet_penetration;
		combined_damage_modifier = 0.16f;
	}
	else {
		combined_damage_modifier = 0.16f;
		combined_penetration_modifier = ((enter_penetration_modifier + exit_surf_penetration_modifier) * 0.5f);
	}

	if (enter_material == exit_material) {
		if (enter_material == CHAR_TEX_WOOD || exit_material == CHAR_TEX_CARDBOARD)
			combined_penetration_modifier = 3.f;
		else if (enter_material == CHAR_TEX_PLASTIC)
			combined_penetration_modifier = 2.f;
	}

	auto thickness = (exit_trace.endpos - enter_trace.endpos).Length();
	thickness *= thickness;
	thickness *= fmaxf(0.f, 1.0f / combined_penetration_modifier);
	thickness /= 24.0f;

	const auto lost_damage = fmaxf(0.0f, current_damage * combined_damage_modifier + fmaxf(0.f, 1.0f / combined_penetration_modifier)
		* 3.0f * fmaxf(0.0f, 3.0f / penetration_power) * 1.25f + thickness);

	if (lost_damage > current_damage)
		return false;

	if (lost_damage > 0.f)
		current_damage -= lost_damage;

	if (current_damage < 1.f)
		return false;

	eye_position = exit_trace.endpos;
	--possible_hits_remaining;

	return true;
}

void autowall::fix_trace_ray(Vector end, Vector start, trace_t* oldtrace, C_BasePlayer* entity) {

	if (!entity)
		return;

	const auto mins = entity->GetCollideable()->OBBMins();
	const auto maxs = entity->GetCollideable()->OBBMaxs();

	auto dir(end - start);
	dir.Normalized();

	const auto center = (maxs + mins) / 2;
	const auto pos(center + entity->get_origin());

	auto to = pos - start;
	const float range_along = dir.Dot(to);

	float range;
	if (range_along < 0.f) {
		range = -to.Length();
	}
	else if (range_along > dir.Length()) {
		range = -(pos - end).Length();
	}
	else {
		auto ray(pos - (dir * range_along + start));
		range = ray.Length();
	}

	if (range <= 60.f) {

		Ray_t ray;
		ray.Init(start, end);

		trace_t trace;
		g_engine_trace->ClipRayToEntity(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, entity, &trace);

		if (oldtrace->fraction > trace.fraction)
			*oldtrace = trace;
	}
}

void autowall::clip_trace_to_players(const Vector& vec_abs_start, const Vector& vec_abs_end, uint32_t mask, ITraceFilter* filter, trace_t* tr) {
	float smallest_fraction = tr->fraction;
	constexpr float max_range = 60.0f;

	Vector delta(vec_abs_end - vec_abs_start);
	const float delta_length = delta.Normalize();

	Ray_t ray;
	ray.Init(vec_abs_start, vec_abs_end);

	for (int i = 1; i <= 64; ++i) {
		auto entity = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(i));
		if (!entity || entity->IsDormant() || !entity->is_alive())
			continue;

		if (filter && !filter->ShouldHitEntity(entity, mask))
			continue;

		matrix3x4_t coordinate_frame;
		math::angle_matrix(entity->get_abs_angles(), entity->get_origin(), coordinate_frame);

		auto mins = entity->GetCollideable()->OBBMins();
		auto maxs = entity->GetCollideable()->OBBMaxs();

		auto obb_center = (maxs + mins) * 0.5f;
		math::vector_transform(obb_center, coordinate_frame, obb_center);

		auto extend = (obb_center - vec_abs_start);
		auto range_along = delta.Dot(extend);

		float range;
		if (range_along >= 0.0f) {
			if (range_along <= delta_length)
				range = Vector(obb_center - ((delta * range_along) + vec_abs_start)).Length();
			else
				range = -(obb_center - vec_abs_end).Length();
		}
		else {
			range = -extend.Length();
		}

		if (range > 0.0f && range <= max_range) {
			trace_t player_trace;
			g_engine_trace->ClipRayToEntity(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, entity, &player_trace);
			if (player_trace.fraction < smallest_fraction) {
				*tr = player_trace;
				smallest_fraction = player_trace.fraction;
			}
		}
	}
}

bool autowall::fire_bullet(C_BasePlayer* local_player, Vector eyepos, C_BaseCombatWeapon* weapon, Vector& direction, float& current_damage, C_BasePlayer * entity) {

	if (!weapon || !local_player)
		return false;

	float current_distance = 0.f, penetration_power, penetration_distance, max_range, ff_damage_bullet_penetration;

	static IConVar* damage_bullet_penetration = g_cvar->FindVar(XORSTR("ff_damage_bullet_penetration"));

	ff_damage_bullet_penetration = damage_bullet_penetration->GetFloat();

	CCSWeaponInfo* weapon_data = weapon->get_weapon_data();
	CGameTrace enter_trace;

	CTraceFilter filter;
	filter.pSkip = local_player;

	if (!weapon_data)
		return false;

	max_range = weapon_data->m_flRange;
	penetration_distance = weapon_data->m_flRange;
	penetration_power = weapon_data->m_flPenetration;

	last_penetrated_count = 4;
	current_damage = float(weapon_data->m_iDamage);

	while (last_penetrated_count > 0 && current_damage >= 1.0f) {
		max_range -= current_distance;
		Vector end = eyepos + direction * max_range;

		trace_line(eyepos, end, MASK_SHOT_HULL | CONTENTS_HITBOX, local_player, &enter_trace);

		if (entity)
			fix_trace_ray(eyepos + direction * 40.f, eyepos, &enter_trace, entity);
		else
			clip_trace_to_players(eyepos, end + direction * 40.f, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &enter_trace);

		if (enter_trace.fraction == 1.0f)
			break;

		auto enter_surface_data = g_physprops->GetSurfaceData(enter_trace.surface.surfaceProps);
		float enter_surf_penetration_modifier = enter_surface_data->game.flPenetrationModifier;
		int enter_material = enter_surface_data->game.material;

		current_distance += enter_trace.fraction * max_range;
		current_damage *= powf(weapon_data->m_flRangeModifier, (current_distance / 500.f));

		if (current_distance > max_range || current_distance > 3000.0 && weapon_data->m_flPenetration > 0.f || enter_surf_penetration_modifier < 0.1f)
			last_penetrated_count = 0;

		bool can_do_damage = (enter_trace.hitgroup > 0 && enter_trace.hitgroup <= 8);
		bool is_player = (enter_trace.hit_entity && enter_trace.hit_entity->GetClientClass() && enter_trace.hit_entity->GetClientClass()->m_ClassID == classids::get(XORSTR("CCSPlayer")));
		bool is_enemy = !reinterpret_cast<C_BasePlayer*>(enter_trace.hit_entity)->is_teammate(local_player);
		bool on_team = (reinterpret_cast<C_BasePlayer*>(enter_trace.hit_entity)->get_team() == 2 || reinterpret_cast<C_BasePlayer*>(enter_trace.hit_entity)->get_team() == 3);

		if ((can_do_damage && is_player && is_enemy) && on_team || (entity == enter_trace.hit_entity)) {
			scale_damage(local_player, enter_trace, weapon_data, current_damage);
			return true;
		}

		if (!handle_bullet_penetration(local_player, weapon_data, enter_trace, eyepos, direction, last_penetrated_count, current_damage, penetration_power, ff_damage_bullet_penetration))
			break;
	}

	return false;
}

float autowall::can_hit(C_BasePlayer* local_player, Vector &point,  C_BasePlayer* entity) {

	if (local_player == nullptr || entity == nullptr)
		return 0;

	Vector angles, direction;
	Vector tmp = point - local_player->get_eye_position();
	float current_damage = 0;

	math::vector_angles(tmp, angles);
	math::angle_vectors(angles, &direction);
	direction.Normalize();

	auto local_weapon = reinterpret_cast<C_BaseCombatWeapon*>(g_ent_list->GetClientEntityFromHandle(local_player->get_weapon_handle()));

	if (local_weapon != nullptr) {
		if (fire_bullet(local_player, local_player->get_eye_position(), local_weapon, direction, current_damage, entity))
			return current_damage;
		else
			return -1;
	}

	return -1; 
}