#pragma once

#include "../../utils/entity.h"

namespace glow {
	Color get_player_color(C_BasePlayer * local_player, C_BasePlayer * entity, bool visible);
	void on_post_screen(C_BasePlayer * local_player);
}