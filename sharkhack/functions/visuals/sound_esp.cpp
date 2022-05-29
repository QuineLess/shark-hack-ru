#include "sound_esp.h"

void sound_esp::on_draw(C_BasePlayer* local_player) {

	if (!config.esp_sound_enable)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected() || !local_player)
		return;

	for (auto&[entity_index, sound] : m_sound_list) {
		if (sound.empty())
			continue;

		for (auto& info : sound) {

			static float esp_sound_time = 1.0f;

			if (info.time + esp_sound_time < g_globals->realtime)
				info.alpha -= g_globals->frametime;

			if (info.alpha <= 0.0f)
				continue;

			float delta_time = g_globals->realtime - info.time;
			auto factor = delta_time / esp_sound_time;
			if (factor > 1.0f)
				factor = 1.0f;

			float radius = config.esp_sound_size * factor;

			float precision = 40.0f;

			const float step = (float(M_PI) * 2) / precision;

			for (float a = 0.f; a < (float(M_PI) * 2); a += step) {
				Vector start = { radius * cosf(a) + info.pos.x, radius * sinf(a) + info.pos.y, info.pos.z };
				Vector end = { radius * cosf(a + step) + info.pos.x, radius * sinf(a + step) + info.pos.y, info.pos.z };

				Vector start_screen, end_screen;
				if (!math::world_to_screen(start, start_screen) || !math::world_to_screen(end, end_screen))
					continue;

				Color color = Color(config.color_esp_sound);

				color._CColor[3] = int(info.alpha * 255);

				render::render_line_float(start_screen.x, start_screen.y, end_screen.x, end_screen.y, color, 1.5f);
			}
		}

		while (!sound.empty()) {
			auto& back = sound.back();
			if (back.alpha <= 0.0f)
				sound.pop_back();
			else
				break;
		}
	}
}

void sound_esp::frame_update(C_BasePlayer* local_player) {

	if (!config.esp_enable || !config.esp_sound_enable)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected() || !local_player)
		return;

	CUtlVector<sound_info_t> sounds;

	g_sound->GetActiveSounds(sounds);
	if (!sounds.Count())
		return;

	for (auto i = 0; i < sounds.Count(); i++) {
		const auto& sound = sounds[i];

		if (!sound.sound_source)
			continue;

		if (sound.channel != 4)
			continue;

		auto player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(sound.sound_source));

		if (!player)
			continue;

		if (const auto entity = reinterpret_cast<C_BaseEntity*>(g_ent_list->GetClientEntityFromHandle(player->get_owner_entity())); entity && entity->is_weapon())
			player = reinterpret_cast<C_BasePlayer*>(entity);

		if (!player->is_alive() || player->GetClientClass()->m_ClassID != classids::get(XORSTR("CCSPlayer")))
			continue;

		if (player == local_player || player->is_teammate(local_player))
			continue;

		auto& player_sounds = m_sound_list[player->EntIndex()];
		if (!player_sounds.empty()) {
			auto should_break = false;
			for (const auto& player_sound : player_sounds) {
				if (player_sound.guid == sound.guid) {
					should_break = true;
					break;
				}
			}

			if (should_break)
				continue;
		}

		auto& new_sound = player_sounds.emplace_back();
		new_sound.guid = sound.guid;
		new_sound.pos = *sound.origin;
		new_sound.time = g_globals->realtime;
		new_sound.alpha = 1.0f;
		new_sound.is_enemy = !player->is_teammate(local_player);
	}
}
