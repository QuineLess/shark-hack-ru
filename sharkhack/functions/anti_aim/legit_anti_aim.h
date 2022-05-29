#pragma once
#include "../../utils/entity.h"
#include "../../utils/math.h"

namespace legit_anti_aim {
	void update_lby( C_CSGOPlayerAnimationState* anim_state);
	void on_move(C_BasePlayer * local_player, QAngle old_angles, CUserCmd * cmd, bool* send_packet);
	void on_draw(C_BasePlayer * local_player);
}