#include "csplayerresource.h"
#include "../../utils/netvars.h"

int* CSPlayerResource::get_active_coin_rank(int index) {
	return (int*)((uintptr_t)this + netvars::get_offset("DT_CSPlayerResource", "m_nActiveCoinRank") + index * 4);
}

int* CSPlayerResource::get_music_id(int index) {
	return (int*)((uintptr_t)this + netvars::get_offset("DT_CSPlayerResource", "m_nMusicID") + index * 4);
}

team_id CSPlayerResource::get_team(int index) {
	return *(team_id*)((uintptr_t)this + netvars::get_offset("DT_PlayerResource", "m_iTeam") + index * 4);
}