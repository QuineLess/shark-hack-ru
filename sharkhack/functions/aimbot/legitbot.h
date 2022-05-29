#pragma once

#include "../../utils/entity.h"
#include "../../utils/render.h"
#include "../../utils/math.h"

#include "autowal2l.h"

namespace legitbot {
	extern int weapon_index_data[34];
	extern const char* weapon_data[34];
	int get_weapon_select(C_BasePlayer * local_player);
	bool is_valid(C_BasePlayer * local_player);
	float get_player_fov(C_BasePlayer * local_player);
	float get_player_silent_fov(C_BasePlayer * local_player);
	void on_draw(C_BasePlayer * local_player);
	float get_player_smooth(C_BasePlayer * local_player);
	void get_player_rcs(C_BasePlayer * local_player, QAngle & angle, C_BasePlayer * entity);
	bool check_player_fov(Vector screen, float fov);
	void check_player_smooth(QAngle & view_angles, QAngle angles, QAngle & out_angles, float smoothing);
	int get_best_target(C_BasePlayer * local_player);
	int get_best_hitbox(C_BasePlayer * local_player);
	void on_events(C_BasePlayer * local_player, IGameEvent * event);
	bool on_move(C_BasePlayer * local_player, CUserCmd * cmd);
}