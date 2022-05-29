#include "chams.h"
#include "../backtrack/backtrack.h"

void chams::on_init() {
	material_glow_overlay = g_mat_system->CreateMaterial(XORSTR("glow_overlay"), KeyValues::FromString(XORSTR("VertexLitGeneric"), "$additive 1 $envmap models/effects/cube_white $envmapfresnel 1 $phongboost 48 $phongalbedoboost 120"));
	material_glow_overlay->IncrementReferenceCount();
	material_animated = g_mat_system->CreateMaterial(XORSTR("animated"), KeyValues::FromString(XORSTR("VertexLitGeneric"), "$envmap editor/cube_vertigo $envmapcontrast 1 $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } }"));
	material_animated->IncrementReferenceCount();
	material_pearlescent = g_mat_system->CreateMaterial(XORSTR("pearlescent"), KeyValues::FromString(XORSTR("VertexLitGeneric"), "$nocull 1 $nofog 1 $model 1 $phong 1 $phongboost 0 $basemapalphaphongmask 1"));
	material_pearlescent->IncrementReferenceCount();
	material_texture = g_mat_system->FindMaterial("debug/debugambientcube", nullptr, true, nullptr);
	material_texture->IncrementReferenceCount();
	material_flat = g_mat_system->FindMaterial("debug/debugdrawflat", nullptr, true, nullptr);
	material_flat->IncrementReferenceCount();
	material_glow = g_mat_system->FindMaterial("dev/glow_armsrace", nullptr, true, nullptr);
	material_glow->IncrementReferenceCount();
	material_gloss = g_mat_system->FindMaterial("models/inventory_items/trophy_majors/gloss", nullptr, true, nullptr);
	material_gloss->IncrementReferenceCount();
	material_crystal = g_mat_system->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", nullptr, true, nullptr);
	material_crystal->IncrementReferenceCount();
	material_glass = g_mat_system->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", nullptr, true, nullptr);
	material_glass->IncrementReferenceCount();
	material_ghost = g_mat_system->FindMaterial("models/effects/ghosts/ghost", nullptr, true, nullptr);
	material_ghost->IncrementReferenceCount();
}

void chams::get_material(IMaterial* material, bool ignorez, bool wireframe, float r, float g, float b, float alpha) {
	material->ColorModulate(r, g, b);
	material->AlphaModulate(alpha);
	material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignorez);
	material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	g_studio_render->ForcedMaterialOverride(material);
}

void chams::player(C_BasePlayer* local_player, uintptr_t ecx, uintptr_t edx, void * results, DrawModelInfo_t * info, matrix3x4_t * bone_to_world, float * flex_weights, float * flex_delayed_weights, Vector & model_origin, int flags) {

	if (!info || !info->client_entity)
		return;

	if (strstr(info->client_entity->GetModel()->szName, XORSTR("models/player"))) {
		const auto entity = reinterpret_cast<C_BasePlayer*>(info->client_entity->GetIClientUnknown()->GetBaseEntity());

		if (!entity)
			return;

		if (entity->IsDormant() || entity->GetClientClass()->m_ClassID != classids::get(XORSTR("CCSPlayer")))
			return;

		if (!entity->is_alive())
			return;

		auto& data = records[entity->EntIndex()];

		if (data.size() > 0 && entity->get_velocity().Length() > 1.f) {
			if (config.legit_backtrack_chams_enable) {
				if (config.legit_backtrack_chams == 0) {
					for (auto& record : data) {
						get_material(material_texture, false, false, 1.f, 1.f, 1.f, 0.8f);
						o_draw_model_execute(ecx, edx, results, info, record.bone_matrix, flex_weights, flex_delayed_weights, model_origin, flags);
					}
				}
				else if (config.legit_backtrack_chams == 1) {
					get_material(material_texture, false, false, 1.f, 1.f, 1.f, 0.8f);
					o_draw_model_execute(ecx, edx, results, info, data.back().bone_matrix, flex_weights, flex_delayed_weights, model_origin, flags);
				}
			}
		}

		if (config.chams_enable) {

			if (entity->is_teammate(local_player) && config.chams_enemy_only)
				return;

			static float* color_visible;
			static float* color_invisible;
			static float* color_more;

			if (entity->get_team() == team_id::team_terrorist) {
				color_visible = config.color_chams_t_visible;
				color_invisible = config.color_chams_t_invisible;
				color_more = config.color_chams_more_t;
			}
			else if (entity->get_team() == team_id::team_counter_terrorist) {
				color_visible = config.color_chams_ct_visible;
				color_invisible = config.color_chams_ct_invisible;
				color_more = config.color_chams_more_ct;
			}

			if (config.chams_type == chams_type::chams_type_flat) {
				if (config.chams_invisible) {
					get_material(material_flat, true, config.chams_wireframe, color_invisible[0], color_invisible[1], color_invisible[2], 1.f);
					o_draw_model_execute(ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
				}
				get_material(material_flat, false, config.chams_wireframe, color_visible[0], color_visible[1], color_visible[2], 1.f);
			}
			else if (config.chams_type == chams_type::chams_type_texture) {
				if (config.chams_invisible) {
					get_material(material_texture, true, config.chams_wireframe, color_invisible[0], color_invisible[1], color_invisible[2], 1.f);
					o_draw_model_execute(ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
				}
				get_material(material_texture, false, config.chams_wireframe, color_visible[0], color_visible[1], color_visible[2], 1.f);
			}
			else if (config.chams_type == chams_type::chams_type_animated) {
				if (config.chams_invisible) {
					get_material(material_animated, true, config.chams_wireframe, color_invisible[0], color_invisible[1], color_invisible[2], 1.f);
					o_draw_model_execute(ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
				}
				get_material(material_animated, false, config.chams_wireframe, color_visible[0], color_visible[1], color_visible[2], 1.f);
			}
			else if (config.chams_type == chams_type::chams_type_pearlescent) {
				if (config.chams_invisible) {
					get_material(material_pearlescent, true, config.chams_wireframe, color_invisible[0], color_invisible[1], color_invisible[2], 1.f);
					o_draw_model_execute(ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
				}
				get_material(material_pearlescent, false, config.chams_wireframe, color_visible[0], color_visible[1], color_visible[2], 1.f);
				material_pearlescent->FindVar(XORSTR("$pearlescent"), nullptr, false)->SetValue(config.chams_pearlescent_value);
			}
			else if (config.chams_type == chams_type::chams_type_ghost) {
				if (config.chams_invisible) {
					get_material(material_ghost, true, config.chams_wireframe, color_invisible[0], color_invisible[1], color_invisible[2], 1.f);
					o_draw_model_execute(ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
				}
				get_material(material_ghost, false, config.chams_wireframe, color_visible[0], color_visible[1], color_visible[2], 1.f);
			}
			o_draw_model_execute(ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
			if (config.chams_more) {
				if (config.chams_more_type == chams_more_type::chams_more_type_glow - 1)
					material = material_glow;
				else if (config.chams_more_type == chams_more_type::chams_more_type_glow_overlay - 1) {
					material = material_glow_overlay;
					material->FindVar(XORSTR("$envmapfresnelminmaxexp"), nullptr, false)->SetVecComponentValue(3.8f, 2);
				}
				else if (config.chams_more_type == chams_more_type::chams_more_type_glow_pulse - 1) {
					material = material_glow_overlay;
					material->FindVar(XORSTR("$envmapfresnelminmaxexp"), nullptr, false)->SetVecComponentValue(9.0f * (1.2f - (0.5f * std::sin(g_globals->curtime * 5.0f) * 0.5f + 0.5f)), 2);
				}
				else if (config.chams_more_type == chams_more_type::chams_more_type_gloss - 1)
					material = material_gloss;
				else if (config.chams_more_type == chams_more_type::chams_more_type_crystal - 1)
					material = material_crystal;
				else if (config.chams_more_type == chams_more_type::chams_more_type_glass - 1)
					material = material_glass;

				material->FindVar(XORSTR("$envmaptint"), nullptr, false)->SetVectorValue(color_more[0], color_more[1], color_more[2]);
				g_studio_render->ForcedMaterialOverride(material);
				o_draw_model_execute(ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
			}
		}
	}
}

void chams::arms(C_BasePlayer* local_player, uintptr_t ecx, uintptr_t edx, void * results, DrawModelInfo_t * info, matrix3x4_t * bone_to_world, float * flex_weights, float * flex_delayed_weights, Vector & model_origin, int flags) {

	if (!info || !info->client_entity)
		return;

	if (!local_player || !local_player->is_alive())
		return;

	if (strstr(info->client_entity->GetModel()->szName, XORSTR("arms"))) {
		if (config.chams_arms) {
			switch (config.chams_arms_type) {
			case chams_type::chams_type_flat:
				get_material(material_flat, false, config.chams_arms_wireframe, config.color_chams_arms[0], config.color_chams_arms[1], config.color_chams_arms[2], 1.f);
				break;
			case chams_type::chams_type_texture:
				get_material(material_texture, false, config.chams_arms_wireframe, config.color_chams_arms[0], config.color_chams_arms[1], config.color_chams_arms[2], 1.f);
				break;
			case chams_type::chams_type_animated:
				get_material(material_animated, false, config.chams_arms_wireframe, config.color_chams_arms[0], config.color_chams_arms[1], config.color_chams_arms[2], 1.f);
				break;
			case chams_type::chams_type_pearlescent:
				get_material(material_pearlescent, false, config.chams_arms_wireframe, config.color_chams_arms[0], config.color_chams_arms[1], config.color_chams_arms[2], 1.f);
				material_pearlescent->FindVar(XORSTR("$pearlescent"), nullptr, false)->SetValue(config.chams_arms_pearlescent_value);
				break;
			case chams_type::chams_type_ghost:
				get_material(material_ghost, false, config.chams_arms_wireframe, config.color_chams_arms[0], config.color_chams_arms[1], config.color_chams_arms[2], 1.f);
				break;
			}
			o_draw_model_execute(ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
		}

		if (config.chams_arms_more_type != 0) {
			switch (config.chams_arms_more_type) {
			case chams_more_type::chams_more_type_glow:
				material = material_glow;
				break;
			case chams_more_type::chams_more_type_glow_overlay:
				material = material_glow_overlay;
				material->FindVar(XORSTR("$envmapfresnelminmaxexp"), nullptr, false)->SetVecComponentValue(3.8f, 2);
				break;
			case chams_more_type::chams_more_type_glow_pulse:
				material = material_glow_overlay;
				material->FindVar(XORSTR("$envmapfresnelminmaxexp"), nullptr, false)->SetVecComponentValue(9.0f * (1.2f - (0.5f * std::sin(g_globals->curtime * 5.0f) * 0.5f + 0.5f)), 2);
				break;
			case chams_more_type::chams_more_type_gloss:
				material = material_gloss;
				break;
			case chams_more_type::chams_more_type_crystal:
				material = material_crystal;
				break;
			case chams_more_type::chams_more_type_glass:
				material = material_glass;
				break;
			}
			material->FindVar(XORSTR("$envmaptint"), nullptr, false)->SetVectorValue(config.color_chams_arms_more[0], config.color_chams_arms_more[1], config.color_chams_arms_more[2]);
			g_studio_render->ForcedMaterialOverride(material);
			o_draw_model_execute(ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
		}
	}
}

void chams::weapon(C_BasePlayer* local_player, uintptr_t ecx, uintptr_t edx, void * results, DrawModelInfo_t * info, matrix3x4_t * bone_to_world, float * flex_weights, float * flex_delayed_weights, Vector & model_origin, int flags) {

	if (!info || !info->client_entity)
		return;

	if (!local_player || !local_player->is_alive() || local_player->is_scoped())
		return;

	if (strstr(info->client_entity->GetModel()->szName, XORSTR("models/weapons/v_")) && !strstr(info->client_entity->GetModel()->szName, XORSTR("tablet")) && !strstr(info->client_entity->GetModel()->szName, XORSTR("parachute")) && !strstr(info->client_entity->GetModel()->szName, XORSTR("fists")) && !strstr(info->client_entity->GetModel()->szName, XORSTR("arms"))) {
		const auto color_more = config.color_chams_weapon_more;

		if (config.chams_weapon) {
			switch (config.chams_weapon_type) {
			case chams_type::chams_type_flat:
				get_material(material_flat, false, config.chams_weapon_wireframe, config.color_chams_weapon[0], config.color_chams_weapon[1], config.color_chams_weapon[2], 1.f);
				break;
			case chams_type::chams_type_texture:
				get_material(material_texture, false, config.chams_weapon_wireframe, config.color_chams_weapon[0], config.color_chams_weapon[1], config.color_chams_weapon[2], 1.f);
				break;
			case chams_type::chams_type_animated:
				get_material(material_animated, false, config.chams_weapon_wireframe, config.color_chams_weapon[0], config.color_chams_weapon[1], config.color_chams_weapon[2], 1.f);
				break;
			case chams_type::chams_type_pearlescent:
				get_material(material_pearlescent, false, config.chams_weapon_wireframe, config.color_chams_weapon[0], config.color_chams_weapon[1], config.color_chams_weapon[2], 1.f);
				material_pearlescent->FindVar(XORSTR("$pearlescent"), nullptr, false)->SetValue(config.chams_arms_pearlescent_value);
				break;
			case chams_type::chams_type_ghost:
				get_material(material_ghost, false, config.chams_weapon_wireframe, config.color_chams_weapon[0], config.color_chams_weapon[1], config.color_chams_weapon[2], 1.f);
				break;
			}
			o_draw_model_execute(ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
		}

		if (config.chams_weapon_more_type != 0) {
			switch (config.chams_weapon_more_type) {
			case chams_more_type::chams_more_type_glow:
				material = material_glow;
				break;
			case chams_more_type::chams_more_type_glow_overlay:
				material = material_glow_overlay;
				material->FindVar(XORSTR("$envmapfresnelminmaxexp"), nullptr, false)->SetVecComponentValue(3.8f, 2);
				break;
			case chams_more_type::chams_more_type_glow_pulse:
				material = material_glow_overlay;
				material->FindVar(XORSTR("$envmapfresnelminmaxexp"), nullptr, false)->SetVecComponentValue(9.0f * (1.2f - (0.5f * std::sin(g_globals->curtime * 5.0f) * 0.5f + 0.5f)), 2);
				break;
			case chams_more_type::chams_more_type_gloss:
				material = material_gloss;
				break;
			case chams_more_type::chams_more_type_crystal:
				material = material_crystal;
				break;
			case chams_more_type::chams_more_type_glass:
				material = material_glass;
				break;
			}
			material->FindVar(XORSTR("$envmaptint"), nullptr, false)->SetVectorValue(config.color_chams_weapon_more[0], config.color_chams_weapon_more[1], config.color_chams_weapon_more[2]);
			g_studio_render->ForcedMaterialOverride(material);
			o_draw_model_execute(ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
		}
	}
}