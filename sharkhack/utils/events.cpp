#include "events.h"
#include "entity.h"

#include "../functions/visuals/bullet_tracers.h"
#include "../functions/visuals/visuals.h"
#include "../functions/aimbot/legitbot.h"
#include "../functions/changer/skins.h"

eventlistner::listener m_listener;
std::vector<std::pair<uint32_t, std::function<void(IGameEvent*)>>> m_callbacks = {};

void eventlistner::listener::FireGameEvent(IGameEvent* event) {
	for (const auto& callback : m_callbacks) {
		if (get_hash(event->GetName()) == callback.first)
			callback.second(event);
	}
}

void eventlistner::undo() {
	g_game_events->RemoveListener(&m_listener);
}

void eventlistner::add(const std::string_view event_name, const std::function<void(IGameEvent * e)> & callback) {
	m_callbacks.push_back(std::make_pair(get_hash(event_name.data()), callback));
	g_game_events->AddListener(&m_listener, event_name.data(), false);
}

void player_hurt(IGameEvent* event) {

	const auto local_player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(g_engine->GetLocalPlayer()));
	visuals::on_events_damage_indicator(event);
	visuals::on_events_hit_marker(event);
	visuals::draw_hit_hitbox(event);
	if (config.visuals_hit_effect && local_player && local_player->is_alive()) {
		if (auto attacker_id = g_engine->GetPlayerForUserID(event->GetInt("attacker")) == g_engine->GetLocalPlayer())
			local_player->get_health_boost_time() = g_globals->curtime + 0.5f;
	}
}

void bullet_impact(IGameEvent* event) {
	bullet_tracers::on_events(event);
}

void player_death(IGameEvent * event) {
	const auto local_player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(g_engine->GetLocalPlayer()));
	legitbot::on_events(local_player, event);
}

void eventlistner::on_init() {
	get_event_callback(player_hurt);
	get_event_callback(bullet_impact);
	get_event_callback(player_death);
}