#pragma once

#include "../../utils/entity.h"
#include "../../utils/render.h"
#include "../../utils/math.h"

#define time_to_ticks( dt )		( (int)( 0.5f + (float)(dt) / g_globals->interval_per_tick ) )

enum act {
	act_none,
	act_throw,
	act_lob,
	act_drop,
};

namespace grenade_prediction {
	void on_move(C_BasePlayer * local_player, int buttons);
	void override_view(C_BasePlayer * local_player, CViewSetup * setup);
	void on_draw(C_BasePlayer * local_player);
}