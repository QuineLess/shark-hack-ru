#pragma once

#include "vector.h"
#include "../utils/cutilvector.h"

struct sound_info_t {
	int guid;
	void* filename_handle;
	int sound_source;
	int channel;
	int speaker_entity;
	float volume;
	float last_spatialized_volume;
	float radius;
	int pitch;
	Vector* origin;
	Vector* direction;
	bool update_positions;
	bool is_sentence;
	bool dry_mix;
	bool speaker;
	bool special_dsp;
	bool from_server;
};

class IEngineSound
{
public:
	void GetActiveSounds(CUtlVector<sound_info_t>& sndlist) {
		return CallVFunction<void(__thiscall*)(void*, CUtlVector<sound_info_t>&)>(this, 19)(this, sndlist);
	}
};