#include "utils.h"
#include "../sdk/interfaces.h"
#include <random>

#include <Psapi.h>
#pragma comment(lib,"psapi")

uint8_t* utils::find_signature(const char* module, const char* signature) {
	static auto pattern_to_byte = [](const char* pattern) {

		auto bytes = std::vector<int>{};
		auto start = const_cast<char*>(pattern);
		auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current) {

			if (*current == '?') {
				++current;

				if (*current == '?')
					++current;

				bytes.push_back(-1);
			}
			else
				bytes.push_back(strtoul(current, &current, 16));
		}
		return bytes;
	};

	const auto module_handle = GetModuleHandleA(module);
	if (!module_handle)
		return {};

	MODULEINFO module_info;
	GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(module_handle), &module_info, sizeof(MODULEINFO));

	const auto image_size = module_info.SizeOfImage;

	if (!image_size)
		return {};

	auto pattern_bytes = pattern_to_byte(signature);

	const auto image_bytes = reinterpret_cast<unsigned char*>(module_handle);

	const auto signature_size = pattern_bytes.size();
	const auto signature_bytes = pattern_bytes.data();

	for (auto i = 0ul; i < image_size - signature_size; ++i) {
		auto byte_sequence_found = true;

		for (auto j = 0ul; j < signature_size; ++j) {
			if (image_bytes[i + j] != signature_bytes[j] && signature_bytes[j] != -1) {
				byte_sequence_found = false;
				break;
			}
		}

		if (byte_sequence_found)
			return &image_bytes[i];
	}

	return {};
}

std::string utils::to_utf8(std::wstring_view wstr) {
	if (wstr.empty())
		return std::string();

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);

	std::string out(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), &out[0], size_needed, NULL, NULL);

	return out;
}

std::string utils::localize_text(const char* in) {
	const auto wide_name = g_localize->FindSafe(in);

	if (!wcslen(wide_name))
		return "";

	return to_utf8(wide_name);
}

void utils::to_clipboard(HWND hwnd, const std::string &s) {
	OpenClipboard(hwnd);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
	if (!hg) {
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}

std::string utils::random_string(int length) {
	static constexpr char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZbcdefghijklmnopqrstuvwxyz1234567890";
	static std::random_device rd;
	static std::uniform_int_distribution<> dist(0, _countof(alphabet) - 2);
	static std::uniform_int_distribution<> dist_len(5, 15);
	std::string result;

	if (length == 0)
		length = dist_len(rd);

	for (int i = 0; i < length; i++)
		result.push_back(alphabet[dist(rd)]);

	return result;
}

int utils::rgb_to_int(int red, int green, int blue) {
	int c = red;
	c = (c << 8) | green;
	c = (c << 8) | blue;
	return c;
}

DWORD utils::find_hud_element(const char* name) {
	typedef DWORD(__thiscall* find_hud_element_t)(void*, const char*);
	PDWORD _this = *(DWORD**)memory.element_this;
	find_hud_element_t find_hud_element = (find_hud_element_t)memory.element_fn;
	DWORD dw_buff = find_hud_element(_this, name);
	return dw_buff;
}

bool utils::key_bind(int key, int select_mode)
{
	if (key <= 0)
	{
		if (select_mode == 3)
			return true;

		return false;
	}

	switch (select_mode)
	{
	case 0:
		return toggled_keys[key];
	case 1:
		return holded_keys[key];
	case 2:
		return !holded_keys[key];
	case 3:
		return true;
	}

	return false;
}

bool utils::is_key_pressed(int key) {
	static bool key_press[256];

	if (GetAsyncKeyState(key)) {
		if (!key_press[key]) {
			key_press[key] = true;
			return true;
		}
	}
	else
		key_press[key] = false;

	return false;
}

bool utils::is_danger_zone() {
	return g_game_types->GetCurrentGameType() == 6;
}

float utils::random_float(float min, float max) {
	using fn = float(*)(float, float);
	auto procedure = (fn)(g_random_float);

	return (procedure ? procedure(min, max) : 0.0f);
}

void utils::random_seed(unsigned int seed) {
	using fn = void(*)(unsigned int);
	auto procedure = (fn)(g_random_seed);

	if (procedure)
		procedure(seed);
}

int utils::random(int min, int max) {
	return min + rand() % (max - min);
}

bool utils::send_client_rank_update_zone()
{
	writer rank_dz(7);
	rank_dz.replace(field(PlayerRankingInfo::rank_type_id, TYPE_UINT32, (int64_t)10));

	writer msg(1);
	msg.replace(field(ClientGCRankUpdate::ranking, TYPE_STRING, rank_dz.serialize()));

	auto packet = msg.serialize();
	void* ptr = malloc(packet.size() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCCStrike15_v2_ClientGCRankUpdate | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
	bool result = g_steam_game_coordinator->GCSendMessage(k_EMsgGCCStrike15_v2_ClientGCRankUpdate | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
	free(ptr);

	return result;
}

bool utils::send_client_rank_update_wingman()
{
	writer rank_wm(7);
	rank_wm.replace(field(PlayerRankingInfo::rank_type_id, TYPE_UINT32, (int64_t)7));

	writer msg(1);
	msg.replace(field(ClientGCRankUpdate::ranking, TYPE_STRING, rank_wm.serialize()));

	auto packet = msg.serialize();
	void* ptr = malloc(packet.size() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCCStrike15_v2_ClientGCRankUpdate | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
	bool result = g_steam_game_coordinator->GCSendMessage(k_EMsgGCCStrike15_v2_ClientGCRankUpdate | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
	free(ptr);

	return result;
}

bool utils::send_matchmaking_client_hello()
{
	writer msg(0);
	auto packet = msg.serialize();
	void* ptr = malloc(packet.size() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
	bool result = g_steam_game_coordinator->GCSendMessage(k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
	free(ptr);

	return result;
}


IUIPanel* utils::get_root() {
	auto eng = g_panorama_engine->access_ui_engine();
	auto panel = eng->get_last_target_panel();

	IUIPanel* itr = panel;
	IUIPanel* ret = nullptr;

	while (itr && g_panorama_engine->access_ui_engine()->is_valid_panel_ptr(itr)) {
		if (!strcmp(itr->get_panel_id(), "CSGOHud")) {
			ret = itr;
			break;
		}
		else if (!strcmp(itr->get_panel_id(), "CSGOMainMenu")) {
			ret = itr;
			break;
		}

		itr = itr->get_parent();
	}
	return ret;
}

void utils::run_script(const char* code) {
	if (!menu_root) {
		IUIPanel* panel = get_root();

		if (!panel)
			return;

		menu_root = panel;
	}

	IUIPanel* root = menu_root;
	g_panorama_engine->access_ui_engine()->run_script(root, code, "panorama/layout/base_mainmenu.xml", 8, 10, false, false);
}

bool utils::line_goes_through_smoke(Vector start, Vector end) {
	static auto line_goes_through_smoke_fn = (bool(*)(Vector start, Vector end))(memory.get_line_goes_through_smoke);
	return line_goes_through_smoke_fn(start, end);
}