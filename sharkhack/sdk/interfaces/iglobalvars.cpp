#include "../interfaces.h"
#include "../../utils/entity.h"

float CGlobalVarsBase::server_time(CUserCmd* cmd) {
	static int tick;
	static CUserCmd* last_cmd;
	const auto local_player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(g_engine->GetLocalPlayer()));

	if (cmd) {
		if (!last_cmd || last_cmd->hasbeenpredicted)
			tick = local_player->get_tick_base();
		else
			tick++;
		last_cmd = cmd;
	}
	return tick * interval_per_tick;
}

C_BasePlayer* CGlobalVarsBase::GetLocalPlayer( )
{
	IClientEntity* LocalPlayer = g_ent_list->GetClientEntity( g_engine->GetLocalPlayer( ) );
	if ( !LocalPlayer )
		return nullptr;

	return static_cast < C_BasePlayer* > ( LocalPlayer );
}