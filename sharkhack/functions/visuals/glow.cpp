#include "glow.h"

Color glow::get_player_color(C_BasePlayer* local_player, C_BasePlayer* entity, bool visible) {
	Color color;

	if (entity->get_team() == team_id::team_terrorist)
		color = visible ? Color(config.color_glow_t_visible) : Color(config.color_glow_t_invisible);
	else if (entity->get_team() == team_id::team_counter_terrorist)
		color = visible ? Color(config.color_glow_ct_visible) : Color(config.color_glow_ct_invisible);

	return color;
}

void glow::on_post_screen(C_BasePlayer* local_player) {

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	for (auto i = 0; i < g_glow_manager->glow_object_definitions.Count(); i++) {
		auto & glow_object = g_glow_manager->glow_object_definitions[i];
		const auto entity = glow_object.get_entity();

		if (glow_object.is_empty() || !entity)
			continue;

		if (entity->IsDormant())
			continue;

		const auto class_id = entity->GetClientClass()->m_ClassID;

		if (class_id == classids::get(XORSTR("CCSPlayer"))) {

			if (!config.glow_enable)
				continue;

			if (!entity->is_alive())
				continue;

			if (entity->is_teammate(local_player) && config.glow_enemy_only)
				continue;

			bool is_visible = local_player->is_visible_with_smoke_check(entity, HITBOX_CHEST);

			if (!config.glow_invisible && !is_visible)
				continue;

			glow_object.glow_style = is_visible ? config.glow_style : 0;
			glow_object.set(get_player_color(local_player, entity, is_visible));
		}		
		else if (class_id == classids::get(XORSTR("CChicken"))) {

			if (!config.glow_world_enable)
				continue;

			if (!config.glow_chicken)
				continue;

			glow_object.set(Color(config.color_glow_chicken));
		}
		else if (class_id == classids::get(XORSTR("CBaseAnimating"))) {

			if (!config.glow_world_enable)
				continue;

			if (!config.glow_defuses)
				continue;

			glow_object.set(Color(config.color_glow_defuses));
		}
		else if (class_id == classids::get(XORSTR("CC4"))) {

			if (!config.glow_world_enable)
				continue;

			if (!config.glow_bomb)
				continue;

			glow_object.set(Color(config.color_glow_bomb));
		}
		else if (class_id == classids::get(XORSTR("CPlantedC4"))) {

			if (!config.glow_world_enable)
				continue;

			if (!config.glow_planted_bomb)
				continue;

			glow_object.set(Color(config.color_glow_planted_bomb));
		}
		else {
			if (entity->is_weapon()) {

				if (!config.glow_world_enable)
					continue;

				if (!config.glow_weapons)
					continue;

				glow_object.set(Color(config.color_glow_weapons));
			}
		}
	}
}