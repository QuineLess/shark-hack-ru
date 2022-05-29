#pragma once

#include "../../utils/entity.h"
#include "../../utils/render.h"
#include "../../utils/hooks.h"
#include "../../utils/math.h"

namespace misc {
	void auto_strafe(C_BasePlayer* local_player, CUserCmd* cmd);
	void bunny_hop(C_BasePlayer* local_player, CUserCmd* cmd);
	void on_move(C_BasePlayer * local_player, CUserCmd * cmd );
	void override_view(C_BasePlayer * local_player, CViewSetup * p_setup);
	void view_model_fov(C_BasePlayer * local_player, float & fov);
	void edge_jump(C_BasePlayer* local_player, CUserCmd* cmd);
	void fast_ladder(C_BasePlayer* local_player, CUserCmd* cmd);
	void ragdoll();
	void auto_defuse(C_BasePlayer* local_player, CUserCmd* cmd);
	void radar();
	void clan_tag();
	bool change_name(bool reconnect, const char * new_name, float delay);
	void name_stealer(C_BasePlayer * local_player);
	void spectator_list();
	bool unlock_all_achievements(FILE * fp);
	void get_match_cookie();
	void reconnect_to_on_going_match();
	void third_person(C_BasePlayer * local_player);
	void disable_post_processing();
	void thunder();
	inline float_t m_flThirdpersonDistance = 0.0f;
	inline uint64_t cookie;
}