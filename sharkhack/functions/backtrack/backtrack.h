#pragma once

#include "../../utils/entity.h"
#include "../../utils/math.h"
#include <deque>

#define TIME_TO_TICKS(time) ((int)(0.5 + float(time) / g_globals->interval_per_tick))

struct backtrack_data {
	float simtime;
	Vector hitbox_position;
	matrix3x4_t bone_matrix[128];
};

namespace backtrack {
	void frame_update(C_BasePlayer * local_player);
	void on_move(C_BasePlayer * local_player, CUserCmd * cmd);
};

inline std::deque<backtrack_data> records[65];
