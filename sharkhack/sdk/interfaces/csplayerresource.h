#pragma once

enum team_id {
	team_unassigned,
	team_spectator,
	team_terrorist,
	team_counter_terrorist,
};

class CSPlayerResource
{
public:
	int* get_active_coin_rank(int index);
	int* get_music_id(int index);
	team_id get_team(int index);
};