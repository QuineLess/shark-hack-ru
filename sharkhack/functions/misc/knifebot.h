#pragma once

#include "../../utils/entity.h"
#include "../../utils/math.h"

namespace knifebot {
	bool get_best_target(C_BasePlayer * local_player);
	void on_move(C_BasePlayer * local_player, CUserCmd * cmd);
}