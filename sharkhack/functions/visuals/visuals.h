#pragma once

#include "../../utils/entity.h"
#include "../../utils/render.h"
#include "../../utils/math.h"

#include <deque>

struct damage_indicator {
	damage_indicator(int p_hp, float p_time, Vector p_bone) {
		hp = p_hp;
		time = p_time;
		bone = p_bone;
	}

	int hp;
	float time;
	Vector bone;
};

inline std::deque<damage_indicator> damage_indicator_log;

namespace visuals {
	void no_flash(C_BasePlayer* local_player);
	void set_sky_box(C_BasePlayer * local_player, const std::string sky_box_name);
	void no_smoke(C_BasePlayer * local_player);
	void set_sky_box(C_BasePlayer* local_player, const std::string SkyboxName);
	void dynamic_light(C_BasePlayer * local_player);
	void draw_spread_circle(C_BasePlayer * local_player);
	void fog_controller();
	void draw_taser_range(C_BasePlayer * local_player);
	void add_damage_indicator(damage_indicator hit_log);
	void draw_damage_indicator(C_BasePlayer * local_player);
	void on_events_damage_indicator(IGameEvent * event);
	void draw_hit_marker(C_BasePlayer * local_player);
	void on_events_hit_marker(IGameEvent * event);
	void sniper_crosshair(C_BasePlayer * local_player);
	void draw_hit_hitbox(IGameEvent * event);
	void draw_player_circle(C_BasePlayer * local_player);
	void bomb_timer();
	void night_mode(C_BasePlayer* local_player);
}