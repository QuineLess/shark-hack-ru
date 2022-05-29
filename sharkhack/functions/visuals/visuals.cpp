#include "visuals.h"
#include "sounds.h"

#include "../../gui/gui.h"

void visuals::no_smoke(C_BasePlayer* local_player) {

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player)
		return;

	std::vector<const char*> smoke_materials { XORSTR("particle/vistasmokev1/vistasmokev1_emods"), XORSTR("particle/vistasmokev1/vistasmokev1_emods_impactdust"), XORSTR("particle/vistasmokev1/vistasmokev1_fire"), XORSTR("particle/vistasmokev1/vistasmokev1_smokegrenade") };

	for (const auto mat : smoke_materials) {
		auto material = g_mat_system->FindMaterial(mat, TEXTURE_GROUP_OTHER);
		material->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_NO_DRAW, config.misc_no_smoke);
	}

	if (config.misc_no_smoke) {
		static int* get_smoke_count = *(int**)(memory.smoke_count);
		*get_smoke_count = 0;
	}
}

void visuals::no_flash(C_BasePlayer* local_player) {

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player)
		return;

	std::vector<const char*> flash_materials{ XORSTR("effects\\flashbang"), XORSTR("effects\\flashbang_white") };

	for (const auto mat : flash_materials) {
		auto material = g_mat_system->FindMaterial(mat, TEXTURE_GROUP_CLIENT_EFFECTS);
		material->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_NO_DRAW, config.misc_no_flash_effect);
	}
}

void visuals::set_sky_box(C_BasePlayer* local_player, const std::string sky_box_name) {

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player)
		return;

	auto o_set_sky_box = reinterpret_cast<void(__fastcall*)(const char*)>(memory.custom_sky_box);
	o_set_sky_box(sky_box_name.c_str());
}

void visuals::dynamic_light(C_BasePlayer* local_player) {

	if (!config.visuals_dynamic_lights)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	for (int i = 1; i <= g_ent_list->GetHighestEntityIndex(); i++) {
		const auto entity = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(i));

		if (!entity)
			continue;

		if (entity == local_player)
			continue;

		if (entity->IsDormant() || entity->GetClientClass()->m_ClassID != classids::get(XORSTR("CCSPlayer")))
			continue;

		if (!entity->is_alive())
			continue;

		if (entity->is_teammate(local_player))
			continue;

		Color color = Color(config.color_visuals_dynamic_lights);

		dlight_t* elight = g_effects->CL_AllocElight(entity->EntIndex());

		elight->color.r = color.r();
		elight->color.g = color.g();
		elight->color.b = color.b();
		elight->direction = entity->get_origin();
		elight->origin = entity->get_origin();
		elight->radius = 200.0f;
		elight->die = g_globals->curtime + 0.1f;
		elight->decay = 25.0f;
		elight->key = entity->EntIndex();

		dlight_t* dlight = g_effects->CL_AllocDlight(entity->EntIndex());

		dlight->color.r = color.r();
		dlight->color.g = color.g();
		dlight->color.b = color.b();
		dlight->direction = entity->get_origin();
		dlight->origin = entity->get_origin();
		dlight->radius = 100.f;
		dlight->die = g_globals->curtime + 0.1f;
		dlight->decay = dlight->radius / 5.f;
		dlight->key = entity->EntIndex();
	}
}

void visuals::draw_spread_circle(C_BasePlayer* local_player) {

	if (!config.visuals_spread_crosshair)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player || !local_player->is_alive())
		return;

	if (!local_player->get_weapon_handle())
		return;

	const auto accuracy_raduis = local_player->get_weapon_handle()->get_inaccuracy() * 550.f;

	render::render_circle(int(ImGui::GetIO().DisplaySize.x / 2.0f), int(ImGui::GetIO().DisplaySize.y / 2.0f), accuracy_raduis, int(accuracy_raduis * 3.f), Color(config.color_visuals_spread_crosshair), 3.f);
}

void visuals::fog_controller() {

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	for (int i = 1; i <= g_ent_list->GetHighestEntityIndex(); i++) {
		const auto entity = reinterpret_cast<C_FogController*>(g_ent_list->GetClientEntity(i));

		if (!entity)
			continue;

		if (entity->GetClientClass()->m_ClassID != classids::get(XORSTR("CFogController")))
			continue;

		Color color(config.color_fog);

		entity->get_fog_enable() = config.misc_fog_enable;
		entity->get_fog_start() = config.misc_fog_start;
		entity->get_fog_end() = config.misc_fog_end;
		entity->get_fog_max_density() = config.misc_fog_density;
		entity->get_fog_blend() = config.misc_fog_blend;
		entity->get_fog_color_primary() = utils::rgb_to_int(color.b(), color.g(), color.r());
		entity->get_fog_color_secondary() = utils::rgb_to_int(color.b(), color.g(), color.r());
	}
}

void visuals::draw_taser_range(C_BasePlayer* local_player) {

	if (!config.visuals_taser_range)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player || !local_player->is_alive())
		return;

	auto weapon = local_player->get_weapon_handle().Get();

	if (!weapon)
		return;

	if (weapon->get_definition_index() == WEAPON_TASER) {
		Vector prev_scr_pos, scr_pos;
		auto step = M_PI * 2.0 / 2047;
		float rad = local_player->get_weapon_handle()->get_weapon_data()->m_flRange;
		Vector eye_pos = local_player->get_eye_position();
		static float hue_offset = 0;

		for (float rotation = 0; rotation < (M_PI * 2.0); rotation += float(step)) {
			Vector pos(rad * cos(rotation) + eye_pos.x, rad * sin(rotation) + eye_pos.y, eye_pos.z);
			Ray_t ray;
			trace_t trace;
			CTraceFilter filter;

			filter.pSkip = local_player;
			ray.Init(eye_pos, pos);

			g_engine_trace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace);

			if (math::world_to_screen(trace.endpos, scr_pos)) {
				if (prev_scr_pos.IsValid()) {
					float hue = RAD2DEG(rotation) + hue_offset;
					ImVec4 temp = ImColor::HSV(hue / 360.f, 1, 1);
					Color color = Color(temp.x, temp.y, temp.z);

					render::render_line_float(prev_scr_pos.x, prev_scr_pos.y, scr_pos.x, scr_pos.y, color);
				}
				prev_scr_pos = scr_pos;
			}
		}
		hue_offset += 0.25f;
	}
}

void visuals::add_damage_indicator(damage_indicator hit_log) {
	damage_indicator_log.push_back(hit_log);
}

void visuals::draw_damage_indicator(C_BasePlayer * local_player) {

	if (!config.visuals_damage_indicator)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player || !local_player->is_alive())
		return;

	for (size_t id = 0; id < damage_indicator_log.size(); id++) {

		if (damage_indicator_log.at(id).bone.IsZero() || damage_indicator_log.at(id).time <= g_globals->curtime || ((damage_indicator_log.at(id).time - g_globals->curtime) > 3.1f)) {
			damage_indicator_log.erase(damage_indicator_log.begin() + id);
			continue;
		}

		if (!damage_indicator_log.at(id).bone.IsZero()) {
			damage_indicator_log.at(id).bone.z -= (config.visuals_damage_indicator_speed * (g_globals->curtime - damage_indicator_log.at(id).time));

			Vector out;
			if (math::world_to_screen(damage_indicator_log.at(id).bone, out) && damage_indicator_log.at(id).time > g_globals->curtime && ((damage_indicator_log.at(id).time - g_globals->curtime) < 3.1f))
				render::render_text(to_string(damage_indicator_log.at(id).hp).c_str(), (int)out.x, (int)out.y, Color(config.color_visuals_damage_indicator), g_visuals_font, 16.f, text_effect::text_outline);
		}
	}
}

void visuals::on_events_damage_indicator(IGameEvent* event) {

	if (!config.visuals_damage_indicator)
		return;

	int hitted_id = g_engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));
	int attacker_id = g_engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker")));

	if (hitted_id != g_engine->GetLocalPlayer() && attacker_id == g_engine->GetLocalPlayer()) {
		int hp = event->GetInt(XORSTR("dmg_health"));
		const auto entity = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(hitted_id));

		damage_indicator hit_log(hp, g_globals->curtime + 3.f, entity->get_bone_position(HITBOX_HEAD));
		add_damage_indicator(hit_log);
	}
}

float get_hit_marker_time;

void visuals::draw_hit_marker(C_BasePlayer * local_player) {

	if (!config.visuals_hit_marker)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player || !local_player->is_alive())
		return;

	if (get_hit_marker_time + 0.45f >= g_globals->curtime) {
		int center_x = int(ImGui::GetIO().DisplaySize.x / 2);
		int center_y = int(ImGui::GetIO().DisplaySize.y / 2);

		int line_size = 14;

		line_size -= static_cast<int>(static_cast<float>(((get_hit_marker_time + 0.45f) - g_globals->curtime) / 1.2) * 45);
		std::clamp(line_size, 0, 8);

		render::render_line(center_x - line_size, center_y - line_size, center_x - (line_size / 2), center_y - (line_size / 2), Color::White(), 1.5f);
		render::render_line(center_x - line_size, center_y + line_size, center_x - (line_size / 2), center_y + (line_size / 2), Color::White(), 1.5f);
		render::render_line(center_x + line_size, center_y + line_size, center_x + (line_size / 2), center_y + (line_size / 2), Color::White(), 1.5f);
		render::render_line(center_x + line_size, center_y - line_size, center_x + (line_size / 2), center_y - (line_size / 2), Color::White(), 1.5f);
	}
}

void visuals::on_events_hit_marker(IGameEvent* event) {

	if (g_engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker"))) == g_engine->GetLocalPlayer()) {
		get_hit_marker_time = g_globals->curtime;

		switch (config.visuals_hit_sound) {
		case 1: get_play_sound(XORSTR("physics/metal/metal_solid_impact_bullet2")); break;
		case 2: get_play_sound(XORSTR("buttons/arena_switch_press_02")); break;
		case 3: get_play_sound(XORSTR("training/timer_bell")); break;
		case 4: get_play_sound(XORSTR("physics/glass/glass_impact_bullet1")); break;
		case 5: get_play_sound(XORSTR("ui/lobby_notification_matchready")); break;
		case 6: get_play_sound(XORSTR("resource/warning")); break;
		case 7: get_custom_play_sound(hit_sound_one); break;
		}
	}
}

void visuals::sniper_crosshair(C_BasePlayer* local_player) {
	static auto show_spread = g_cvar->FindVar(XORSTR("weapon_debug_spread_show"));
	show_spread->SetValue((config.visuals_sniper_crosshair && local_player && !local_player->is_scoped()) ? 3 : 0);
}

void visuals::draw_hit_hitbox(IGameEvent* event) {

	if (!config.visuals_hit_hitbox)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	int hitted_id = g_engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));
	int attacker_id = g_engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker")));

	if (attacker_id == g_engine->GetLocalPlayer() && hitted_id != g_engine->GetLocalPlayer()) {

		const auto entity = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(hitted_id));

		if (!entity)
			return;

		if (!entity->EntIndex())
			return;

		matrix3x4_t bone_matrix[MAXSTUDIOBONES];

		if (!entity->SetupBones(bone_matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, g_globals->curtime))
			return;

		studiohdr_t *studio_hdr = g_mdl_info->get_studio_model(entity->GetModel());

		if (!studio_hdr)
			return;

		const auto hitbox_set = studio_hdr->GetHitboxSet(entity->get_hitbox_set());

		if (!hitbox_set)
			return;

		for (int i = 0; i < hitbox_set->numhitboxes; i++) {
			mstudiobbox_t *hitbox = hitbox_set->GetHitbox(i);

			if (!hitbox)
				continue;

			Vector min_actual, max_actual;

			math::vector_transform(hitbox->bbmin, bone_matrix[hitbox->bone], min_actual);
			math::vector_transform(hitbox->bbmax, bone_matrix[hitbox->bone], max_actual);

			g_debug_overlay->add_capsule_overlay(min_actual, max_actual, hitbox->m_flRadius, Color(config.color_visuals_hit_hitbox), 2.5f);
		}
	}
}

std::unordered_map<int, float> player_in_move;

void visuals::draw_player_circle(C_BasePlayer* local_player) {

	if (!config.visuals_enemy_circle)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	for (int i = 1; i <= g_ent_list->GetHighestEntityIndex(); i++) {
		const auto entity = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(i));

		if (!entity)
			continue;

		if (entity == local_player)
			continue;

		if (i <= g_globals->max_clients) {

			if (entity->is_teammate(local_player))
				continue;

			if (entity->IsDormant() || entity->GetClientClass()->m_ClassID != classids::get(XORSTR("CCSPlayer")))
				continue;

			if (!entity->is_alive())
				continue;

			auto radius = &player_in_move[entity->EntIndex()];
			ImGui::Anim(radius, entity->get_velocity().Length() > 1.f, 35.f, 60.f, 45.f, 45.f);

			float precision = 120.0f;
			Vector origin = entity->get_origin();
			static float hue_offset = 0.f;

			const float step = (float(M_PI) * 2) / precision;

			for (float a = 0.f; a < (float(M_PI) * 2); a += step) {
				Vector start = { *radius * cosf(a) + origin.x, *radius * sinf(a) + origin.y, origin.z };
				Vector end = { *radius * cosf(a + step) + origin.x, *radius * sinf(a + step) + origin.y, origin.z };
				Vector start_screen, end_screen;

				if (!math::world_to_screen(start, start_screen) || !math::world_to_screen(end, end_screen))
					continue;

				float hue = RAD2DEG(a) + hue_offset;
				ImVec4 temp = ImColor::HSV(hue / 360.f, 1, 1);
				Color color = Color(temp.x, temp.y, temp.z);

				render::render_line_float(start_screen.x, start_screen.y, end_screen.x, end_screen.y, color, 2.5f);
			}
			hue_offset += 2.f;
		}
	}
}

void visuals::bomb_timer() {

	if (!config.visuals_bomb_timer)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	for (int i = 1; i <= g_ent_list->GetHighestEntityIndex(); i++) {
		const auto entity = reinterpret_cast<C_PlantedC4*>(g_ent_list->GetClientEntity(i));

		if (!entity || entity->IsDormant())
			continue;

		if (entity->GetClientClass()->m_ClassID != classids::get(XORSTR("CPlantedC4")))
			continue;

		if (entity->get_bomb_defused())
			continue;

		float timer = entity->get_bomb_timer();

		if (timer == 0.f)
			continue;

		int x = int(ImGui::GetIO().DisplaySize.x) - 200;

		static auto pos_bomb = [](int y1, int y2, float value, float max_value) { return (float(fabs(y1 - y2))) / max_value * (max_value - value); };

		render::render_rect_filled_multicolor(x - 110, 82, x + 160, 166, Color(0, 0, 0, 0), Color(0, 0, 0, 0), Color(0, 0, 0, 175), Color(0, 0, 0, 175));

		render::render_rect_filled(x - 100, 125, x + 50, 140, Color(0, 185, 0));
		render::render_rect_filled(x + 50, 125, x + 75, 140, Color(185, 185, 0));
		render::render_rect_filled(x + 75, 125, x + 100, 140, Color(185, 0, 0));

		render::render_rect_filled(x - 100 + int(pos_bomb(x + 100, x - 100, timer, 40.f)), 125, x + 100, 140, Color(255, 255, 255, 70));

		render::render_text(std::to_string(int(timer)).c_str(), x - 100 + int(pos_bomb(x + 100, x - 100, timer, 40.f)), 126, Color::White(), roboto_medium, 13.f, text_effect::text_clear);

		static std::string timer_text;

		if (timer <= 40.f && timer > 10.f)
			timer_text = XORSTR("CAN DEFUSE!");
		else if (timer <= 10.f && timer > 5.f)
			timer_text = XORSTR("WITH KIT!");
		else if (timer <= 5.f)
			timer_text = XORSTR("RUN!");

		render::render_text(timer_text.c_str(), x - 100, 145, Color::White(), roboto_medium, 16.f, text_effect::text_clear);
		render::render_text(XORSTR("TIMER"), x - 100, 104, Color::White(), roboto_medium, 16.f, text_effect::text_clear);
		render::render_text(XORSTR("BOMB"), x - 100, 92, Color::White(), roboto_medium, 12.f, text_effect::text_clear);

		render::render_image(x + 74, 59, x + 184, 189, bomb_texture);
	}
}

void visuals::night_mode(C_BasePlayer* local_player)
{
	if (!g_engine->IsConnected() || !g_engine->IsInGame())
		return;

	for (int i = 1; i <= g_ent_list->GetHighestEntityIndex(); i++) {
		const auto entity = reinterpret_cast<C_EnvTonemapController*>(g_ent_list->GetClientEntity(i));

		if (!entity)
			continue;

		if (entity->GetClientClass()->m_ClassID != classids::get(XORSTR("CEnvTonemapController")))
			continue;

		entity->get_use_custom_auto_exposure_min() = entity->get_use_custom_auto_exposure_max() = config.misc_night_mode_enable;
		entity->get_custom_auto_exposure_min() = entity->get_custom_auto_exposure_max() = 1.01f - config.misc_night_mode_value;
		entity->get_custom_bloom_scale() = config.misc_night_mode_enable ? config.misc_bloom_effect_value : 0.f;
	}

	static auto skyname_cvar = g_cvar->FindVar(XORSTR("sv_skyname"));
	static auto r_3dsky = g_cvar->FindVar(XORSTR("r_3dsky"));
	static auto r_modelAmbientMin = g_cvar->FindVar(XORSTR("r_modelAmbientMin"));

	set_sky_box(local_player, config.misc_night_mode_enable ? XORSTR("sky_csgo_night02") : skyname_cvar->GetString());
	r_3dsky->SetValue(!config.misc_night_mode_enable);
	r_modelAmbientMin->SetValue(config.misc_night_mode_enable ? config.misc_model_ambient_value : 0.f);
}