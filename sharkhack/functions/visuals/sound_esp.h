#pragma once

#include "../../utils/entity.h"
#include "../../utils/render.h"
#include "../../utils/math.h"

#include <corecrt_math_defines.h>

namespace sound_esp {
	void on_draw(C_BasePlayer * local_player);
	void frame_update(C_BasePlayer * local_player);

	struct sound_info {
		int guid;
		float time;
		Vector pos;
		float alpha;
		bool is_enemy;
	};

	static std::unordered_map<int, std::vector<sound_info>> m_sound_list;
}