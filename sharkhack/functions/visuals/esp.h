#pragma once

#include "../../utils/entity.h"
#include "../../utils/render.h"
#include "../../utils/math.h"

namespace player_esp {
	Color get_color(C_BasePlayer * local_player, C_BasePlayer * entity, bool visible);
	void on_draw(C_BasePlayer * local_player);
}

namespace world_esp {
	void get_esp(C_BaseEntity * entity, const char * name, bool enable, bool box, Color color, bool icon = false);
	void on_draw(C_BasePlayer * local_player);
}

enum health_pos {
	health_pos_left_1,
	health_pos_left_2,
	health_pos_right_1,
	health_pos_right_2,
	health_pos_top_1,
	health_pos_top_2,
	health_pos_bottom_1,
	health_pos_bottom_2
};

enum armor_pos {
	armor_pos_left_1,
	armor_pos_left_2,
	armor_pos_right_1,
	armor_pos_right_2,
	armor_pos_top_1,
	armor_pos_top_2,
	armor_pos_bottom_1,
	armor_pos_bottom_2
};

enum name_pos {
	name_pos_top_1,
	name_pos_top_2,
	name_pos_top_3,
	name_pos_left_1,
	name_pos_left_2,
	name_pos_left_3,
	name_pos_right_1,
	name_pos_right_2,
	name_pos_right_3
};

enum weapon_pos {
	weapon_pos_left_1,
	weapon_pos_left_2,
	weapon_pos_left_3,
	weapon_pos_bottom_1,
	weapon_pos_bottom_2,
	weapon_pos_bottom_3,
	weapon_pos_right_1,
	weapon_pos_right_2,
	weapon_pos_right_3
};
