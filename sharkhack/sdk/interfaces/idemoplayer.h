#pragma once

class DemoPlaybackParameter_t {
private:
	uint8_t	_pad0[0x10];	
public:
	uint32_t	m_is_overwatch;		
private:
	uint8_t	_pad1[0x88];	
}; 

class CDemoFile {
public:
	char* get_header_id(void) {
		return reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(this) + 0x108);
	}
};

class IDemoPlayer {
public:
	CDemoFile* get_demo_file(void) {
		return *reinterpret_cast<CDemoFile**>(reinterpret_cast<std::uintptr_t>(this) + 0x4);
	}

	DemoPlaybackParameter_t* get_playback_parameter(void) {
		return *reinterpret_cast<DemoPlaybackParameter_t**>(reinterpret_cast<std::uintptr_t>(this) + 0x5C8);
	}

	bool is_in_overwatch(void) {
		auto playback_parameter = get_playback_parameter();
		return (playback_parameter && playback_parameter->m_is_overwatch);
	}

	void set_overwatch_state(bool state) {
		auto playback_parameter = get_playback_parameter();
		if (playback_parameter) {
			playback_parameter->m_is_overwatch = state;
		}
	}

	bool is_playing_demo(void) {
		return CallVFunction<bool(__thiscall*)(void*)>(this, 4)(this);
	}

	bool is_playing_time_demo(void) {
		return CallVFunction<bool(__thiscall*)(void*)>(this, 6)(this);
	}
};