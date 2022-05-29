#pragma once

#include <Windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <array>

using namespace std;
struct IUIPanel;
class Vector;

namespace utils {
	uint8_t * find_signature(const char * module, const char * signature);
	std::string to_utf8(std::wstring_view wstr);
	std::string localize_text(const char * in);
	void to_clipboard(HWND hwnd, const std::string & s);
	std::string random_string(int length = 0);
	int rgb_to_int(int red, int green, int blue);
	DWORD find_hud_element(const char * name);
	bool key_bind(int key, int select_mode);
	bool is_key_pressed(int key);
	bool is_danger_zone();
	float random_float(float min, float max);
	void random_seed(unsigned int seed);
	int random(int min, int max);
	bool send_client_rank_update_zone();
	bool send_client_rank_update_wingman();
	bool send_matchmaking_client_hello();
	IUIPanel* get_root();
	void run_script(const char* code);
	bool line_goes_through_smoke(Vector start, Vector end);

	template <typename T>
	T make_ptr(const void* base, int32_t offset = 0) {
		return reinterpret_cast<T>((uintptr_t)base + offset);
	}

	constexpr uint64_t BASIS = 0x811c9dc5;
	constexpr uint64_t PRIME = 0x1000193;

	inline constexpr uint32_t get_const(const char* txt, const uint32_t value = BASIS) noexcept {
		return (txt[0] == '\0') ? value :
			get_const(&txt[1], (value ^ uint32_t(txt[0])) * PRIME);
	}

	inline uint32_t get(const char* txt) {
		uint32_t ret = BASIS;
		uint32_t length = strlen(txt);
		for (auto i = 0u; i < length; ++i) {
			ret ^= txt[i];
			ret *= PRIME;
		}

		return ret;
	}

	inline int cur_weapon;
	inline IUIPanel* menu_root;
	inline std::array< bool, 256 > holded_keys = { };
	inline std::array< bool, 256 > toggled_keys = { };
}

#define get_hash( str ) utils::get( str )