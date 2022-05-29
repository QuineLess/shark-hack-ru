#include "gui.h"
#include "font.h"
#include "image.h"

#include "../functions/visuals/esp.h"
#include "../functions/changer/skins.h"
#include "../functions/aimbot/legitbot.h"
#include "../functions/misc/misc.h"
#include "../functions/games/snake.h"
#include "../functions/visuals/preview.h"
#include "../functions/lobby/lobby.h"

WNDPROC o_wnd_proc = nullptr;
bool gui::menu_open = false;
HWND window = NULL;

ImFont* roboto_bold_tabs = nullptr;
ImFont* roboto_medium_menu = nullptr;
ImFont* weapon_icons_menu = nullptr;

IDirect3DTexture9* ragebot_image = nullptr;
IDirect3DTexture9* antiaim_image = nullptr;
IDirect3DTexture9* legitbot_image = nullptr;
IDirect3DTexture9* player_esp_image = nullptr;
IDirect3DTexture9* world_esp_image = nullptr;
IDirect3DTexture9* glow_and_chams_image = nullptr;
IDirect3DTexture9* inventory_image = nullptr;
IDirect3DTexture9* profile_image = nullptr;
IDirect3DTexture9* misc_image = nullptr;
IDirect3DTexture9* settings_image = nullptr;
IDirect3DTexture9* games_image = nullptr;
IDirect3DTexture9* settings_misc_image = nullptr;
IDirect3DTexture9* modulation_misc_image = nullptr;
IDirect3DTexture9* king_misc_image = nullptr;
IDirect3DTexture9 *m_skin_texture = nullptr;
IDirect3DTexture9 *m_sticker_texture[4] = { nullptr, nullptr, nullptr, nullptr };

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

ImColor get_color(Color color) { return ImColor(color.r(), color.g(), color.b(), color.a()); }
ImVec4 color_clear(0.f, 0.f, 0.f, 0.f);
ImVec4 color_white(1.f, 1.f, 1.f, 1.f);

void gui::styles() {
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = color_white;
	style.Colors[ImGuiCol_TextDisabled] = color_white;
	style.Colors[ImGuiCol_Border] = ImGui::ColConvertToFloat(74.f, 156.f, 239.f);
	style.Colors[ImGuiCol_BorderShadow] = color_clear;
	style.Colors[ImGuiCol_FrameBg] = ImGui::ColConvertToFloat(34.f, 52.f, 76.f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.752f, 0.094f, 0.113f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.752f, 0.094f, 0.113f, 0.40f);
	style.Colors[ImGuiCol_TitleBg] = ImGui::ColConvertToFloat(74.f, 156.f, 239.f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImGui::ColConvertToFloat(74.f, 156.f, 239.f);
	style.Colors[ImGuiCol_TitleBgActive] = ImGui::ColConvertToFloat(74.f, 156.f, 239.f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.09f, 0.12f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(1.f, 1.f, 1.f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImGui::ColConvertToFloat(74.f, 156.f, 239.f);
	style.Colors[ImGuiCol_ButtonHovered] = ImGui::ColConvertToFloat(74.f, 156.f, 239.f, 190.f);
	style.Colors[ImGuiCol_ButtonActive] = ImGui::ColConvertToFloat(74.f, 156.f, 239.f, 210.f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.752f, 0.094f, 0.113f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.752f, 0.094f, 0.113f, 0.74f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.752f, 0.094f, 0.113f, 0.84f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.86f, 0.86f, 0.86f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.752f, 0.094f, 0.113f, 0.84f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.752f, 0.094f, 0.113f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.13f, 0.91f, 0.71f, 0.84f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.20f, 0.72f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.13f, 0.91f, 0.71f, 0.84f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	//Checkbox
	style.Colors[ImGuiCol_Checkbox] = color_white;
	style.Colors[ImGuiCol_CheckboxActive] = style.Colors[ImGuiCol_Checkbox];
	style.Colors[ImGuiCol_CheckboxFrame] = ImGui::ColConvertToFloat(34.f, 52.f, 76.f);
	style.Colors[ImGuiCol_CheckboxFrameActive] = ImGui::ColConvertToFloat(74.f, 156.f, 239.f);

	//Slider
	style.Colors[ImGuiCol_SliderGrab] = style.Colors[ImGuiCol_CheckboxFrameActive];
	style.Colors[ImGuiCol_SliderGrabActive] = color_white;

	//Window
	style.Alpha = 1.f;
	style.WindowPadding = ImVec2(0, 0);
	style.WindowMinSize = ImVec2(20, 20);
	style.WindowRounding = 4.f;
	style.Colors[ImGuiCol_WindowBg] = ImGui::ColConvertToFloat(28.f, 50.f, 74.f);

	//Child
	style.ChildBorderSize = 0.f;
	style.ChildRounding = 0.f;
	style.Colors[ImGuiCol_ChildBg] = ImGui::ColConvertToFloat(37.f, 62.f, 93.f);

	//Combo
	style.ComboButtonSize = ImVec2(style.WindowPadding.x, 30.f);

	//Scrollbar
	style.ScrollbarSize = 3.f;
	style.ScrollbarRounding = 0.f;
	style.Colors[ImGuiCol_ScrollbarBg] = ImGui::ColConvertToFloat(76.f, 76.f, 76.f, 0.f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImGui::ColConvertToFloat(76.f, 76.f, 76.f, 165.f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImGui::ColConvertToFloat(76.f, 76.f, 76.f, 165.f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImGui::ColConvertToFloat(76.f, 76.f, 76.f, 165.f);

	//Shadow
	style.ShadowSize = ImVec2(1.3f, 1.3f);
	style.Colors[ImGuiCol_Shadow] = ImGui::ColConvertToFloat(10.f, 10.f, 10.f, 50.f);

	//Popup
	style.PopupBorderSize = 0.f;
	style.PopupRounding = 0.f;
	style.Colors[ImGuiCol_PopupBg] = ImGui::ColConvertToFloat(37.f, 62.f, 93.f);

	//Separator
	style.Colors[ImGuiCol_Separator] = ImGui::ColConvertToFloat(34.f, 52.f, 76.f);

	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.FramePadding = ImVec2(8, 5);
	style.FrameRounding = 3.f;
	style.ItemSpacing = ImVec2(8, 10);
	style.ItemInnerSpacing = ImVec2(8, 8);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 3.0f;
	style.GrabMinSize = 1.f;
	style.GrabRounding = 0.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.DisplayWindowPadding = ImVec2(22, 22);
	style.DisplaySafeAreaPadding = ImVec2(4, 4);
	style.AntiAliasedLines = true;
	style.CurveTessellationTol = 1.25f;
	style.PopupBorderSize = 0.f;
}

const char* get_language(const char* english, const char* russian) {
	return english;
}

bool gui::wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	/*if (uMsg == WM_SYSKEYUP || uMsg == WM_KEYUP) {
		if (wParam == VK_INSERT)
			menu_open = !menu_open;
	}
	if (menu_open && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;*/

	bool bIsHandlePossible = true;
	if (g_engine->IsConnected() && g_engine->IsInGame())
	{
		if (g_engine->Con_IsVisible())
		{
			bIsHandlePossible = false;
			if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP || uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
				bIsHandlePossible = true;

			if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP)
				if (wParam == VK_INSERT)
					bIsHandlePossible = true;
		}
	}

	if (bIsHandlePossible)
	{
		if (wParam == VK_MENU)
		{
			if (uMsg == WM_SYSKEYDOWN)
			{
				utils::toggled_keys[VK_MENU] = !utils::toggled_keys[VK_MENU];
				utils::holded_keys[VK_MENU] = true;
			}
			else if (uMsg == WM_SYSKEYUP)
				utils::holded_keys[VK_MENU] = false;
		}

		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
			utils::toggled_keys[VK_LBUTTON] = !utils::toggled_keys[VK_LBUTTON];
			utils::holded_keys[VK_LBUTTON] = true;
			break;
		case WM_LBUTTONUP:
			utils::holded_keys[VK_LBUTTON] = false;
			break;
		case WM_XBUTTONDOWN:
			if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
			{
				utils::holded_keys[VK_XBUTTON1] = true;
				utils::toggled_keys[VK_XBUTTON1] = !utils::toggled_keys[VK_XBUTTON1];
			}

			if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
			{
				utils::holded_keys[VK_XBUTTON2] = true;
				utils::toggled_keys[VK_XBUTTON2] = !utils::toggled_keys[VK_XBUTTON2];
			}
			break;
		case WM_XBUTTONUP:
			if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
				utils::holded_keys[VK_XBUTTON1] = false;

			if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
				utils::holded_keys[VK_XBUTTON2] = false;
			break;
		case WM_MBUTTONDOWN:
			utils::holded_keys[VK_MBUTTON] = true;
			utils::toggled_keys[VK_MBUTTON] = !utils::toggled_keys[VK_MBUTTON];
			break;
		case WM_MBUTTONUP:
			utils::holded_keys[VK_MBUTTON] = false;
			break;
		case WM_RBUTTONDOWN:
			utils::toggled_keys[VK_RBUTTON] = !utils::toggled_keys[VK_RBUTTON];
			utils::holded_keys[VK_RBUTTON] = true;
			break;
		case WM_RBUTTONUP:
			utils::holded_keys[VK_RBUTTON] = false;
			break;
		case WM_KEYDOWN:
			utils::toggled_keys[wParam] = !utils::toggled_keys[wParam];
			utils::holded_keys[wParam] = true;
			break;
		case WM_KEYUP:
			utils::holded_keys[wParam] = false;
			break;
		default: break;
		}
	}

	const auto local_player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(g_engine->GetLocalPlayer()));

	if (!local_player)
	{
		for (int i = 0; i < 256; i++)
		{
			if (i == VK_INSERT)
				continue;

			utils::holded_keys[i] = false;
			utils::toggled_keys[i] = false;
		}
	}
	bool bPressedMovementKeys = false;

	if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_LBUTTONUP || uMsg == WM_MOUSEMOVE || uMsg == WM_MOUSEWHEEL)
		bPressedMovementKeys = true;

	if (ImGui::GetIO().WantTextInput)
		bPressedMovementKeys = false;

	menu_open = utils::toggled_keys[VK_INSERT];

	g_input_system->enable_input(!menu_open);

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) && menu_open && !bPressedMovementKeys)
		return true;

	if (menu_open && bPressedMovementKeys)
		return false;

	return false;
}

long __stdcall hk_wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (gui::wnd_proc(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(o_wnd_proc, hWnd, uMsg, wParam, lParam);
}

void gui::on_init() {
	ImGui::CreateContext();

	while (!(window = FindWindowA("Valve001", NULL)))
		std::this_thread::sleep_for(0.2s);

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(g_device);
	o_wnd_proc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (long)hk_wnd_proc);

	styles();

	roboto_bold_tabs = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(roboto_bold_compressed_data, roboto_bold_compressed_size, 9.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	roboto_medium_menu = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(roboto_bold_compressed_data, roboto_bold_compressed_size, 13.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	roboto_bold_elements = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(roboto_bold_compressed_data, roboto_bold_compressed_size, 12.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	roboto_regular_checkbox = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(roboto_regular_compressed_data, roboto_regular_compressed_size, 14.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	weapon_icons = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(weapon_compressed_data, weapon_compressed_size, 18.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	weapon_icons_menu = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(weapon_compressed_data, weapon_compressed_size, 14.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	roboto_medium = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(roboto_medium_compressed_data, roboto_medium_compressed_size, 12.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

	D3DXCreateTextureFromFileInMemory(g_device, &ragebot_icon, sizeof(ragebot_icon), &ragebot_image);
	D3DXCreateTextureFromFileInMemory(g_device, &antiaim_icon, sizeof(antiaim_icon), &antiaim_image);
	D3DXCreateTextureFromFileInMemory(g_device, &legitbot_icon, sizeof(legitbot_icon), &legitbot_image);
	D3DXCreateTextureFromFileInMemory(g_device, &player_esp_icon, sizeof(player_esp_icon), &player_esp_image);
	D3DXCreateTextureFromFileInMemory(g_device, &world_esp_icon, sizeof(world_esp_icon), &world_esp_image);
	D3DXCreateTextureFromFileInMemory(g_device, &glow_and_chams_icon, sizeof(glow_and_chams_icon), &glow_and_chams_image);
	D3DXCreateTextureFromFileInMemory(g_device, &inventory_icon, sizeof(inventory_icon), &inventory_image);
	D3DXCreateTextureFromFileInMemory(g_device, &profile_icon, sizeof(profile_icon), &profile_image);
	D3DXCreateTextureFromFileInMemory(g_device, &misc_icon, sizeof(misc_icon), &misc_image);
	D3DXCreateTextureFromFileInMemory(g_device, &settings_icon, sizeof(settings_icon), &settings_image);
	D3DXCreateTextureFromFileInMemory(g_device, &settings_misc, sizeof(settings_misc), &settings_misc_image);
	D3DXCreateTextureFromFileInMemory(g_device, &modulation_misc, sizeof(modulation_misc), &modulation_misc_image);
	D3DXCreateTextureFromFileInMemory(g_device, &king_misc, sizeof(king_misc), &king_misc_image);
	D3DXCreateTextureFromFileInMemory(g_device, &games_icon, sizeof(games_icon), &games_image);
	D3DXCreateTextureFromFileInMemory(g_device, &arrow, sizeof(arrow), &arrow_texture);
	D3DXCreateTextureFromFileInMemory(g_device, &bomb, sizeof(bomb), &bomb_texture);

	CreateDirectoryA(XORSTR("C:/shark-hack/"), NULL);
	CreateDirectoryA(XORSTR("C:/shark-hack/configs"), NULL);
}

std::unordered_map<ImGuiID, float> animation;

void gui::player_esp_preview() {

	ImVec2 winpos = ImGui::GetWindowPos();
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	static auto box = [](ImVec2 winpos, ImGuiWindow* window) {
		ImVec2 boxsize;
		const auto context{ *GImGui };
		auto mouse_pos = context.IO.MouseDelta;
		if (config.esp_box_type == 2)
			boxsize = ImVec2(154, 244);
		else
			boxsize = ImVec2(155, 245);
		ImVec2 box_start = ImVec2(winpos.x + 60, winpos.y + 40);
		ImVec2 box_end = ImVec2(winpos.x + 60 + boxsize.x, winpos.y + 40 + boxsize.y);

		window->DrawList->AddImage(preview_texture->texture_handles[0]->texture_ptr, { box_start.x - 42.0f,  box_start.y - 50.0f }, { box_start.x + 183.0f,  box_start.y + 245.0f });

		ImGui::SetCursorPos({ 2.0f + ImGui::GetStyle().WindowPadding.x * 3.f, ImGui::GetStyle().WindowPadding.y * 6.6f + 2.0f });
		ImGui::InvisibleButton("preview", { 270.f - ImGui::GetStyle().WindowPadding.x * 6.0f - 5.0f, 207.0f });

		static float_t rotation = 180.0f;

		if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			rotation += mouse_pos.x;

		preview_matrix = ImLerp(preview_matrix, rotation, 4.0f * context.IO.DeltaTime);

		if (config.esp_box) {
			if (config.esp_box_filled) {
				if (config.esp_box_filled_fade)
					window->DrawList->AddRectFilledMultiColor(ImVec2(box_start.x + 1, box_start.y + 1), ImVec2(box_end.x, box_end.y), get_color(Color(0, 0, 0, 0)), get_color(Color(0, 0, 0, 0)), get_color(Color(0, 255, 0, config.esp_box_filled_alpha)), get_color(Color(0, 255, 0, config.esp_box_filled_alpha)));
				else
					window->DrawList->AddRectFilled(ImVec2(box_start.x + 1, box_start.y + 1), ImVec2(box_end.x, box_end.y), get_color(Color(0, 255, 0, config.esp_box_filled_alpha)), (float)config.esp_box_rounding, 15);
			}

			if (config.esp_box_type == 0) {
				if (config.esp_box_outline) {
					window->DrawList->AddRect(ImVec2(box_start.x - 1, box_start.y - 1), ImVec2(box_end.x + 1, box_end.y + 1), get_color(Color::Black()), (float)config.esp_box_rounding, 15, 1.f);
					window->DrawList->AddRect(ImVec2(box_start.x + 1, box_start.y + 1), ImVec2(box_end.x - 1, box_end.y - 1), get_color(Color::Black()), (float)config.esp_box_rounding, 15, 1.f);
				}
				window->DrawList->AddRect(box_start, box_end, get_color(Color::Green()), (float)config.esp_box_rounding, 15, 1.f);
			}
			else if (config.esp_box_type == 1) {
				if (config.esp_box_outline) {
					window->DrawList->AddRect(ImVec2(box_start.x - 1, box_start.y - 1), ImVec2(box_end.x + 1, box_end.y + 1), get_color(Color::Black()), (float)config.esp_box_rounding, 15, 1.f);
					window->DrawList->AddRect(ImVec2(box_start.x + 1, box_start.y + 1), ImVec2(box_end.x - 1, box_end.y - 1), get_color(Color::Black()), (float)config.esp_box_rounding, 15, 1.f);
				}
				window->DrawList->AddRect(box_start, box_end, get_color(Color::Green()), (float)config.esp_box_rounding, 15, 2.f);
			}
			else if (config.esp_box_type == 2) {
				window->DrawList->AddLine(ImVec2(box_start.x, box_start.y), ImVec2(box_start.x, box_start.y + 65.25f), get_color(Color::Green()));	// left top | 
				window->DrawList->AddLine(ImVec2(box_start.x, box_start.y), ImVec2(box_start.x + 41.25f, box_start.y), get_color(Color::Green()));	// left top _

				window->DrawList->AddLine(ImVec2(box_start.x, box_end.y - 65.25f), ImVec2(box_start.x, box_end.y), get_color(Color::Green()));	// left bottom |
				window->DrawList->AddLine(ImVec2(box_start.x, box_end.y), ImVec2(box_start.x + 41.25f, box_end.y), get_color(Color::Green()));	// left bottom _

				window->DrawList->AddLine(ImVec2(box_end.x, box_start.y), ImVec2(box_end.x, box_start.y + 65.25f), get_color(Color::Green()));	// right top | 
				window->DrawList->AddLine(ImVec2(box_end.x - 41.25f, box_start.y), ImVec2(box_end.x, box_start.y), get_color(Color::Green()));	// right top _

				window->DrawList->AddLine(ImVec2(box_end.x, box_end.y - 65.25f), ImVec2(box_end.x, box_end.y), get_color(Color::Green()));	// right bottom | 
				window->DrawList->AddLine(ImVec2(box_end.x - 41.25f, box_end.y), ImVec2(box_end.x + 1, box_end.y), get_color(Color::Green()));// right bottom _
				if (config.esp_box_outline) {
					window->DrawList->AddLine(ImVec2(box_start.x - 1, box_start.y - 1), ImVec2(box_start.x - 1, box_start.y + 66.25f), get_color(Color::Black()));	// left top | 
					window->DrawList->AddLine(ImVec2(box_start.x + 1, box_start.y + 1), ImVec2(box_start.x + 1, box_start.y + 66.25f), get_color(Color::Black()));	// left top   |
					window->DrawList->AddLine(ImVec2(box_start.x - 1, box_start.y + 66.25f), ImVec2(box_start.x + 1, box_start.y + 66.25f), get_color(Color::Black()));	// left top |_|

					window->DrawList->AddLine(ImVec2(box_start.x, box_start.y - 1), ImVec2(box_start.x + 41.25f, box_start.y - 1), get_color(Color::Black()));	// left top _
					window->DrawList->AddLine(ImVec2(box_start.x + 1, box_start.y + 1), ImVec2(box_start.x + 41.25f, box_start.y + 1), get_color(Color::Black()));	// left top _
					window->DrawList->AddLine(ImVec2(box_start.x + 41.25f, box_start.y - 1), ImVec2(box_start.x + 41.25f, box_start.y + 1), get_color(Color::Black()));	// left top _


					window->DrawList->AddLine(ImVec2(box_start.x - 1, box_end.y - 65.25f), ImVec2(box_start.x - 1, box_end.y + 1), get_color(Color::Black()));	// left bottom | 
					window->DrawList->AddLine(ImVec2(box_start.x + 1, box_end.y - 65.25f), ImVec2(box_start.x + 1, box_end.y - 1), get_color(Color::Black()));	// left bottom   |
					window->DrawList->AddLine(ImVec2(box_start.x - 1, box_end.y - 65.25f), ImVec2(box_start.x + 1, box_end.y - 65.25f), get_color(Color::Black()));	// left bottom |_|

					window->DrawList->AddLine(ImVec2(box_start.x - 1, box_end.y + 1), ImVec2(box_start.x + 41.25f, box_end.y + 1), get_color(Color::Black()));	// left bottom _
					window->DrawList->AddLine(ImVec2(box_start.x + 1, box_end.y - 1), ImVec2(box_start.x + 41.25f, box_end.y - 1), get_color(Color::Black()));	// left bottom _
					window->DrawList->AddLine(ImVec2(box_start.x + 41.25f, box_end.y - 1), ImVec2(box_start.x + 41.25f, box_end.y + 1), get_color(Color::Black()));	// left bottom _

					window->DrawList->AddLine(ImVec2(box_end.x - 41.25f, box_start.y + 1), ImVec2(box_end.x, box_start.y + 1), get_color(Color::Black()));	// right top _
					window->DrawList->AddLine(ImVec2(box_end.x - 41.25f, box_start.y - 1), ImVec2(box_end.x + 1, box_start.y - 1), get_color(Color::Black()));	// right top _
					window->DrawList->AddLine(ImVec2(box_end.x - 41.25f, box_start.y - 1), ImVec2(box_end.x - 41.25f, box_start.y + 1), get_color(Color::Black()));	// right top _

					window->DrawList->AddLine(ImVec2(box_end.x - 1, box_start.y + 1), ImVec2(box_end.x - 1, box_start.y + 65.25f), get_color(Color::Black()));	// right top | 
					window->DrawList->AddLine(ImVec2(box_end.x + 1, box_start.y - 1), ImVec2(box_end.x + 1, box_start.y + 65.25f), get_color(Color::Black()));	// right top | 
					window->DrawList->AddLine(ImVec2(box_end.x - 1, box_start.y + 65.25f), ImVec2(box_end.x + 1, box_start.y + 65.25f), get_color(Color::Black()));	// right top | 


					window->DrawList->AddLine(ImVec2(box_end.x - 41.25f, box_end.y - 1), ImVec2(box_end.x, box_end.y - 1), get_color(Color::Black()));	// right bottom _
					window->DrawList->AddLine(ImVec2(box_end.x - 41.25f, box_end.y + 1), ImVec2(box_end.x + 2, box_end.y + 1), get_color(Color::Black()));	// right bottom _
					window->DrawList->AddLine(ImVec2(box_end.x - 41.25f, box_end.y - 1), ImVec2(box_end.x - 41.25f, box_end.y + 1), get_color(Color::Black()));	// right bottom _

					window->DrawList->AddLine(ImVec2(box_end.x - 1, box_end.y - 65.25f), ImVec2(box_end.x - 1, box_end.y - 1), get_color(Color::Black()));	// right bottom |
					window->DrawList->AddLine(ImVec2(box_end.x + 1, box_end.y - 65.25f), ImVec2(box_end.x + 1, box_end.y + 2), get_color(Color::Black()));	// right bottom | 		
					window->DrawList->AddLine(ImVec2(box_end.x - 1, box_end.y - 65.25f), ImVec2(box_end.x + 1, box_end.y - 65.25f), get_color(Color::Black()));	// right bottom | 		
				}
			}
		}

	};

	static auto health = [](ImVec2 winpos, ImGuiWindow* window) {
		ImVec2 healthsize;
		ImVec2 health_start;
		ImVec2 health_end;

		ImGuiID id = window->GetID("position_health");
		ImGuiContext& g = *GImGui;

		if (config.esp_health_pos == health_pos::health_pos_left_1) {
			healthsize = ImVec2(4, 245);
			health_start = ImVec2(winpos.x + 52, winpos.y + 40);
			health_end = ImVec2(winpos.x + 52 + healthsize.x, winpos.y + 40 + healthsize.y);
		}
		else  if (config.esp_health_pos == health_pos::health_pos_left_2) {
			healthsize = ImVec2(4, 245);
			health_start = ImVec2(winpos.x + 45, winpos.y + 40);
			health_end = ImVec2(winpos.x + 45 + healthsize.x, winpos.y + 40 + healthsize.y);
		}
		else if (config.esp_health_pos == health_pos::health_pos_right_1) {
			healthsize = ImVec2(4, 245);
			health_start = ImVec2(winpos.x + 219, winpos.y + 40);
			health_end = ImVec2(winpos.x + 219 + healthsize.x, winpos.y + 40 + healthsize.y);
		}
		else if (config.esp_health_pos == health_pos::health_pos_right_2) {
			healthsize = ImVec2(4, 245);
			health_start = ImVec2(winpos.x + 226, winpos.y + 40);
			health_end = ImVec2(winpos.x + 226 + healthsize.x, winpos.y + 40 + healthsize.y);
		}
		else if (config.esp_health_pos == health_pos::health_pos_top_1) {
			healthsize = ImVec2(155, 4);
			health_start = ImVec2(winpos.x + 60, winpos.y + 32);
			health_end = ImVec2(winpos.x + 60 + healthsize.x, winpos.y + 32 + healthsize.y);
		}
		else if (config.esp_health_pos == health_pos::health_pos_top_2) {
			healthsize = ImVec2(155, 4);
			health_start = ImVec2(winpos.x + 60, winpos.y + 25);
			health_end = ImVec2(winpos.x + 60 + healthsize.x, winpos.y + 25 + healthsize.y);
		}
		else if (config.esp_health_pos == health_pos::health_pos_bottom_1) {
			healthsize = ImVec2(155, 4);
			health_start = ImVec2(winpos.x + 60, winpos.y + 289);
			health_end = ImVec2(winpos.x + 60 + healthsize.x, winpos.y + 289 + healthsize.y);
		}
		else if (config.esp_health_pos == health_pos::health_pos_bottom_2) {
			healthsize = ImVec2(155, 4);
			health_start = ImVec2(winpos.x + 60, winpos.y + 296);
			health_end = ImVec2(winpos.x + 60 + healthsize.x, winpos.y + 296 + healthsize.y);
		}

		const ImRect bb(health_start, health_end);
		bool hovered;
		bool held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

		auto pos = g.IO.MousePos;

		ImVec2 health_start_anim;
		ImVec2 health_end_anim;

		if (config.esp_health_pos >= health_pos::health_pos_left_1 && config.esp_health_pos <= health_pos::health_pos_right_2) {
			auto animation_y = &animation[id];
			ImGui::Anim(animation_y, (g.ActiveId == id), 0.f, (healthsize.y / 2.f), (healthsize.y / 2.f) * 4.f, (healthsize.y / 2.f) * 4.f);
			if (pos.y <= (winpos.y + 40 + healthsize.y / 2.f)) {
				health_start_anim = ImVec2(pos.x - 2.f, pos.y - *animation_y);
				health_end_anim = ImVec2(pos.x + healthsize.x - 2.f, pos.y - *animation_y + healthsize.y);
			}
			else {
				health_start_anim = ImVec2(pos.x - 2.f, pos.y - healthsize.y + *animation_y);
				health_end_anim = ImVec2(pos.x + healthsize.x - 2.f, pos.y - healthsize.y + *animation_y + healthsize.y);
			}
		}
		else if (config.esp_health_pos >= health_pos::health_pos_top_1 && config.esp_health_pos <= health_pos::health_pos_bottom_2) {
			auto animation_x = &animation[id];
			ImGui::Anim(animation_x, (g.ActiveId == id), 0.f, (healthsize.x / 2.f), (healthsize.x / 2.f) * 4.f, (healthsize.x / 2.f) * 4.f);
			if (pos.x <= (winpos.x + 60 + healthsize.x / 2.f)) {
				health_start_anim = ImVec2(pos.x - *animation_x, pos.y - 2.f);
				health_end_anim = ImVec2(pos.x - *animation_x + healthsize.x + 2.f, pos.y - 2.f + healthsize.y);
			}
			else {
				health_start_anim = ImVec2(pos.x - healthsize.x + *animation_x, pos.y - 2.f);
				health_end_anim = ImVec2(pos.x - healthsize.x + *animation_x + healthsize.x + 2.f, pos.y - 2.f + healthsize.y);
			}
		}

		if (config.esp_health_background)
			window->DrawList->AddRect(ImVec2((g.ActiveId == id) ? health_start_anim.x - 1 : health_start.x - 1, (g.ActiveId == id) ? health_start_anim.y - 1 : health_start.y - 1), ImVec2((g.ActiveId == id) ? health_end_anim.x + 1 : health_end.x + 1, (g.ActiveId == id) ? health_end_anim.y + 1 : health_end.y + 1), get_color(Color::Black()));
		if (config.esp_health_pos >= health_pos::health_pos_left_1 && config.esp_health_pos <= health_pos::health_pos_right_2) {
			if (config.esp_health_fade)
				window->DrawList->AddRectFilledMultiColor((g.ActiveId == id) ? health_start_anim : health_start, (g.ActiveId == id) ? health_end_anim : health_end, get_color(Color::Green()), get_color(Color::Green()), get_color(Color::Red()), get_color(Color::Red()));
			else
				window->DrawList->AddRectFilled((g.ActiveId == id) ? health_start_anim : health_start, (g.ActiveId == id) ? health_end_anim : health_end, get_color(Color::Green()));
		}
		else if (config.esp_health_pos >= health_pos::health_pos_top_1 && config.esp_health_pos <= health_pos::health_pos_bottom_2) {
			if (config.esp_health_fade)
				window->DrawList->AddRectFilledMultiColor((g.ActiveId == id) ? health_start_anim : health_start, (g.ActiveId == id) ? health_end_anim : health_end, get_color(Color::Green()), get_color(Color::Red()), get_color(Color::Red()), get_color(Color::Green()));
			else
				window->DrawList->AddRectFilled((g.ActiveId == id) ? health_start_anim : health_start, (g.ActiveId == id) ? health_end_anim : health_end, get_color(Color::Green()));
		}
		if (g.ActiveId == id) {

			float cur_x = pos.x;
			float cur_y = pos.y;

			if (config.esp_armor) {
				if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= (winpos.x + 52) && cur_x <= (winpos.x + 60))
					config.esp_health_pos = health_pos::health_pos_left_1;
				if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= winpos.x && cur_x <= (winpos.x + 52))
					config.esp_health_pos = health_pos::health_pos_left_2;

				if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= (winpos.x + 215) && cur_x <= (winpos.x + 223))
					config.esp_health_pos = health_pos::health_pos_right_1;
				if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= (winpos.x + 226) && cur_x <= (winpos.x + window->Size.x))
					config.esp_health_pos = health_pos::health_pos_right_2;

				if (cur_y >= winpos.y + 32 && cur_y <= winpos.y + 40 && cur_x >= (winpos.x + 60) && cur_x <= (winpos.x + 215))
					config.esp_health_pos = health_pos::health_pos_top_1;
				if (cur_y >= winpos.y && cur_y <= winpos.y + 32 && cur_x >= (winpos.x + 60) && cur_x <= (winpos.x + 215))
					config.esp_health_pos = health_pos::health_pos_top_2;

				if (cur_y >= winpos.y + 289 && cur_y <= winpos.y + 296 && cur_x >= (winpos.x + 60) && cur_x <= (winpos.x + 215))
					config.esp_health_pos = health_pos::health_pos_bottom_1;
				if (cur_y >= winpos.y + 296 && cur_y <= winpos.y + window->Size.y && cur_x >= (winpos.x + 60) && cur_x <= (winpos.x + 215))
					config.esp_health_pos = health_pos::health_pos_bottom_2;
			}
			else {
				if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= winpos.x && cur_x <= (winpos.x + 60))
					config.esp_health_pos = health_pos::health_pos_left_1;

				if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= winpos.x + 215 && cur_x <= (winpos.x + window->Size.x))
					config.esp_health_pos = health_pos::health_pos_right_1;

				if (cur_y >= winpos.y && cur_y <= winpos.y + 40 && cur_x >= (winpos.x + 60) && cur_x <= (winpos.x + 215))
					config.esp_health_pos = health_pos::health_pos_top_1;

				if (cur_y >= winpos.y + 289 && cur_y <= winpos.y + window->Size.y && cur_x >= (winpos.x + 60) && cur_x <= (winpos.x + 215))
					config.esp_health_pos = health_pos::health_pos_bottom_1;
			}

			if (config.esp_armor_pos == armor_pos::armor_pos_left_2 && config.esp_health_pos == health_pos::health_pos_left_2)
				config.esp_armor_pos = armor_pos::armor_pos_left_1;
			if (config.esp_armor_pos == armor_pos::armor_pos_left_1 && config.esp_health_pos == health_pos::health_pos_left_1)
				config.esp_armor_pos = armor_pos::armor_pos_left_2;

			if (config.esp_armor_pos == armor_pos::armor_pos_right_1 && config.esp_health_pos == health_pos::health_pos_right_1)
				config.esp_armor_pos = armor_pos::armor_pos_right_2;
			if (config.esp_armor_pos == armor_pos::armor_pos_right_2 && config.esp_health_pos == health_pos::health_pos_right_2)
				config.esp_armor_pos = armor_pos::armor_pos_right_1;

			if (config.esp_armor_pos == armor_pos::armor_pos_top_2 && config.esp_health_pos == health_pos::health_pos_top_2)
				config.esp_armor_pos = armor_pos::armor_pos_top_1;
			if (config.esp_armor_pos == armor_pos::armor_pos_top_1 && config.esp_health_pos == health_pos::health_pos_top_1)
				config.esp_armor_pos = armor_pos::armor_pos_top_2;

			if (config.esp_armor_pos == armor_pos::armor_pos_bottom_1 && config.esp_health_pos == health_pos::health_pos_bottom_1)
				config.esp_armor_pos = armor_pos::armor_pos_bottom_2;
			if (config.esp_armor_pos == armor_pos::armor_pos_bottom_2 && config.esp_health_pos == health_pos::health_pos_bottom_2)
				config.esp_armor_pos = armor_pos::armor_pos_bottom_1;
		}

		if (config.esp_armor) {
			if (config.esp_armor_pos != armor_pos::armor_pos_left_2 && config.esp_armor_pos != armor_pos::armor_pos_left_1 && config.esp_health_pos == health_pos::health_pos_left_2)
				config.esp_health_pos = health_pos::health_pos_left_1;

			if (config.esp_armor_pos != armor_pos::armor_pos_right_2 && config.esp_armor_pos != armor_pos::armor_pos_right_1 && config.esp_health_pos == health_pos::health_pos_right_2)
				config.esp_health_pos = health_pos::health_pos_right_1;

			if (config.esp_armor_pos != armor_pos::armor_pos_top_2 && config.esp_armor_pos != armor_pos::armor_pos_top_1 && config.esp_health_pos == health_pos::health_pos_top_2)
				config.esp_health_pos = health_pos::health_pos_top_1;

			if (config.esp_armor_pos != armor_pos::armor_pos_bottom_2 && config.esp_armor_pos != armor_pos::armor_pos_bottom_1 && config.esp_health_pos == health_pos::health_pos_bottom_2)
				config.esp_health_pos = health_pos::health_pos_bottom_1;

			if (config.esp_armor_pos == armor_pos::armor_pos_left_2 && config.esp_health_pos == health_pos::health_pos_left_2)
				config.esp_health_pos = health_pos::health_pos_left_1;
			if (config.esp_armor_pos == armor_pos::armor_pos_left_1 && config.esp_health_pos == health_pos::health_pos_left_1)
				config.esp_health_pos = health_pos::health_pos_left_2;

			if (config.esp_armor_pos == armor_pos::armor_pos_right_2 && config.esp_health_pos == health_pos::health_pos_right_2)
				config.esp_health_pos = health_pos::health_pos_right_1;
			if (config.esp_armor_pos == armor_pos::armor_pos_right_1 && config.esp_health_pos == health_pos::health_pos_right_1)
				config.esp_health_pos = health_pos::health_pos_right_2;

			if (config.esp_armor_pos == armor_pos::armor_pos_top_2 && config.esp_health_pos == health_pos::health_pos_top_2)
				config.esp_health_pos = health_pos::health_pos_top_1;
			if (config.esp_armor_pos == armor_pos::armor_pos_top_1 && config.esp_health_pos == health_pos::health_pos_top_1)
				config.esp_health_pos = health_pos::health_pos_top_2;

			if (config.esp_armor_pos == armor_pos::armor_pos_bottom_2 && config.esp_health_pos == health_pos::health_pos_bottom_2)
				config.esp_health_pos = health_pos::health_pos_bottom_1;
			if (config.esp_armor_pos == armor_pos::armor_pos_bottom_1 && config.esp_health_pos == health_pos::health_pos_bottom_1)
				config.esp_health_pos = health_pos::health_pos_bottom_2;
		}
		else {
			if (config.esp_health_pos == health_pos::health_pos_left_2)
				config.esp_health_pos = health_pos::health_pos_left_1;

			if (config.esp_health_pos == health_pos::health_pos_right_2)
				config.esp_health_pos = health_pos::health_pos_right_1;

			if (config.esp_health_pos == health_pos::health_pos_top_2)
				config.esp_health_pos = health_pos::health_pos_top_1;

			if (config.esp_health_pos == health_pos::health_pos_bottom_2)
				config.esp_health_pos = health_pos::health_pos_bottom_1;
		}
	};

	static auto armor = [](ImVec2 winpos, ImGuiWindow* window) {
		ImVec2 armorsize;
		ImVec2 armor_start;
		ImVec2 armor_end;

		ImGuiID id = window->GetID("position_armor");
		ImGuiContext& g = *GImGui;

		if (config.esp_armor_pos == armor_pos::armor_pos_left_1) {
			armorsize = ImVec2(4, 245);
			armor_start = ImVec2(winpos.x + 52, winpos.y + 40);
			armor_end = ImVec2(winpos.x + 52 + armorsize.x, winpos.y + 40 + armorsize.y);
		}
		else  if (config.esp_armor_pos == armor_pos::armor_pos_left_2) {
			armorsize = ImVec2(4, 245);
			armor_start = ImVec2(winpos.x + 45, winpos.y + 40);
			armor_end = ImVec2(winpos.x + 45 + armorsize.x, winpos.y + 40 + armorsize.y);
		}
		else if (config.esp_armor_pos == armor_pos::armor_pos_right_1) {
			armorsize = ImVec2(4, 245);
			armor_start = ImVec2(winpos.x + 219, winpos.y + 40);
			armor_end = ImVec2(winpos.x + 219 + armorsize.x, winpos.y + 40 + armorsize.y);
		}
		else if (config.esp_armor_pos == armor_pos::armor_pos_right_2) {
			armorsize = ImVec2(4, 245);
			armor_start = ImVec2(winpos.x + 226, winpos.y + 40);
			armor_end = ImVec2(winpos.x + 226 + armorsize.x, winpos.y + 40 + armorsize.y);
		}
		else if (config.esp_armor_pos == armor_pos::armor_pos_top_1) {
			armorsize = ImVec2(155, 4);
			armor_start = ImVec2(winpos.x + 60, winpos.y + 32);
			armor_end = ImVec2(winpos.x + 60 + armorsize.x, winpos.y + 32 + armorsize.y);
		}
		else if (config.esp_armor_pos == armor_pos::armor_pos_top_2) {
			armorsize = ImVec2(155, 4);
			armor_start = ImVec2(winpos.x + 60, winpos.y + 25);
			armor_end = ImVec2(winpos.x + 60 + armorsize.x, winpos.y + 25 + armorsize.y);
		}
		else if (config.esp_armor_pos == armor_pos::armor_pos_bottom_1) {
			armorsize = ImVec2(155, 4);
			armor_start = ImVec2(winpos.x + 60, winpos.y + 289);
			armor_end = ImVec2(winpos.x + 60 + armorsize.x, winpos.y + 289 + armorsize.y);
		}
		else if (config.esp_armor_pos == armor_pos::armor_pos_bottom_2) {
			armorsize = ImVec2(155, 4);
			armor_start = ImVec2(winpos.x + 60, winpos.y + 296);
			armor_end = ImVec2(winpos.x + 60 + armorsize.x, winpos.y + 296 + armorsize.y);
		}

		const ImRect bb(armor_start, armor_end);
		bool hovered;
		bool held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

		auto pos = g.IO.MousePos;

		ImVec2 armor_start_anim;
		ImVec2 armor_end_anim;

		if (config.esp_armor_pos >= armor_pos::armor_pos_left_1 && config.esp_armor_pos <= armor_pos::armor_pos_right_2) {
			auto animation_y = &animation[id];
			ImGui::Anim(animation_y, (g.ActiveId == id), 0.f, (armorsize.y / 2.f), (armorsize.y / 2.f) * 4.f, (armorsize.y / 2.f) * 4.f);
			if (pos.y <= (winpos.y + 40 + armorsize.y / 2.f)) {
				armor_start_anim = ImVec2(pos.x - 2.f, pos.y - *animation_y);
				armor_end_anim = ImVec2(pos.x + armorsize.x - 2.f, pos.y - *animation_y + armorsize.y);
			}
			else {
				armor_start_anim = ImVec2(pos.x - 2.f, pos.y - armorsize.y + *animation_y);
				armor_end_anim = ImVec2(pos.x + armorsize.x - 2.f, pos.y - armorsize.y + *animation_y + armorsize.y);
			}
		}
		else if (config.esp_armor_pos >= armor_pos::armor_pos_top_1 && config.esp_armor_pos <= armor_pos::armor_pos_bottom_2) {
			auto animation_x = &animation[id];
			ImGui::Anim(animation_x, (g.ActiveId == id), 0.f, (armorsize.x / 2.f), (armorsize.x / 2.f) * 4.f, (armorsize.x / 2.f) * 4.f);
			if (pos.x <= (winpos.x + 60 + armorsize.x / 2.f)) {
				armor_start_anim = ImVec2(pos.x - *animation_x, pos.y - 2.f);
				armor_end_anim = ImVec2(pos.x - *animation_x + armorsize.x + 2.f, pos.y - 2.f + armorsize.y);
			}
			else {
				armor_start_anim = ImVec2(pos.x - armorsize.x + *animation_x, pos.y - 2.f);
				armor_end_anim = ImVec2(pos.x - armorsize.x + *animation_x + armorsize.x + 2.f, pos.y - 2.f + armorsize.y);
			}
		}

		if (config.esp_armor_background)
			window->DrawList->AddRect(ImVec2((g.ActiveId == id) ? armor_start_anim.x - 1 : armor_start.x - 1, (g.ActiveId == id) ? armor_start_anim.y - 1 : armor_start.y - 1), ImVec2((g.ActiveId == id) ? armor_end_anim.x + 1 : armor_end.x + 1, (g.ActiveId == id) ? armor_end_anim.y + 1 : armor_end.y + 1), get_color(Color::Black()));

		if (config.esp_armor_pos >= armor_pos::armor_pos_left_1 && config.esp_armor_pos <= armor_pos::armor_pos_right_2) {
			if (config.esp_armor_fade)
				window->DrawList->AddRectFilledMultiColor((g.ActiveId == id) ? armor_start_anim : armor_start, (g.ActiveId == id) ? armor_end_anim : armor_end, get_color(Color(10, 140, 200)), get_color(Color(10, 140, 200)), get_color(Color::White()), get_color(Color::White()));
			else
				window->DrawList->AddRectFilled((g.ActiveId == id) ? armor_start_anim : armor_start, (g.ActiveId == id) ? armor_end_anim : armor_end, get_color(Color(10, 140, 200)));
		}
		else if (config.esp_armor_pos >= armor_pos::armor_pos_top_1 && config.esp_armor_pos <= armor_pos::armor_pos_bottom_2) {
			if (config.esp_armor_fade)
				window->DrawList->AddRectFilledMultiColor((g.ActiveId == id) ? armor_start_anim : armor_start, (g.ActiveId == id) ? armor_end_anim : armor_end, get_color(Color(10, 140, 200)), get_color(Color::White()), get_color(Color::White()), get_color(Color(10, 140, 200)));
			else
				window->DrawList->AddRectFilled((g.ActiveId == id) ? armor_start_anim : armor_start, (g.ActiveId == id) ? armor_end_anim : armor_end, get_color(Color(10, 140, 200)));
		}

		if (g.ActiveId == id) {

			float cur_x = pos.x;
			float cur_y = pos.y;

			if (config.esp_armor) {
				if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= (winpos.x + 52) && cur_x <= (winpos.x + 60))
					config.esp_armor_pos = armor_pos::armor_pos_left_1;
				if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= winpos.x && cur_x <= (winpos.x + 52))
					config.esp_armor_pos = armor_pos::armor_pos_left_2;

				if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= (winpos.x + 215) && cur_x <= (winpos.x + 223))
					config.esp_armor_pos = armor_pos::armor_pos_right_1;
				if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= (winpos.x + 226) && cur_x <= (winpos.x + window->Size.x))
					config.esp_armor_pos = armor_pos::armor_pos_right_2;

				if (cur_y >= winpos.y + 32 && cur_y <= winpos.y + 40 && cur_x >= (winpos.x + 60) && cur_x <= (winpos.x + 215))
					config.esp_armor_pos = armor_pos::armor_pos_top_1;
				if (cur_y >= winpos.y && cur_y <= winpos.y + 32 && cur_x >= (winpos.x + 60) && cur_x <= (winpos.x + 215))
					config.esp_armor_pos = armor_pos::armor_pos_top_2;

				if (cur_y >= winpos.y + 289 && cur_y <= winpos.y + 296 && cur_x >= (winpos.x + 60) && cur_x <= (winpos.x + 215))
					config.esp_armor_pos = armor_pos::armor_pos_bottom_1;
				if (cur_y >= winpos.y + 296 && cur_y <= winpos.y + window->Size.y && cur_x >= (winpos.x + 60) && cur_x <= (winpos.x + 215))
					config.esp_armor_pos = armor_pos::armor_pos_bottom_2;
			}
			else {
				if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= winpos.x && cur_x <= (winpos.x + 60))
					config.esp_armor_pos = armor_pos::armor_pos_left_1;

				if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= winpos.x + 215 && cur_x <= (winpos.x + window->Size.x))
					config.esp_armor_pos = armor_pos::armor_pos_right_1;

				if (cur_y >= winpos.y && cur_y <= winpos.y + 40 && cur_x >= (winpos.x + 60) && cur_x <= (winpos.x + 215))
					config.esp_armor_pos = armor_pos::armor_pos_top_1;

				if (cur_y >= winpos.y + 289 && cur_y <= winpos.y + window->Size.y && cur_x >= (winpos.x + 60) && cur_x <= (winpos.x + 215))
					config.esp_armor_pos = armor_pos::armor_pos_bottom_1;
			}

			if (config.esp_health_pos == health_pos::health_pos_left_2 && config.esp_armor_pos == armor_pos::armor_pos_left_2)
				config.esp_health_pos = health_pos::health_pos_left_1;
			if (config.esp_health_pos == health_pos::health_pos_left_1 && config.esp_armor_pos == armor_pos::armor_pos_left_1)
				config.esp_health_pos = health_pos::health_pos_left_2;

			if (config.esp_health_pos == health_pos::health_pos_right_2 && config.esp_armor_pos == armor_pos::armor_pos_right_2)
				config.esp_health_pos = health_pos::health_pos_right_1;
			if (config.esp_health_pos == health_pos::health_pos_right_1 && config.esp_armor_pos == armor_pos::armor_pos_right_1)
				config.esp_health_pos = health_pos::health_pos_right_2;

			if (config.esp_health_pos == health_pos::health_pos_top_2 && config.esp_armor_pos == armor_pos::armor_pos_top_2)
				config.esp_health_pos = health_pos::health_pos_top_1;
			if (config.esp_health_pos == health_pos::health_pos_top_1 && config.esp_armor_pos == armor_pos::armor_pos_top_1)
				config.esp_health_pos = health_pos::health_pos_top_2;

			if (config.esp_health_pos == health_pos::health_pos_bottom_2 && config.esp_armor_pos == armor_pos::armor_pos_bottom_2)
				config.esp_health_pos = health_pos::health_pos_bottom_1;
			if (config.esp_health_pos == health_pos::health_pos_bottom_1 && config.esp_armor_pos == armor_pos::armor_pos_bottom_1)
				config.esp_health_pos = health_pos::health_pos_bottom_2;
		}

		if (config.esp_health) {
			if (config.esp_health_pos != health_pos::health_pos_left_2 && config.esp_health_pos != health_pos::health_pos_left_1 && config.esp_armor_pos == armor_pos::armor_pos_left_2)
				config.esp_armor_pos = armor_pos::armor_pos_left_1;

			if (config.esp_health_pos != health_pos::health_pos_right_2 && config.esp_health_pos != health_pos::health_pos_right_1 && config.esp_armor_pos == armor_pos::armor_pos_right_2)
				config.esp_armor_pos = armor_pos::armor_pos_right_1;

			if (config.esp_health_pos != health_pos::health_pos_top_2 && config.esp_health_pos != health_pos::health_pos_top_1 && config.esp_armor_pos == armor_pos::armor_pos_top_2)
				config.esp_armor_pos = armor_pos::armor_pos_top_1;

			if (config.esp_health_pos != health_pos::health_pos_bottom_2 && config.esp_health_pos != health_pos::health_pos_bottom_1 && config.esp_armor_pos == armor_pos::armor_pos_bottom_2)
				config.esp_armor_pos = armor_pos::armor_pos_bottom_1;

			if (config.esp_health_pos == health_pos::health_pos_left_2 && config.esp_armor_pos == armor_pos::armor_pos_left_2)
				config.esp_armor_pos = armor_pos::armor_pos_left_1;
			if (config.esp_health_pos == health_pos::health_pos_left_1 && config.esp_armor_pos == armor_pos::armor_pos_left_1)
				config.esp_armor_pos = armor_pos::armor_pos_left_2;

			if (config.esp_health_pos == health_pos::health_pos_right_2 && config.esp_armor_pos == armor_pos::armor_pos_right_2)
				config.esp_armor_pos = armor_pos::armor_pos_right_1;
			if (config.esp_health_pos == health_pos::health_pos_right_1 && config.esp_armor_pos == armor_pos::armor_pos_right_1)
				config.esp_armor_pos = armor_pos::armor_pos_right_2;

			if (config.esp_health_pos == health_pos::health_pos_top_2 && config.esp_armor_pos == armor_pos::armor_pos_top_2)
				config.esp_armor_pos = armor_pos::armor_pos_top_1;
			if (config.esp_health_pos == health_pos::health_pos_top_1 && config.esp_armor_pos == armor_pos::armor_pos_top_1)
				config.esp_armor_pos = armor_pos::armor_pos_top_2;

			if (config.esp_health_pos == health_pos::health_pos_bottom_2 && config.esp_armor_pos == armor_pos::armor_pos_bottom_2)
				config.esp_armor_pos = armor_pos::armor_pos_bottom_1;
			if (config.esp_health_pos == health_pos::health_pos_bottom_1 && config.esp_armor_pos == armor_pos::armor_pos_bottom_1)
				config.esp_armor_pos = armor_pos::armor_pos_bottom_2;
		}
		else {
			if (config.esp_armor_pos == armor_pos::armor_pos_left_2)
				config.esp_armor_pos = armor_pos::armor_pos_left_1;

			if (config.esp_armor_pos == armor_pos::armor_pos_right_2)
				config.esp_armor_pos = armor_pos::armor_pos_right_1;

			if (config.esp_armor_pos == armor_pos::armor_pos_top_2)
				config.esp_armor_pos = armor_pos::armor_pos_top_1;

			if (config.esp_armor_pos == armor_pos::armor_pos_bottom_2)
				config.esp_armor_pos = armor_pos::armor_pos_bottom_1;
		}

	};

	static auto name = [](ImVec2 winpos, ImGuiWindow* window) {
		ImVec2 pos_name;
		const auto get_name = "Name";
		static float font_size = 13.5f;
		const auto get_right = (winpos.x + 215) - (winpos.x + 60);
		const auto name_size = roboto_medium->CalcTextSizeA(font_size, FLT_MAX, 0.0f, get_name);

		ImGuiID id = window->GetID(get_name);
		ImGuiContext& g = *GImGui;

		if (config.esp_name_pos == name_pos::name_pos_top_1)
			pos_name = ImVec2((winpos.x + 60 + get_right * 0.5f) - name_size.x * 0.5f, winpos.y + 40 - name_size.y - 2);
		else if (config.esp_name_pos == name_pos::name_pos_top_2)
			pos_name = ImVec2((winpos.x + 60 + get_right * 0.5f) - name_size.x * 0.5f, winpos.y + 40 - name_size.y - 10);
		else if (config.esp_name_pos == name_pos::name_pos_top_3)
			pos_name = ImVec2((winpos.x + 60 + get_right * 0.5f) - name_size.x * 0.5f, winpos.y + 40 - name_size.y - 18);
		else if (config.esp_name_pos == name_pos::name_pos_left_1)
			pos_name = ImVec2(winpos.x + 56 - name_size.x, winpos.y + 40);
		else if (config.esp_name_pos == name_pos::name_pos_left_2)
			pos_name = ImVec2(winpos.x + 56 - name_size.x - 8, winpos.y + 40);
		else if (config.esp_name_pos == name_pos::name_pos_left_3)
			pos_name = ImVec2(winpos.x + 56 - name_size.x - 16, winpos.y + 40);
		else if (config.esp_name_pos == name_pos::name_pos_right_1)
			pos_name = ImVec2(winpos.x + 219, winpos.y + 40);
		else if (config.esp_name_pos == name_pos::name_pos_right_2)
			pos_name = ImVec2(winpos.x + 227, winpos.y + 40);
		else if (config.esp_name_pos == name_pos::name_pos_right_3)
			pos_name = ImVec2(winpos.x + 235, winpos.y + 40);

		const ImRect bb(ImVec2(pos_name.x, pos_name.y), ImVec2(pos_name.x + name_size.x, pos_name.y + name_size.y));
		bool hovered;
		bool held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

		auto pos = g.IO.MousePos;

		window->DrawList->AddText(roboto_medium, font_size, ImVec2((g.ActiveId == id) ? pos.x - 1 : pos_name.x - 1, (g.ActiveId == id) ? pos.y : pos_name.y), get_color(Color::Black()), get_name);
		window->DrawList->AddText(roboto_medium, font_size, ImVec2((g.ActiveId == id) ? pos.x : pos_name.x, (g.ActiveId == id) ? pos.y - 1 : pos_name.y - 1), get_color(Color::Black()), get_name);
		window->DrawList->AddText(roboto_medium, font_size, ImVec2((g.ActiveId == id) ? pos.x + 1 : pos_name.x + 1, (g.ActiveId == id) ? pos.y : pos_name.y), get_color(Color::Black()), get_name);
		window->DrawList->AddText(roboto_medium, font_size, ImVec2((g.ActiveId == id) ? pos.x : pos_name.x, (g.ActiveId == id) ? pos.y + 1 : pos_name.y + 1), get_color(Color::Black()), get_name);

		window->DrawList->AddText(roboto_medium, font_size, ImVec2((g.ActiveId == id) ? pos.x - 1 : pos_name.x - 1, (g.ActiveId == id) ? pos.y - 1 : pos_name.y - 1), get_color(Color::Black()), get_name);
		window->DrawList->AddText(roboto_medium, font_size, ImVec2((g.ActiveId == id) ? pos.x + 1 : pos_name.x + 1, (g.ActiveId == id) ? pos.y + 1 : pos_name.y + 1), get_color(Color::Black()), get_name);
		window->DrawList->AddText(roboto_medium, font_size, ImVec2((g.ActiveId == id) ? pos.x - 1 : pos_name.x - 1, (g.ActiveId == id) ? pos.y + 1 : pos_name.y + 1), get_color(Color::Black()), get_name);
		window->DrawList->AddText(roboto_medium, font_size, ImVec2((g.ActiveId == id) ? pos.x + 1 : pos_name.x + 1, (g.ActiveId == id) ? pos.y - 1 : pos_name.y - 1), get_color(Color::Black()), get_name);

		window->DrawList->AddText(roboto_medium, font_size, (g.ActiveId == id) ? pos : pos_name, get_color(Color::White()), get_name);

		if (g.ActiveId == id) {

			float cur_x = pos.x;
			float cur_y = pos.y;

			if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= winpos.x && cur_x <= (winpos.x + 60))
				config.esp_name_pos = name_pos::name_pos_left_1;
			if (cur_y >= winpos.y && cur_y <= winpos.y + 40 && cur_x >= (winpos.x + 60) && cur_x <= (winpos.x + 215))
				config.esp_name_pos = name_pos::name_pos_top_1;
			if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= winpos.x + 215 && cur_x <= (winpos.x + window->Size.x))
				config.esp_name_pos = name_pos::name_pos_right_1;
		}

		if (config.esp_health && config.esp_armor) {
			if ((config.esp_name_pos == name_pos::name_pos_left_1 || config.esp_name_pos == name_pos::name_pos_left_2 || config.esp_name_pos == name_pos::name_pos_left_3) && (config.esp_health_pos != health_pos::health_pos_left_1 && config.esp_health_pos != health_pos::health_pos_left_2 && config.esp_armor_pos != armor_pos::armor_pos_left_1 && config.esp_armor_pos != armor_pos::armor_pos_left_2))
				config.esp_name_pos = name_pos::name_pos_left_1;
			if ((config.esp_name_pos == name_pos::name_pos_left_1 || config.esp_name_pos == name_pos::name_pos_left_3) && (config.esp_health_pos == health_pos::health_pos_left_1 && config.esp_health_pos != health_pos::health_pos_left_2 && config.esp_armor_pos != armor_pos::armor_pos_left_1 && config.esp_armor_pos != armor_pos::armor_pos_left_2))
				config.esp_name_pos = name_pos::name_pos_left_2;
			if ((config.esp_name_pos == name_pos::name_pos_left_1 || config.esp_name_pos == name_pos::name_pos_left_3) && (config.esp_armor_pos == armor_pos::armor_pos_left_1 && config.esp_armor_pos != armor_pos::armor_pos_left_2 && config.esp_health_pos != health_pos::health_pos_left_1 && config.esp_health_pos != health_pos::health_pos_left_2))
				config.esp_name_pos = name_pos::name_pos_left_2;
			if ((config.esp_name_pos == name_pos::name_pos_left_1 || config.esp_name_pos == name_pos::name_pos_left_2 || config.esp_name_pos == name_pos::name_pos_left_3) && (config.esp_health_pos == health_pos::health_pos_left_2 && config.esp_armor_pos == armor_pos::armor_pos_left_1 || config.esp_health_pos == health_pos::health_pos_left_1 && config.esp_armor_pos == armor_pos::armor_pos_left_2))
				config.esp_name_pos = name_pos::name_pos_left_3;

			if ((config.esp_name_pos == name_pos::name_pos_top_1 || config.esp_name_pos == name_pos::name_pos_top_2 || config.esp_name_pos == name_pos::name_pos_top_3) && (config.esp_health_pos != health_pos::health_pos_top_1 && config.esp_health_pos != health_pos::health_pos_top_2 && config.esp_armor_pos != armor_pos::armor_pos_top_1 && config.esp_armor_pos != armor_pos::armor_pos_top_2))
				config.esp_name_pos = name_pos::name_pos_top_1;
			if ((config.esp_name_pos == name_pos::name_pos_top_1 || config.esp_name_pos == name_pos::name_pos_top_3) && (config.esp_health_pos == health_pos::health_pos_top_1 && config.esp_health_pos != health_pos::health_pos_top_2 && config.esp_armor_pos != armor_pos::armor_pos_top_1 && config.esp_armor_pos != armor_pos::armor_pos_top_2))
				config.esp_name_pos = name_pos::name_pos_top_2;
			if ((config.esp_name_pos == name_pos::name_pos_top_1 || config.esp_name_pos == name_pos::name_pos_top_3) && (config.esp_armor_pos == armor_pos::armor_pos_top_1 && config.esp_armor_pos != armor_pos::armor_pos_top_2 && config.esp_health_pos != health_pos::health_pos_top_1 && config.esp_health_pos != health_pos::health_pos_top_2))
				config.esp_name_pos = name_pos::name_pos_top_2;
			if ((config.esp_name_pos == name_pos::name_pos_top_1 || config.esp_name_pos == name_pos::name_pos_top_2 || config.esp_name_pos == name_pos::name_pos_top_3) && (config.esp_health_pos == health_pos::health_pos_top_2 && config.esp_armor_pos == armor_pos::armor_pos_top_1 || config.esp_health_pos == health_pos::health_pos_top_1 && config.esp_armor_pos == armor_pos::armor_pos_top_2))
				config.esp_name_pos = name_pos::name_pos_top_3;

			if ((config.esp_name_pos == name_pos::name_pos_right_1 || config.esp_name_pos == name_pos::name_pos_right_2 || config.esp_name_pos == name_pos::name_pos_right_3) && (config.esp_health_pos != health_pos::health_pos_right_1 && config.esp_health_pos != health_pos::health_pos_right_2 && config.esp_armor_pos != armor_pos::armor_pos_right_1 && config.esp_armor_pos != armor_pos::armor_pos_right_2))
				config.esp_name_pos = name_pos::name_pos_right_1;
			if ((config.esp_name_pos == name_pos::name_pos_right_1 || config.esp_name_pos == name_pos::name_pos_right_3) && (config.esp_health_pos == health_pos::health_pos_right_1 && config.esp_health_pos != health_pos::health_pos_right_2 && config.esp_armor_pos != armor_pos::armor_pos_right_1 && config.esp_armor_pos != armor_pos::armor_pos_right_2))
				config.esp_name_pos = name_pos::name_pos_right_2;
			if ((config.esp_name_pos == name_pos::name_pos_right_1 || config.esp_name_pos == name_pos::name_pos_right_3) && (config.esp_armor_pos == armor_pos::armor_pos_right_1 && config.esp_armor_pos != armor_pos::armor_pos_right_2 && config.esp_health_pos != health_pos::health_pos_right_1 && config.esp_health_pos != health_pos::health_pos_right_2))
				config.esp_name_pos = name_pos::name_pos_right_2;
			if ((config.esp_name_pos == name_pos::name_pos_right_1 || config.esp_name_pos == name_pos::name_pos_right_2 || config.esp_name_pos == name_pos::name_pos_right_3) && (config.esp_health_pos == health_pos::health_pos_right_2 && config.esp_armor_pos == armor_pos::armor_pos_right_1 || config.esp_health_pos == health_pos::health_pos_right_1 && config.esp_armor_pos == armor_pos::armor_pos_right_2))
				config.esp_name_pos = name_pos::name_pos_right_3;
		}
		else if (config.esp_health && !config.esp_armor) {
			if ((config.esp_name_pos == name_pos::name_pos_left_1 || config.esp_name_pos == name_pos::name_pos_left_2 || config.esp_name_pos == name_pos::name_pos_left_3) && config.esp_health_pos != health_pos::health_pos_left_1)
				config.esp_name_pos = name_pos::name_pos_left_1;
			if ((config.esp_name_pos == name_pos::name_pos_left_1 || config.esp_name_pos == name_pos::name_pos_left_3) && config.esp_health_pos == health_pos::health_pos_left_1)
				config.esp_name_pos = name_pos::name_pos_left_2;

			if ((config.esp_name_pos == name_pos::name_pos_top_1 || config.esp_name_pos == name_pos::name_pos_top_2 || config.esp_name_pos == name_pos::name_pos_top_3) && config.esp_health_pos != health_pos::health_pos_top_1)
				config.esp_name_pos = name_pos::name_pos_top_1;
			if ((config.esp_name_pos == name_pos::name_pos_top_1 || config.esp_name_pos == name_pos::name_pos_top_3) && config.esp_health_pos == health_pos::health_pos_top_1)
				config.esp_name_pos = name_pos::name_pos_top_2;

			if ((config.esp_name_pos == name_pos::name_pos_right_1 || config.esp_name_pos == name_pos::name_pos_right_2 || config.esp_name_pos == name_pos::name_pos_right_3) && config.esp_health_pos != health_pos::health_pos_right_1)
				config.esp_name_pos = name_pos::name_pos_right_1;
			if ((config.esp_name_pos == name_pos::name_pos_right_1 || config.esp_name_pos == name_pos::name_pos_right_3) && config.esp_health_pos == health_pos::health_pos_right_1)
				config.esp_name_pos = name_pos::name_pos_right_2;
		}
		else if (!config.esp_health && config.esp_armor) {
			if ((config.esp_name_pos == name_pos::name_pos_left_1 || config.esp_name_pos == name_pos::name_pos_left_2 || config.esp_name_pos == name_pos::name_pos_left_3) && config.esp_armor_pos != armor_pos::armor_pos_left_1)
				config.esp_name_pos = name_pos::name_pos_left_1;
			if ((config.esp_name_pos == name_pos::name_pos_left_1 || config.esp_name_pos == name_pos::name_pos_left_3) && config.esp_armor_pos == armor_pos::armor_pos_left_1)
				config.esp_name_pos = name_pos::name_pos_left_2;

			if ((config.esp_name_pos == name_pos::name_pos_top_1 || config.esp_name_pos == name_pos::name_pos_top_2 || config.esp_name_pos == name_pos::name_pos_top_3) && config.esp_armor_pos != armor_pos::armor_pos_top_1)
				config.esp_name_pos = name_pos::name_pos_top_1;
			if ((config.esp_name_pos == name_pos::name_pos_top_1 || config.esp_name_pos == name_pos::name_pos_top_3) && config.esp_armor_pos == armor_pos::armor_pos_top_1)
				config.esp_name_pos = name_pos::name_pos_top_2;

			if ((config.esp_name_pos == name_pos::name_pos_right_1 || config.esp_name_pos == name_pos::name_pos_right_2 || config.esp_name_pos == name_pos::name_pos_right_3) && config.esp_armor_pos != armor_pos::armor_pos_right_1)
				config.esp_name_pos = name_pos::name_pos_right_1;
			if ((config.esp_name_pos == name_pos::name_pos_right_1 || config.esp_name_pos == name_pos::name_pos_right_3) && config.esp_armor_pos == armor_pos::armor_pos_right_1)
				config.esp_name_pos = name_pos::name_pos_right_2;
		}
		else {
			if (config.esp_name_pos == name_pos::name_pos_left_1 || config.esp_name_pos == name_pos::name_pos_left_2 || config.esp_name_pos == name_pos::name_pos_left_3)
				config.esp_name_pos = name_pos::name_pos_left_1;

			if (config.esp_name_pos == name_pos::name_pos_top_1 || config.esp_name_pos == name_pos::name_pos_top_2 || config.esp_name_pos == name_pos::name_pos_top_3)
				config.esp_name_pos = name_pos::name_pos_top_1;

			if (config.esp_name_pos == name_pos::name_pos_right_1 || config.esp_name_pos == name_pos::name_pos_right_2 || config.esp_name_pos == name_pos::name_pos_right_3)
				config.esp_name_pos = name_pos::name_pos_right_1;
		}
	};

	static auto weapon = [](ImVec2 winpos, ImGuiWindow* window) {
		ImVec2 pos_weapon;
		const auto get_weapon = config.esp_weapon_icon ? "e" : "AK-47";
		static float font_size = 13.5f;
		const auto get_right = (winpos.x + 215) - (winpos.x + 60);
		const auto font = config.esp_weapon_icon ? weapon_icons : roboto_medium;
		const auto weapon_size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, get_weapon);

		ImGuiID id = window->GetID(get_weapon);
		ImGuiContext& g = *GImGui;

		if (config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_1)
			pos_weapon = ImVec2((winpos.x + 60 + get_right * 0.5f) - weapon_size.x * 0.5f, winpos.y + 285 + 2);
		else if (config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_2)
			pos_weapon = ImVec2((winpos.x + 60 + get_right * 0.5f) - weapon_size.x * 0.5f, winpos.y + 285 + 10);
		else if (config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_3)
			pos_weapon = ImVec2((winpos.x + 60 + get_right * 0.5f) - weapon_size.x * 0.5f, winpos.y + 285 + 18);
		else if (config.esp_weapon_pos == weapon_pos::weapon_pos_left_1)
			pos_weapon = ImVec2(winpos.x + 56 - weapon_size.x, winpos.y + 285 - weapon_size.y);
		else if (config.esp_weapon_pos == weapon_pos::weapon_pos_left_2)
			pos_weapon = ImVec2(winpos.x + 56 - weapon_size.x - 8, winpos.y + 285 - weapon_size.y);
		else if (config.esp_weapon_pos == weapon_pos::weapon_pos_left_3)
			pos_weapon = ImVec2(winpos.x + 56 - weapon_size.x - 16, winpos.y + 285 - weapon_size.y);
		else if (config.esp_weapon_pos == weapon_pos::weapon_pos_right_1)
			pos_weapon = ImVec2(winpos.x + 219, winpos.y + 285 - weapon_size.y);
		else if (config.esp_weapon_pos == weapon_pos::weapon_pos_right_2)
			pos_weapon = ImVec2(winpos.x + 227, winpos.y + 285 - weapon_size.y);
		else if (config.esp_weapon_pos == weapon_pos::weapon_pos_right_3)
			pos_weapon = ImVec2(winpos.x + 235, winpos.y + 285 - weapon_size.y);

		const ImRect bb(ImVec2(pos_weapon.x, pos_weapon.y), ImVec2(pos_weapon.x + weapon_size.x, pos_weapon.y + weapon_size.y));
		bool hovered;
		bool held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

		auto pos = g.IO.MousePos;

		window->DrawList->AddText(font, font_size, ImVec2((g.ActiveId == id) ? pos.x - 1 : pos_weapon.x - 1, (g.ActiveId == id) ? pos.y : pos_weapon.y), get_color(Color::Black()), get_weapon);
		window->DrawList->AddText(font, font_size, ImVec2((g.ActiveId == id) ? pos.x : pos_weapon.x, (g.ActiveId == id) ? pos.y - 1 : pos_weapon.y - 1), get_color(Color::Black()), get_weapon);
		window->DrawList->AddText(font, font_size, ImVec2((g.ActiveId == id) ? pos.x + 1 : pos_weapon.x + 1, (g.ActiveId == id) ? pos.y : pos_weapon.y), get_color(Color::Black()), get_weapon);
		window->DrawList->AddText(font, font_size, ImVec2((g.ActiveId == id) ? pos.x : pos_weapon.x, (g.ActiveId == id) ? pos.y + 1 : pos_weapon.y + 1), get_color(Color::Black()), get_weapon);

		window->DrawList->AddText(font, font_size, ImVec2((g.ActiveId == id) ? pos.x - 1 : pos_weapon.x - 1, (g.ActiveId == id) ? pos.y - 1 : pos_weapon.y - 1), get_color(Color::Black()), get_weapon);
		window->DrawList->AddText(font, font_size, ImVec2((g.ActiveId == id) ? pos.x + 1 : pos_weapon.x + 1, (g.ActiveId == id) ? pos.y + 1 : pos_weapon.y + 1), get_color(Color::Black()), get_weapon);
		window->DrawList->AddText(font, font_size, ImVec2((g.ActiveId == id) ? pos.x - 1 : pos_weapon.x - 1, (g.ActiveId == id) ? pos.y + 1 : pos_weapon.y + 1), get_color(Color::Black()), get_weapon);
		window->DrawList->AddText(font, font_size, ImVec2((g.ActiveId == id) ? pos.x + 1 : pos_weapon.x + 1, (g.ActiveId == id) ? pos.y - 1 : pos_weapon.y - 1), get_color(Color::Black()), get_weapon);

		window->DrawList->AddText(font, font_size, (g.ActiveId == id) ? pos : pos_weapon, get_color(Color::White()), get_weapon);

		if (g.ActiveId == id) {
			float cur_x = pos.x;
			float cur_y = pos.y;

			if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= winpos.x && cur_x <= (winpos.x + 60))
				config.esp_weapon_pos = weapon_pos::weapon_pos_left_1;
			if (cur_y >= winpos.y + 289 && cur_y <= winpos.y + window->Size.y && cur_x >= (winpos.x + 60) && cur_x <= (winpos.x + 215))
				config.esp_weapon_pos = weapon_pos::weapon_pos_bottom_1;
			if (cur_y >= winpos.y + 40 && cur_y <= winpos.y + 285 && cur_x >= winpos.x + 215 && cur_x <= (winpos.x + window->Size.x))
				config.esp_weapon_pos = weapon_pos::weapon_pos_right_1;
		}

		if (config.esp_health && config.esp_armor) {
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_left_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_left_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_left_3) && (config.esp_health_pos != health_pos::health_pos_left_1 && config.esp_health_pos != health_pos::health_pos_left_2 && config.esp_armor_pos != armor_pos::armor_pos_left_1 && config.esp_armor_pos != armor_pos::armor_pos_left_2))
				config.esp_weapon_pos = weapon_pos::weapon_pos_left_1;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_left_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_left_3) && (config.esp_health_pos == health_pos::health_pos_left_1 && config.esp_health_pos != health_pos::health_pos_left_2 && config.esp_armor_pos != armor_pos::armor_pos_left_1 && config.esp_armor_pos != armor_pos::armor_pos_left_2))
				config.esp_weapon_pos = weapon_pos::weapon_pos_left_2;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_left_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_left_3) && (config.esp_armor_pos == armor_pos::armor_pos_left_1 && config.esp_armor_pos != armor_pos::armor_pos_left_2 && config.esp_health_pos != health_pos::health_pos_left_1 && config.esp_health_pos != health_pos::health_pos_left_2))
				config.esp_weapon_pos = weapon_pos::weapon_pos_left_2;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_left_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_left_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_left_3) && (config.esp_health_pos == health_pos::health_pos_left_2 && config.esp_armor_pos == armor_pos::armor_pos_left_1 || config.esp_health_pos == health_pos::health_pos_left_1 && config.esp_armor_pos == armor_pos::armor_pos_left_2))
				config.esp_weapon_pos = weapon_pos::weapon_pos_left_3;

			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_3) && (config.esp_health_pos != health_pos::health_pos_bottom_1 && config.esp_health_pos != health_pos::health_pos_bottom_2 && config.esp_armor_pos != armor_pos::armor_pos_bottom_1 && config.esp_armor_pos != armor_pos::armor_pos_bottom_2))
				config.esp_weapon_pos = weapon_pos::weapon_pos_bottom_1;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_3) && (config.esp_health_pos == health_pos::health_pos_bottom_1 && config.esp_health_pos != health_pos::health_pos_bottom_2 && config.esp_armor_pos != armor_pos::armor_pos_bottom_1 && config.esp_armor_pos != armor_pos::armor_pos_bottom_2))
				config.esp_weapon_pos = weapon_pos::weapon_pos_bottom_2;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_3) && (config.esp_armor_pos == armor_pos::armor_pos_bottom_1 && config.esp_armor_pos != armor_pos::armor_pos_bottom_2 && config.esp_health_pos != health_pos::health_pos_bottom_1 && config.esp_health_pos != health_pos::health_pos_bottom_2))
				config.esp_weapon_pos = weapon_pos::weapon_pos_bottom_2;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_3) && (config.esp_health_pos == health_pos::health_pos_bottom_2 && config.esp_armor_pos == armor_pos::armor_pos_bottom_1 || config.esp_health_pos == health_pos::health_pos_bottom_1 && config.esp_armor_pos == armor_pos::armor_pos_bottom_2))
				config.esp_weapon_pos = weapon_pos::weapon_pos_bottom_3;

			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_right_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_right_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_right_3) && (config.esp_health_pos != health_pos::health_pos_right_1 && config.esp_health_pos != health_pos::health_pos_right_2 && config.esp_armor_pos != armor_pos::armor_pos_right_1 && config.esp_armor_pos != armor_pos::armor_pos_right_2))
				config.esp_weapon_pos = weapon_pos::weapon_pos_right_1;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_right_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_right_3) && (config.esp_health_pos == health_pos::health_pos_right_1 && config.esp_health_pos != health_pos::health_pos_right_2 && config.esp_armor_pos != armor_pos::armor_pos_right_1 && config.esp_armor_pos != armor_pos::armor_pos_right_2))
				config.esp_weapon_pos = weapon_pos::weapon_pos_right_2;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_right_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_right_3) && (config.esp_armor_pos == armor_pos::armor_pos_right_1 && config.esp_armor_pos != armor_pos::armor_pos_right_2 && config.esp_health_pos != health_pos::health_pos_right_1 && config.esp_health_pos != health_pos::health_pos_right_2))
				config.esp_weapon_pos = weapon_pos::weapon_pos_right_2;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_right_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_right_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_right_3) && (config.esp_health_pos == health_pos::health_pos_right_2 && config.esp_armor_pos == armor_pos::armor_pos_right_1 || config.esp_health_pos == health_pos::health_pos_right_1 && config.esp_armor_pos == armor_pos::armor_pos_right_2))
				config.esp_weapon_pos = weapon_pos::weapon_pos_right_3;
		}
		else if (config.esp_health && !config.esp_armor) {
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_left_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_left_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_left_3) && config.esp_health_pos != health_pos::health_pos_left_1)
				config.esp_weapon_pos = weapon_pos::weapon_pos_left_1;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_left_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_left_3) && config.esp_health_pos == health_pos::health_pos_left_1)
				config.esp_weapon_pos = weapon_pos::weapon_pos_left_2;

			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_3) && config.esp_health_pos != health_pos::health_pos_bottom_1)
				config.esp_weapon_pos = weapon_pos::weapon_pos_bottom_1;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_3) && config.esp_health_pos == health_pos::health_pos_bottom_1)
				config.esp_weapon_pos = weapon_pos::weapon_pos_bottom_2;

			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_right_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_right_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_right_3) && config.esp_health_pos != health_pos::health_pos_right_1)
				config.esp_weapon_pos = weapon_pos::weapon_pos_right_1;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_right_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_right_3) && config.esp_health_pos == health_pos::health_pos_right_1)
				config.esp_weapon_pos = weapon_pos::weapon_pos_right_2;
		}
		else if (!config.esp_health && config.esp_armor) {
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_left_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_left_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_left_3) && config.esp_armor_pos != armor_pos::armor_pos_left_1)
				config.esp_weapon_pos = weapon_pos::weapon_pos_left_1;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_left_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_left_3) && config.esp_armor_pos == armor_pos::armor_pos_left_1)
				config.esp_weapon_pos = weapon_pos::weapon_pos_left_2;

			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_3) && config.esp_armor_pos != armor_pos::armor_pos_bottom_1)
				config.esp_weapon_pos = weapon_pos::weapon_pos_bottom_1;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_3) && config.esp_armor_pos == armor_pos::armor_pos_bottom_1)
				config.esp_weapon_pos = weapon_pos::weapon_pos_bottom_2;

			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_right_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_right_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_right_3) && config.esp_armor_pos != armor_pos::armor_pos_right_1)
				config.esp_weapon_pos = weapon_pos::weapon_pos_right_1;
			if ((config.esp_weapon_pos == weapon_pos::weapon_pos_right_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_right_3) && config.esp_armor_pos == armor_pos::armor_pos_right_1)
				config.esp_weapon_pos = weapon_pos::weapon_pos_right_2;
		}
		else {
			if (config.esp_weapon_pos == weapon_pos::weapon_pos_left_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_left_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_left_3)
				config.esp_weapon_pos = weapon_pos::weapon_pos_left_1;

			if (config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_3)
				config.esp_weapon_pos = weapon_pos::weapon_pos_bottom_1;

			if (config.esp_weapon_pos == weapon_pos::weapon_pos_right_1 || config.esp_weapon_pos == weapon_pos::weapon_pos_right_2 || config.esp_weapon_pos == weapon_pos::weapon_pos_right_3)
				config.esp_weapon_pos = weapon_pos::weapon_pos_right_1;
		}
	};

	box(winpos, window);
	if (config.esp_health)
		health(winpos, window);
	if (config.esp_armor)
		armor(winpos, window);
	if (config.esp_name)
		name(winpos, window);
	if (config.esp_weapon)
		weapon(winpos, window);
}

void gui::render() {

	if (!menu_open)
		return;

	static int menu_tabs = 0;

	static int cur_group = 0;
	static int visuals_page = 0;
	static int misc_page = 0;
	static int skins_page = 0;

	static int inventory_paint_kit = 0;
	static int inventory_definition_index = 1;
	static int inventory_seed = 0;
	static int inventory_sticker_slot = 0;
	static int inventory_stattrak = 0;
	static int inventory_other = 0;

	static float inventory_wear = 0.f;
	static std::array< inventory_item::sticker_kit, 5> inventory_stickers;


	static int default_paint_kit = 0;
	static int default_sticker[4] = { 0, 0, 0 ,0 };
	static int default_definition_index = 1;

	static ImVec4 color_rarity;

	static uint32_t type_pistol = utils::get_const(XORSTR("#CSGO_Type_Pistol"));
	static uint32_t type_smg = utils::get_const(XORSTR("#CSGO_Type_SMG"));
	static uint32_t type_rifle = utils::get_const(XORSTR("#CSGO_Type_Rifle"));
	static uint32_t type_sniper = utils::get_const(XORSTR("#CSGO_Type_SniperRifle"));
	static uint32_t type_shotgun = utils::get_const(XORSTR("#CSGO_Type_Shotgun"));
	static uint32_t type_machinegun = utils::get_const(XORSTR("#CSGO_Type_Machinegun"));
	static uint32_t type_knife = utils::get_const(XORSTR("#CSGO_Type_Knife"));
	static uint32_t type_gloves = utils::get_const(XORSTR("#Type_Hands"));
	static uint32_t type_collectible = utils::get_const(XORSTR("#CSGO_Type_Collectible"));
	static uint32_t type_custom_player = utils::get_const(XORSTR("#Type_CustomPlayer"));
	static uint32_t type_music_kit = utils::get_const(XORSTR("#CSGO_Type_MusicKit"));
	static uint32_t type_case = utils::get_const(XORSTR("#CSGO_Type_WeaponCase"));
	static uint32_t type_key = utils::get_const(XORSTR("#CSGO_Tool_WeaponCase_KeyTag"));

	static ImGuiTextFilter filter_sticker;

	static float size_window_x = 930.f;
	static float size_window_y = 500.f;

	static float size_mini_child_x = 270.f;
	static float size_child_x = 410.f;
	static float size_big_child_x = 550.f;

	static float size_max_child_x = 910.f;
	static float size_max_child_y = 426.f;

	static float size_spacing = 10.f;

	static float item_width_mini_child = 250.f;
	static float item_width_child_x = 390.f;
	static float item_big_child_x = 530.f;

	static float item_button_size_y = 40.f;

	const auto color_edit_flags = ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoDragDrop;

	ImGui::SetNextWindowSize({ size_window_x, size_window_y });
	if (ImGui::Begin("Menu", &menu_open, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse)) {
		ImVec2 window_pos{ ImGui::GetWindowPos() };
		ImDrawList* window_draw{ ImGui::GetWindowDrawList() };
		ImGuiContext& g{ *GImGui };

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, { 0.f, 0.f }); {		
			if (ImGui::ButtonTabs("0", { 85.f, 50.f }, menu_tabs == 0)) {
				menu_tabs = 0;
				cur_group = 0;
				ImGui::ResetAnimations();
			}
			ImGui::SameLine();
			if (ImGui::ButtonTabs("1", { 85.f, 50.f }, menu_tabs == 1)) {
				menu_tabs = 1; 
				ImGui::ResetAnimations();
			}
			ImGui::SameLine();
			if (ImGui::ButtonTabs("2", { 85.f, 50.f }, menu_tabs == 0)) {
				menu_tabs = 2; 
				ImGui::ResetAnimations();
			}
			ImGui::SameLine();
			if (ImGui::ButtonTabs("3", { 85.f, 50.f }, menu_tabs == 1)) {
				menu_tabs = 3; 
				ImGui::ResetAnimations();
			}
			ImGui::SameLine();
			if (ImGui::ButtonTabs("4", { 85.f, 50.f }, menu_tabs == 2)) {
				menu_tabs = 4; 
				visuals_page = 0;
				ImGui::ResetAnimations();
			}
			ImGui::SameLine();
			if (ImGui::ButtonTabs("5", { 85.f, 50.f }, menu_tabs == 3)) {
				menu_tabs = 5; 
				ImGui::ResetAnimations();
			}
			ImGui::SameLine();
			if (ImGui::ButtonTabs("6", { 85.f, 50.f }, menu_tabs == 4)) {
				menu_tabs = 6; 
				skins_page = 0;
				inventory_sticker_slot = 0;
				ImGui::ResetAnimations();
			}
			ImGui::SameLine();
			if (ImGui::ButtonTabs("7", { 85.f, 50.f }, menu_tabs == 5)) {
				menu_tabs = 7; 
				ImGui::ResetAnimations();
			}
			ImGui::SameLine();
			if (ImGui::ButtonTabs("8", { 85.f, 50.f }, menu_tabs == 6)) {
				menu_tabs = 8; misc_page = 0; 
				ImGui::ResetAnimations();
			}
			ImGui::SameLine();
			if (ImGui::ButtonTabs("9", { 85.f, 50.f }, menu_tabs == 7)) {
				menu_tabs = 9; 
				ImGui::ResetAnimations();
			}
			ImGui::SameLine();
			if (ImGui::ButtonTabs("10", { 85.f, 50.f }, menu_tabs == 8)) {
				menu_tabs = 10;
				ImGui::ResetAnimations();
			}
		}
		ImGui::PopStyleVar();

		window_draw->AddImage(ragebot_image, { window_pos.x + 32.5f, window_pos.y }, { window_pos.x + 52.5f, window_pos.y + 30.f });
		window_draw->AddImage(antiaim_image, { window_pos.x + 117.5f, window_pos.y }, { window_pos.x + 137.5f, window_pos.y + 30.f });
		window_draw->AddImage(legitbot_image, { window_pos.x + 202.5f, window_pos.y }, { window_pos.x + 222.5f, window_pos.y + 30.f });
		window_draw->AddImage(player_esp_image, { window_pos.x + 287.5f, window_pos.y }, { window_pos.x + 307.5f, window_pos.y + 30.f });
		window_draw->AddImage(world_esp_image, { window_pos.x + 372.5f, window_pos.y }, { window_pos.x + 392.5f, window_pos.y + 30.f });
		window_draw->AddImage(glow_and_chams_image, { window_pos.x + 457.5f, window_pos.y }, { window_pos.x + 477.5f, window_pos.y + 30.f });
		window_draw->AddImage(inventory_image, { window_pos.x + 542.5f, window_pos.y }, { window_pos.x + 562.5f, window_pos.y + 30.f });
		window_draw->AddImage(profile_image, { window_pos.x + 627.5f, window_pos.y }, { window_pos.x + 647.5f, window_pos.y + 30.f });
		window_draw->AddImage(misc_image, { window_pos.x + 712.5f, window_pos.y }, { window_pos.x + 732.5f, window_pos.y + 30.f });
		window_draw->AddImage(settings_image, { window_pos.x + 797.5f, window_pos.y }, { window_pos.x + 817.5f, window_pos.y + 30.f });
		window_draw->AddImage(games_image, { window_pos.x + 882.5f, window_pos.y }, { window_pos.x + 902.5f, window_pos.y + 30.f });

		const char* char_tabs[] = { get_language("RAGEBOT", "РЕЙДЖБОТ"), get_language("ANTI-AIM", "АНТИ-АИМ"), get_language("LEGITBOT", "ЛЕГИТБОТ"), get_language("PLAYER ESP", "ЕСП НА ИГРОКА"),  get_language("OTHER VISUALS", "ДРУГИЕ ВИЗУАЛЫ"), get_language("GLOW AND CHAMS", "СВЕЧЕНИЕ И ЧАМСЫ"), get_language("INVENTORY", "ИНВЕНТАРЬ"), get_language("PROFILE", "ПРОФИЛЬ"), get_language("MISC", "РАЗНОЕ"), get_language("SETTINGS", "НАСТРОЙКИ"), get_language("MINI-GAMES", "МИНИ-ИГРЫ") };

		ImGui::PushFont(roboto_bold_tabs);
		for (size_t i = 0; i < IM_ARRAYSIZE(char_tabs); i++) {
			const auto size = ImGui::CalcTextSize(char_tabs[i]);
			window_draw->AddText(ImVec2(window_pos.x + (85.f * i) + (((window_pos.x + 85.f) - window_pos.x) / 2.f) - (size.x / 2.f), window_pos.y + 33.f), ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)), char_tabs[i]);
		}
		ImGui::PopFont();

		ImGui::SameLine(10.f);
		ImGui::BeginGroup(); {
			ImGui::Dummy({ 0.f, 54.f });
			ImGui::PushFont(roboto_medium_menu);
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, { 10.f, 8.f }); {
				switch (menu_tabs) {
				case 0: {
					ImGui::BeginGroup(); {
						ImGui::BeginChild("General", { size_mini_child_x, 244.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar); {
							ImGui::Checkbox("Enable", &config.m_aRageSettings[cur_group].m_bEnabled);
							ImGui::Checkbox("Automatic stop", &config.m_aRageSettings[cur_group].m_bAutoStop);
							std::vector < const char* > aAutoStop =
							{
								"Force accuracy",
								"Early"
							};

							ImGui::PushItemWidth(item_width_mini_child);
							if (ImGui::BeginCombo("Autostop Options", "Select"))
							{
								ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
									for (int i = 0; i < aAutoStop.size(); i++)
										ImGui::CheckboxButton(aAutoStop[i], ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y), &config.m_aRageSettings[cur_group].m_AutoStopOptions[i]);
								}
								ImGui::PopStyleVar();

								ImGui::EndCombo();
							}
							ImGui::PopItemWidth();

							ImGui::Checkbox("Automatic Scope", &config.m_aRageSettings[cur_group].m_bAutoScope);

							ImGui::Checkbox("Lethal Safe", &config.m_aRageSettings[cur_group].m_bLethalSafety);
							ImGui::Checkbox("Prefer Body", &config.m_aRageSettings[cur_group].m_bPreferBody);
							ImGui::Checkbox("Prefer Safe", &config.m_aRageSettings[cur_group].m_bPreferSafe);

							//ImGui::Text(_S("Force safe"));
							//ImGui::Keybind(_S("Force safe"), &g_Settings->m_aSafePoint->m_iKeySelected, &g_Settings->m_aSafePoint->m_iModeSelected);

							//ImGui::Text(_S("Damage override"));
							//ImGui::Keybind(_S("Damage override"), &g_Settings->m_aMinDamage->m_iKeySelected, &g_Settings->m_aMinDamage->m_iModeSelected);

							//ImGui::Text(_S("Double tap"));
							//ImGui::Keybind(_S("Double tap"), &g_Settings->m_aDoubleTap->m_iKeySelected, &g_Settings->m_aDoubleTap->m_iModeSelected);

							//ImGui::Text(_S("Hide shots"));
							//ImGui::Keybind(_S("Hide shots"), &g_Settings->m_aHideShots->m_iKeySelected, &g_Settings->m_aHideShots->m_iModeSelected);
						}
						ImGui::EndChild();
						ImGui::BeginChild("Accuracy", { size_mini_child_x, 172.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::SliderInt("MIN DAMAGE", &config.m_aRageSettings[cur_group].m_iMinDamage, 0, 100);
							ImGui::SliderInt("MIN DAMAGE OVERRIDE", &config.m_aRageSettings[cur_group].m_iMinDamageOverride, 0, 100);
							ImGui::SliderInt("HITCHANCE", &config.m_aRageSettings[cur_group].m_iHitChance, 0, 100);
							ImGui::SliderInt("ACCURACY BOOST", &config.m_aRageSettings[cur_group].m_iAccuracyBoost, 0, 100);
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x + size_spacing);
					ImGui::BeginGroup(); {
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, ImGui::ColConvertToFloat(0.f, 0.f, 0.f, 0.f));
						ImGui::BeginChild("Groups", { size_big_child_x, 50.f }, true); {
							ImGuiWindow* window = ImGui::GetCurrentWindow();
							ImVec2 size{ size_big_child_x / 7.f, 50.f };
							ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f)); {
								if (ImGui::ButtonTabs("D", size, cur_group == 0))
									cur_group = 0;
								ImGui::SameLine();
								if (ImGui::ButtonTabs("F", size, cur_group == 1))
									cur_group = 1;
								ImGui::SameLine();
								if (ImGui::ButtonTabs("g", size, cur_group == 2))
									cur_group = 2;
								ImGui::SameLine();
								if (ImGui::ButtonTabs("a", size, cur_group == 3))
									cur_group = 3;
								ImGui::SameLine();
								if (ImGui::ButtonTabs("R", size, cur_group == 4))
									cur_group = 4;
								ImGui::SameLine();
								if (ImGui::ButtonTabs("b", size, cur_group == 5))
									cur_group = 5;
								ImGui::SameLine();
								if (ImGui::ButtonTabs("l", size, cur_group == 6))
									cur_group = 6;
							}
							ImGui::PopStyleVar();

							static float current_sub_selected_pos = 0.f;
							static float new_sub_selected_pos = 0.f;

							if (current_sub_selected_pos < new_sub_selected_pos) {
								current_sub_selected_pos += 2000.f * ImGui::GetIO().DeltaTime;
								if (current_sub_selected_pos >= new_sub_selected_pos)
									current_sub_selected_pos = new_sub_selected_pos;
							}

							else if (current_sub_selected_pos > new_sub_selected_pos) {
								current_sub_selected_pos -= 2000.f * ImGui::GetIO().DeltaTime;
								if (current_sub_selected_pos <= new_sub_selected_pos)
									current_sub_selected_pos = new_sub_selected_pos;
							}

							new_sub_selected_pos = size.x * float(cur_group);

							window->DrawList->PushClipRectFullScreen();
							window->DrawList->AddRectFilled({ window->Pos.x + current_sub_selected_pos,  window->Pos.y }, { window->Pos.x + current_sub_selected_pos + size.x,  window->Pos.y + size.y }, ImGui::GetColorU32(ImGui::ColConvertToFloat(34.f, 52.f, 76.f)));
							const char* icons_tabs[] = { "D", "F", "g", "a", "R", "b", "l" };

							ImGui::PushFont(weapon_icons_menu);
							for (size_t i = 0; i < IM_ARRAYSIZE(icons_tabs); i++) {
								const auto text_size = ImGui::CalcTextSize(icons_tabs[i]);
								window->DrawList->AddText(ImVec2(window->Pos.x + (size.x * i) + (((window->Pos.x + size.x) - window->Pos.x) / 2.f) - (text_size.x / 2.f), window->Pos.y + 18.f), ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)), icons_tabs[i]);
							}
							ImGui::PopFont();
							window->DrawList->PopClipRect();
						}
						ImGui::EndChild();
						ImGui::PopStyleColor();
						ImGui::BeginGroup(); {
							std::vector< const char* > aHitboxes =
							{
								("Head"),
								("Chest"),
								("Arms"),
								("Pelvis"),
								("Stomach"),
								("Legs"),
							};

							ImGui::BeginChild("Target", { size_mini_child_x, 198.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
								ImGui::PushItemWidth(item_width_mini_child);						

								ImGui::Spacing();

								if (ImGui::BeginCombo("Multipoints", "Select"))
								{
									ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
										for (int i = 0; i < aHitboxes.size(); i++)
											ImGui::CheckboxButton(aHitboxes[i], ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y), &config.m_aRageSettings[cur_group].m_Multipoints[i]);
									}
									ImGui::PopStyleVar();

									ImGui::EndCombo();
								}

								ImGui::SliderInt("HEAD SCALE", &config.m_aRageSettings[cur_group].m_iHeadScale, 0, 100);
								ImGui::SliderInt("BODY SCALE", &config.m_aRageSettings[cur_group].m_iBodyScale, 0, 100);

								if (ImGui::BeginCombo("Hitboxes", "Select"))
								{
									ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
										for (int i = 0; i < aHitboxes.size(); i++)
											ImGui::CheckboxButton(aHitboxes[i], ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y), &config.m_aRageSettings[cur_group].m_Hitboxes[i]);
									}
									ImGui::PopStyleVar();

									ImGui::EndCombo();
								}
								ImGui::PopItemWidth();
							}
							ImGui::EndChild();
							ImGui::BeginChild("Safe Point", { size_mini_child_x, 158.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
								ImGui::PushItemWidth(item_width_mini_child);

								ImGui::Spacing();

								ImGui::Combo("Safe Point Type Bind##safe_point_key_mode", &config.safe_point_key_mode, "Toggle\0\rHold\0\rForce Disable\0\rAlways on\0\0");
								ImGui::Hotkey("Safe Point Key##safe_point_key", &config.safe_point_key);

								if (ImGui::BeginCombo("Safe Hitboxes", "Select"))
								{
									ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
										for (int i = 0; i < aHitboxes.size(); i++)
											ImGui::CheckboxButton(aHitboxes[i], ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y), &config.m_aRageSettings[cur_group].m_SafeHitboxes[i]);
									}
									ImGui::PopStyleVar();

									ImGui::EndCombo();
								}
								ImGui::PopItemWidth();
							}
							ImGui::EndChild();
						}
						ImGui::EndGroup();
						ImGui::SameLine(size_mini_child_x + size_spacing);
						ImGui::BeginGroup(); {
							ImGui::BeginChild("Exploits", { size_mini_child_x, 366.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar); {
								ImGui::PushItemWidth(item_width_mini_child);
								ImGui::Spacing();
								ImGui::Combo("Damage Override Type Bind##m_bOverrideDamage_key_mode", &config.m_bOverrideDamage_key_mode, "Toggle\0\rHold\0\rForce Disable\0\rAlways on\0\0");
								ImGui::Hotkey("Damage Override Key##m_bOverrideDamage_key", &config.m_bOverrideDamage_key);

								ImGui::Combo("Hide Shots Type Bind##hide_shots_key_mode", &config.hide_shots_key_mode, "Toggle\0\rHold\0\rForce Disable\0\rAlways on\0\0");
								ImGui::Hotkey("Hide Shots Key##hide_shots_key", &config.hide_shots_key);
								ImGui::Spacing();
								ImGui::Combo("Double Tap Type Bind##double_tap_key_mode", &config.double_tap_key_mode, "Toggle\0\rHold\0\rForce Disable\0\rAlways on\0\0");
								ImGui::Hotkey("Double Tap Key##double_tap_key", &config.double_tap_key);
					
								ImGui::SliderInt("DT HITCHANCE", &config.m_aRageSettings[cur_group].m_iDoubleTapHitChance, 0, 100);

								std::vector < const char* > aDoubleTap =
								{
									"Move between shots",
									"Full stop"
								};

								if (ImGui::BeginCombo("DT Options", "Select"))
								{
									ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
										for (int i = 0; i < aDoubleTap.size(); i++)
											ImGui::CheckboxButton(aDoubleTap[i], ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y), &config.m_aRageSettings[cur_group].m_DoubleTapOptions[i]);
									}
									ImGui::PopStyleVar();

									ImGui::EndCombo();
								}
								ImGui::PopItemWidth();
							}
							ImGui::EndChild();
						
						}
						ImGui::EndGroup();
					}
					ImGui::EndGroup();
					break;
				}
				case 1: {
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Anti Aim 1", { size_mini_child_x, 426.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox("Enable", &config.m_bAntiAim);
							ImGui::Checkbox("At Targets", &config.m_bAntiAimAtTargets);
							ImGui::Checkbox("Auto Direction", &config.m_bAutoDirection);

							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::Hotkey("Manual Left Key##hide_shots_key", &config.manual_left_key);
							ImGui::Hotkey("Manual Back Key##hide_shots_key", &config.manual_back_key);
							ImGui::Hotkey("Manual Right Key##hide_shots_key", &config.manual_right_key);
							ImGui::PopItemWidth();

						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x + size_spacing);
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Anti Aim 2", { size_mini_child_x, 426.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::PushItemWidth(item_width_mini_child);

							ImGui::SliderInt("YAW ADD OFFSET", &config.m_iYawAddOffset, 0, 180);
							ImGui::SliderInt("JITTER AMOUNT", &config.m_iJitterAmount, 0, 180);

							const char* PitchModes[5] = { ("None"), ("Down"), ("Up"), ("Fake Down"), ("Fake Up") };
							ImGui::Combo("Pitch", &config.m_iPitchMode, PitchModes, IM_ARRAYSIZE(PitchModes));

							ImGui::SliderInt("LEFT FAKE LIMIT", &config.m_iLeftFakeLimit, 0, 60);
							ImGui::SliderInt("RIGHT FAKE LIMIT", &config.m_iRightFakeLimit, 0, 60);

							std::vector < std::string > InverterConditions = { ("Stand"), ("Move"), ("Air") };
							if (ImGui::BeginCombo("Auto Inverter", ("Select")))
							{
								ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
									for (int i = 0; i < InverterConditions.size(); i++)
										ImGui::CheckboxButton(InverterConditions[i].c_str(), ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y), &config.m_aInverterConditions[i]);
								}
								ImGui::PopStyleVar();
								ImGui::EndCombo();
							}
		

							ImGui::Combo("Inverter Type Bind##inverter_key_mode", &config.inverter_key_mode, "Toggle\0\rHold\0\rForce Disable\0\rAlways on\0\0");
							ImGui::Hotkey("Inverter Key##config.inverter_key", &config.inverter_key);
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x * 2 + size_spacing * 2);
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Anti Aim 3", { size_mini_child_x, 426.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox("Fakelag", &config.m_bFakeLagEnabled);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::SliderInt("LAG LIMIT", &config.m_iLagLimit, 1, 14);

							std::vector < std::string > aLagTriggers =
							{
								("Move"),
								("Air"),
								("Peek")
							};

							if (ImGui::BeginCombo("Lag Triggers", "Select"))
							{
								ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
									for (int i = 0; i < aLagTriggers.size(); i++)
										ImGui::CheckboxButton(aLagTriggers[i].c_str(), ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y), &config.m_aFakelagTriggers[i]);
								}
								ImGui::PopStyleVar();

								ImGui::EndCombo();
							}

							ImGui::SliderInt("TRIGGER LIMIT", &config.m_iTriggerLimit, 1, 14);
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					break;
				}
				case 2: {
					ImGui::BeginGroup(); {
						ImGui::BeginChild("General", { size_mini_child_x, 195.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Enable", "Включить"), &config.legit_enable);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::Combo(get_language("Weapon", "Оружие"), &utils::cur_weapon, legitbot::weapon_data, IM_ARRAYSIZE(legitbot::weapon_data));
							ImGui::PopItemWidth();
							ImGui::Checkbox(get_language("Auto Select Weapon", "Авто Выбор Оружия"), &config.legit_autoselectweapon);
							ImGui::Spacing();
							ImGui::Checkbox(get_language("Resize FOV When Zoom", "Изменять ФОВ При Зуме"), &config.legit_resize_fov);
							ImGui::Checkbox(get_language("Only In Scope", "Только В Прицеливании"), &config.legit_onlyscope);
						}
						ImGui::EndChild();
						ImGui::BeginChild("Checking", { size_mini_child_x, 138.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Check Smoke", "Проверять Дым"), &config.legitbot_for_weapon[utils::cur_weapon].legit_ignoresmoke);
							ImGui::Checkbox(get_language("Check Flash", "Проверять Ослепление"), &config.legitbot_for_weapon[utils::cur_weapon].legit_ignoreflash);
							ImGui::Checkbox(get_language("Check Jump", "Проверять Прыжок"), &config.legitbot_for_weapon[utils::cur_weapon].legit_ignorejump);
							ImGui::Checkbox(get_language("Team Attack", "Огонь По Своим"), &config.legitbot_for_weapon[utils::cur_weapon].legit_teamattack);
						}
						ImGui::EndChild();
						ImGui::BeginChild("Settings", { size_mini_child_x, 73.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Aimlock", "Притягивание"), &config.legitbot_for_weapon[utils::cur_weapon].legit_aimlock);
							//ImGui::Checkbox(get_language("Auto Fire", "Авто Выстрел"), &config.legitbot_for_weapon[utils::cur_weapon].legit_autofire);
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x + size_spacing);
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Target", { size_mini_child_x, 426.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Nearest", "Ближайшая Часть Тела"), &config.legitbot_for_weapon[utils::cur_weapon].legit_nearest);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::Combo(get_language("Hitbox", "Часть Тела"), &config.legitbot_for_weapon[utils::cur_weapon].legit_hitbox, get_language("Head\0\rNeck\0\rBody\0\rThorax\0\rChest\0\0", "Голова\0\rШея\0\rТело\0\rГрудь\0\rЖивот\0\0"));
							ImGui::SliderFloat(get_language("FOV", "ФОВ"), &config.legitbot_for_weapon[utils::cur_weapon].legit_fov, 0.f, 20.f, "%.2f");
							ImGui::Checkbox(get_language("Dynamic FOV", "Динамичный ФОВ"), &config.legitbot_for_weapon[utils::cur_weapon].legit_dynamic_fov);
							ImGui::SliderFloat(get_language("SMOOTHING", "ПЛАВНОСТЬ"), &config.legitbot_for_weapon[utils::cur_weapon].legit_smooth, 0.f, 40.f, "%.2f");
							ImGui::Checkbox(get_language("Enable RCS FOV", "Включить РКС ФОВ"), &config.legitbot_for_weapon[utils::cur_weapon].legit_fov_after_enable);
							ImGui::SliderFloat(get_language("RCS FOV", "РКС ФОВ"), &config.legitbot_for_weapon[utils::cur_weapon].legit_fov_after, 0.f, 20.f, "%.2f");
							ImGui::Checkbox(get_language("Enable RCS Smooth", "Включить РКС Плавность"), &config.legitbot_for_weapon[utils::cur_weapon].legit_smooth_after_enable);
							ImGui::SliderFloat(get_language("RCS SMOOTHING", "РКС ПЛАВНОСТЬ"), &config.legitbot_for_weapon[utils::cur_weapon].legit_smooth_after, 0.f, 40.f, "%.2f");
							ImGui::SliderInt(get_language("RCS VALUE - HORIZONTAL", "РКС ЗНАЧЕНИЕ - ГОРИЗОНТАЛЬ"), &config.legitbot_for_weapon[utils::cur_weapon].legit_rcs_x, 0, 100);
							ImGui::SliderInt(get_language("RCS VALUE - VERTICAL", "РКС ЗНАЧЕНИЕ - ВЕРТИКАЛЬ"), &config.legitbot_for_weapon[utils::cur_weapon].legit_rcs_y, 0, 100);
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x * 2 + size_spacing * 2);
					ImGui::BeginGroup(); {
						ImGui::BeginChild("pSilent Settings", { size_mini_child_x, 127.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("pSilent Enable", "Включить Невидимую Наводку"), &config.legitbot_for_weapon[utils::cur_weapon].legit_psilent_enable);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::Combo(get_language("Type##pSilent", "Тип##pSilent"), &config.legitbot_for_weapon[utils::cur_weapon].legit_psilent_type, get_language("1 Bullet\0\rWith Aim\0\0", "1 Пуля\0\rС Аимом\0\0"));
							ImGui::SliderFloat(get_language("FOV##pSilent", "ФОВ##pSilent"), &config.legitbot_for_weapon[utils::cur_weapon].legit_psilent_fov, 0.f, 10.f, "%.2f");
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
						ImGui::BeginChild("Delays", { size_mini_child_x, 91.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::SliderFloat(get_language("KILL DELAY", "ЗАДЕРЖКА ПОСЛЕ УБИЙСТВА"), &config.legitbot_for_weapon[utils::cur_weapon].legit_delay_after_kill, 0.f, 50.f, "%.2f");
							ImGui::SliderFloat(get_language("SHOT DELAY", "ЗАДЕРЖКА ПЕРЕД ВЫСТРЕЛОМ"), &config.legitbot_for_weapon[utils::cur_weapon].legit_delay_shot, 0.f, 50.f, "%.2f");
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
						ImGui::BeginChild("Damage", { size_mini_child_x, 89.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Auto Wall", "Авто Наведение"), &config.legitbot_for_weapon[utils::cur_weapon].legit_autowall);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::SliderFloat(get_language("MIN DAMAGE", "МИН УРОН"), &config.legitbot_for_weapon[utils::cur_weapon].legit_mindamage, 0.f, 100.f, "%.2f");
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
						ImGui::BeginChild("Legit Backtrack", { size_mini_child_x, 89.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Backtrack Enable", "Включить Бэктрэк"), &config.legit_backtrack);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::SliderInt(get_language("VALUE##legit_backtrack_time", "ЗНАЧЕНИЕ##legit_backtrack_time"), &config.legit_backtrack_time, 1, config.misc_fake_latency ? 400 : 200);
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					break;
				}
				case 3: {
					ImGui::BeginGroup(); {
						ImGui::BeginChild("General", { size_mini_child_x, 105.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Enable", "Включить"), &config.esp_enable);
							ImGui::Checkbox(get_language("Enemy Only", "Только Противники"), &config.esp_enemy_only);
							ImGui::Checkbox(get_language("Invisible", "Невидимые"), &config.esp_invisible);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color CT Invisible", "Цвет КТ Невидимый"), config.color_esp_ct_invisible, color_edit_flags);
							ImGui::SameLine(175.f);
							ImGui::ColorEdit3(get_language("Color T Invisible", "Цвет Т Невидимый"), config.color_esp_t_invisible, color_edit_flags);
						}
						ImGui::EndChild();
						ImGui::BeginChild("Box", { size_mini_child_x, 311.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Box", "Боксы"), &config.esp_box);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color CT Visible", "Цвет КТ Видимый"), config.color_esp_ct_visible, color_edit_flags);
							ImGui::SameLine(175.f);
							ImGui::ColorEdit3(get_language("Color T Visible", "Цвет Т Видимый"), config.color_esp_t_visible, color_edit_flags);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::Combo(get_language("Type", "Тип"), &config.esp_box_type, get_language("Classic\0\rThickened\0\rCorner\0\0", "Классические\0\rУтолщенные\0\rУгловые\0\0"));
							ImGui::Checkbox(get_language("Outline", "Обводка"), &config.esp_box_outline);
							ImGui::Checkbox(get_language("Fill", "Заливка"), &config.esp_box_filled);
							ImGui::Checkbox(get_language("Gradient##esp_box_filled_fade", "Градиент##esp_box_filled_fade"), &config.esp_box_filled_fade);
							ImGui::SliderInt(get_language("FILL ALPHA", "ПРОЗРАЧНОСТЬ ЗАЛИВКИ"), &config.esp_box_filled_alpha, 0, 255);
							ImGui::SliderInt(get_language("ROUNDING", "ЗАКРУГЛЕНИЕ"), &config.esp_box_rounding, 0, 15);
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x + size_spacing);
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Preview", { size_mini_child_x, 327.f }, true); {
							if (config.esp_enable)
								player_esp_preview();
						}
						ImGui::EndChild();
						ImGui::BeginChild("Sound Esp", { size_mini_child_x, 89.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Sound Esp Enable##esp_sound_enable", "Включить Звуковое Есп##esp_sound_enable"), &config.esp_sound_enable);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color Sound Esp", "Цвет Звукового Есп"), config.color_esp_sound, color_edit_flags);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::SliderFloat(get_language("SIZE", "РАЗМЕР"), &config.esp_sound_size, 5.0f, 30.0f, "%.2f");
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x * 2 + size_spacing * 2);
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Info", { size_mini_child_x, 426.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Name", "Имя"), &config.esp_name);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color Name", "Цвет Имен"), config.color_esp_name, color_edit_flags);
							ImGui::Spacing();
							ImGui::Checkbox(get_language("Weapon", "Оружие"), &config.esp_weapon);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color Weapon", "Цвет Оружия"), config.color_esp_weapon, color_edit_flags);
							ImGui::Checkbox(get_language("Icon", "Иконка"), &config.esp_weapon_icon);
							ImGui::Spacing();
							ImGui::Checkbox(get_language("Health Bar", "Полоска Здоровья"), &config.esp_health);
							ImGui::Checkbox(get_language("Gradient", "Градиент"), &config.esp_health_fade);
							ImGui::Checkbox(get_language("Background", "Задний Фон"), &config.esp_health_background);
							ImGui::Spacing();
							ImGui::Checkbox(get_language("Armor Bar", "Полоска Брони"), &config.esp_armor);
							ImGui::Checkbox(get_language("Gradient##esp_armor_fade", "Градиент##esp_armor_fade"), &config.esp_armor_fade);
							ImGui::Checkbox(get_language("Background##esp_armor_background", "Задний Фон##esp_armor_background"), &config.esp_armor_background);
							ImGui::Spacing();
							ImGui::Checkbox(get_language("Line", "Линии"), &config.esp_line);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color Line", "Цвет Линий"), config.color_esp_line, color_edit_flags);
						}
						ImGui::EndChild();

					}
					ImGui::EndGroup();
					break;
				}
				case 4: {
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Damage Indicator", { size_mini_child_x, 89.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Enable Damage Indicator", "Включить Показ Урона"), &config.visuals_damage_indicator);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color Damage Indicator", "Цвет Показа Урона"), config.color_visuals_damage_indicator, color_edit_flags);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::SliderFloat(get_language("SPEED", "СКОРОСТЬ"), &config.visuals_damage_indicator_speed, 0.0f, 5.0f, "%.2f");
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
						ImGui::BeginChild("Danger zone", { size_mini_child_x, 327.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGuiContext& g = *GImGui;
							ImGui::Checkbox(get_language("Enable Danger Zone", "Включить Запретную Зону"), &config.esp_danger_zone_enable);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color Danger Zone", "Цвет Запретной Зоны"), config.color_esp_danger_zone, color_edit_flags);
							ImGui::BeginChild("Danger Zone Esp", ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y * 6.f), false, ImGuiWindowFlags_::ImGuiWindowFlags_NoChildShadow); {
								ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
									const char* danger_zone[]{
										get_language("Turret", "Турель"),
										get_language("Drones", "Дроны"),
										get_language("Cash", "Деньги"),
										get_language("Cash dufflebag", "Сумка с деньгами"),
										get_language("Pistol case", "Пистолетный кейс"),
										get_language("Light case", "Легкий кейс"),
										get_language("Heavy case", "Тяжелый кейс"),
										get_language("Explosive case", "Взрывной кейс"),
										get_language("Tools case", "Инструментный кейс"),
										get_language("Full armor", "Полная броня"),
										get_language("Body armor", "Бронежилет"),
										get_language("Helmet", "Шлем"),
										get_language("Parachute", "Парашют"),
										get_language("Briefcase", "Портфель"),
										get_language("Tablet upgrade", "Обновление планшета"),
										get_language("Exojump", "Экзопрыжок"),
										get_language("Ammobox", "Патроны"),
										get_language("Radar jammer", "Глушитель сигнала"),
									};
									ImGui::PushFont(roboto_bold_elements);
									for (size_t i = 0; i < IM_ARRAYSIZE(danger_zone); i++)
										ImGui::CheckboxButton(danger_zone[i], ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y), &config.esp_danger_zone[i]);
									ImGui::PopFont();
								}
								ImGui::PopStyleVar();
							}
							ImGui::EndChild();
							ImGui::Checkbox(get_language("Box", "Боксы"), &config.esp_danger_zone_box);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::SliderFloat(get_language("MAX DISTANCE", "МАКСИМАЛЬНАЯ ДИСТАНЦИЯ"), &config.esp_danger_zone_distance, 0.0f, 5000.0f, "%.2f");
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x + size_spacing);
					ImGui::BeginGroup(); {
						ImGui::BeginChild("World Weapons", { size_mini_child_x, 145.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("World Weapons", "Оружия На Земле"), &config.esp_world_weapon_enable);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color World Weapons", "Цвет Оружий На Карте"), config.color_esp_world_weapon, color_edit_flags);
							ImGui::BeginChild("World Weapons Esp", ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y * 2.f), false, ImGuiWindowFlags_::ImGuiWindowFlags_NoChildShadow); {
								ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
									ImGui::PushFont(roboto_bold_elements);
									ImGui::CheckboxButton(get_language("Weapons", "Оружия"), ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y), &config.esp_world_weapon_weapons);
									ImGui::CheckboxButton(get_language("Planted Bomb", "Заложенная Бомба"), ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y), &config.esp_world_weapon_planted_bomb);
									ImGui::PopFont();
								}
								ImGui::PopStyleVar();
							}
							ImGui::EndChild();
							ImGui::Checkbox(get_language("Box##esp_world_weapon_box", "Боксы##esp_world_weapon_box"), &config.esp_world_weapon_box);
						}
						ImGui::EndChild();
						ImGui::BeginChild("Radar", { size_mini_child_x, 271.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Radar", "Радар"), &config.radar_enable);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color CT Visible", "Цвет КТ Видимый"), config.color_radar_ct_visible, color_edit_flags);
							ImGui::SameLine(175.f);
							ImGui::ColorEdit3(get_language("Color T Visible", "Цвет Т Видимый"), config.color_radar_t_visible, color_edit_flags);
							ImGui::Checkbox(get_language("Enemy Only", "Только Противники"), &config.radar_enemy_only);
							ImGui::Checkbox(get_language("Invisible", "Невидимые"), &config.radar_invisible);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color CT Invisible", "Цвет КТ Невидимый"), config.color_radar_ct_invisible, color_edit_flags);
							ImGui::SameLine(175.f);
							ImGui::ColorEdit3(get_language("Color T Invisible", "Цвет Т Невидимый"), config.color_radar_t_invisible, color_edit_flags);
							ImGui::Checkbox(get_language("Texture", "Текстура"), &config.radar_texture);
							ImGui::Checkbox(get_language("Grenades", "Гранаты"), &config.radar_grenades);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3("Color Grenades", config.color_radar_grenades, color_edit_flags);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::SliderInt(get_language("RANGE", "ОТДАЛЕНИЕ"), &config.radar_range, 1, 5000);
							ImGui::SliderInt(get_language("SIZE", "РАЗМЕР"), &config.radar_size, 50, 400);
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x * 2 + size_spacing * 2);
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Bullet Tracers", { size_mini_child_x, 126.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Enable Bullet Tracers", "Включить Траектория Пуль"), &config.visuals_bullet_tracers);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color Bullet Tracers", "Цвет Траектории Пуль"), config.color_visuals_bullet_tracers, color_edit_flags);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::SliderFloat(get_language("WIDTH", "РАЗМЕР"), &config.visuals_bullet_tracers_width, 1.0f, 10.0f, "%.2f");
							ImGui::SliderFloat(get_language("LIFE", "ПРОДОЛЖИТЕЛЬНОСТЬ"), &config.visuals_bullet_tracers_life, 1.0f, 10.0f, "%.2f");
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, ImGui::ColConvertToFloat(0.f, 0.f, 0.f, 0.f));
						ImGui::BeginChild("Pages", { size_mini_child_x, 30.f }, true); {
							ImGuiWindow* window = ImGui::GetCurrentWindow();
							ImVec2 size{ size_mini_child_x / 2.f, 30.f };
							ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f)); {
								if (ImGui::ButtonTabs("Page 1", size, visuals_page == 0))
									visuals_page = 0;
								ImGui::SameLine();
								if (ImGui::ButtonTabs("Page 2", size, visuals_page == 1))
									visuals_page = 1;
							}
							ImGui::PopStyleVar();

							static float current_sub_selected_pos = 0.f;
							static float new_sub_selected_pos = 0.f;

							if (current_sub_selected_pos < new_sub_selected_pos) {
								current_sub_selected_pos += 2000.f * ImGui::GetIO().DeltaTime;
								if (current_sub_selected_pos >= new_sub_selected_pos)
									current_sub_selected_pos = new_sub_selected_pos;
							}

							else if (current_sub_selected_pos > new_sub_selected_pos) {
								current_sub_selected_pos -= 2000.f * ImGui::GetIO().DeltaTime;
								if (current_sub_selected_pos <= new_sub_selected_pos)
									current_sub_selected_pos = new_sub_selected_pos;
							}

							new_sub_selected_pos = size.x * float(visuals_page);

							window->DrawList->PushClipRectFullScreen();
							window->DrawList->AddRectFilled({ window->Pos.x + current_sub_selected_pos,  window->Pos.y }, { window->Pos.x + current_sub_selected_pos + size.x,  window->Pos.y + size.y }, ImGui::GetColorU32(ImGui::ColConvertToFloat(34.f, 52.f, 76.f)));

							const char* char_visuals[] = { get_language("PAGE 1", "СТРАНИЦА 1"), get_language("PAGE 2", "СТРАНИЦА 2") };

							for (size_t i = 0; i < IM_ARRAYSIZE(char_visuals); i++) {
								const auto text_size = ImGui::CalcTextSize(char_visuals[i]);
								window->DrawList->AddText(ImVec2(window->Pos.x + (size.x * i) + (((window->Pos.x + size.x) - window->Pos.x) / 2.f) - (text_size.x / 2.f), window->Pos.y + 8.5f), ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)), char_visuals[i]);
							}
							window->DrawList->PopClipRect();
						}
						ImGui::EndChild();
						ImGui::PopStyleColor();
						ImGui::BeginChild("Other", { size_mini_child_x, 250.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							if (visuals_page == 0) {
								ImGui::Checkbox(get_language("Dynamic Lights", "Динамичная Подсветка"), &config.visuals_dynamic_lights);
								ImGui::SameLine(200.f);
								ImGui::ColorEdit3(get_language("Color Dynamic Lights", "Цвет Динамичной Подсветки"), config.color_visuals_dynamic_lights, color_edit_flags);
								ImGui::Checkbox(get_language("Spread Crosshair", "Прицел Разброса"), &config.visuals_spread_crosshair);
								ImGui::SameLine(200.f);
								ImGui::ColorEdit3(get_language("Color Spread Crosshair", "Цвет Прицела Разброса"), config.color_visuals_spread_crosshair, color_edit_flags);
								ImGui::Checkbox(get_language("Sniper Crosshair", "Прицел Снайперок"), &config.visuals_sniper_crosshair);
								ImGui::Checkbox(get_language("Taser Distance", "Дистанция Шокера"), &config.visuals_taser_range);
								ImGui::Checkbox(get_language("Draw FOV", "Показ ФОВ"), &config.visuals_draw_fov);
								ImGui::Checkbox(get_language("Draw pSilent FOV", "Показ пСайлент ФОВ"), &config.visuals_draw_psilent_fov);
								ImGui::Checkbox(get_language("Draw Enemy Circle", "Показ Круга На Противниках"), &config.visuals_enemy_circle);
							}
							else if (visuals_page == 1) {
								ImGui::Checkbox(get_language("Bomb Timer", "Таймер Бомбы"), &config.visuals_bomb_timer);
								ImGui::Checkbox(get_language("Grenade Prediction", "Траектория Гранат"), &config.visuals_grenade_prediction);
								ImGui::SameLine(200.f);
								ImGui::ColorEdit3(get_language("Color Grenade Prediction", "Цвет Траектории Гранат"), config.color_visuals_grenade_prediction, color_edit_flags);
								ImGui::Checkbox(get_language("Hit Marker", "Маркер При Попадании"), &config.visuals_hit_marker);
								ImGui::Checkbox(get_language("Hit Hitbox", "Хитбокс При Попадании"), &config.visuals_hit_hitbox);
								ImGui::SameLine(200.f);
								ImGui::ColorEdit3(get_language("Color Hit Hitbox", "Цвет Хитбокса При Попадании"), config.color_visuals_hit_hitbox, color_edit_flags);
								ImGui::Checkbox(get_language("Hit Effect", "Эффект При Попадании"), &config.visuals_hit_effect);
								ImGui::PushItemWidth(item_width_mini_child);
								ImGui::Combo(get_language("Hit Sound", "Звук При Попадании"), &config.visuals_hit_sound, get_language("Off\0\rMetallic\0\rPress\0\rBell\0\rGlassy\0\rReady\0\rWarning\0\rDefault\0\0", "Выкл\0\rМеталлический\0\rНажатие\0\rКолокольчик\0\rСтекло\0\rГотов\0\rПредупреждение\0\rОбычный\0\0"));
								ImGui::PopItemWidth();
							}

						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					break;
				}
				case 5: {
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Chams Player", { size_mini_child_x, 326.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Chams Player", "Чамсы На Игроков"), &config.chams_enable);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color CT Visible", "Цвет КТ Видимый"), config.color_chams_ct_visible, color_edit_flags);
							ImGui::SameLine(175.f);
							ImGui::ColorEdit3(get_language("Color T Visible", "Цвет Т Видимый"), config.color_chams_t_visible, color_edit_flags);
							ImGui::Checkbox(get_language("Enemy Only", "Только Противники"), &config.chams_enemy_only);
							ImGui::Checkbox(get_language("Invisible", "Невидимые"), &config.chams_invisible);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color CT Invisible", "Цвет КТ Невидимый"), config.color_chams_ct_invisible, color_edit_flags);
							ImGui::SameLine(175.f);
							ImGui::ColorEdit3(get_language("Color T Invisible", "Цвет Т Невидимый"), config.color_chams_t_invisible, color_edit_flags);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::Combo(get_language("Chams Type", "Тип Чамсов"), &config.chams_type, get_language("Flat\0\rTexture\0\rAnimated\0\rPearlescent\0\rGhost\0\0", "Плоские\0\rТекстурированные\0\rАнимированные\0\rПереливающиеся\0\rПризрачные\0\0"));
							ImGui::Checkbox(get_language("Wireframe", "Сетчатые##chams_wireframe"), &config.chams_wireframe);
							if(config.chams_type == 3)
								ImGui::SliderFloat(get_language("PEARLESCENT VALUE", "ПЕРЕЛИВАНИЕ"), &config.chams_pearlescent_value, 0.0f, 10.0f, "%.2f");
							else if (config.chams_type == 5)
								ImGui::SliderFloat(get_language("BRIGHTNESS", "ЯРКОСТЬ"), &config.chams_brightness, 1.0f, 100.0f, "%.2f");

							ImGui::Checkbox(get_language("Chams More", "Дополнительные Чамсы"), &config.chams_more);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color CT More", "Цвет КТ Дополнительный"), config.color_chams_more_ct, color_edit_flags);
							ImGui::SameLine(175.f);
							ImGui::ColorEdit3(get_language("Color T More", "Цвет Т Дополнительный"), config.color_chams_more_t, color_edit_flags);
							ImGui::Combo(get_language("Chams More Type", "Дополнительный Тип Чамсов"), &config.chams_more_type, get_language("Glow\0\rStrong Glow\0\rGlow Pulse\0\rGloss\0\rCrystal\0\rGlass\0\0", "Выкл\0\rСвечение\0\rСильное Свечение\0\rПульсирующее Свечение\0\rБлеск\0\rКристал\0\rСтекло\0\0"));
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
						ImGui::BeginChild("Chams Backtrack", { size_mini_child_x, 90.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Chams Backtrack", "Чамсы На Бэктрэк"), &config.legit_backtrack_chams_enable);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::Combo(get_language("Type", "Тип"), &config.legit_backtrack_chams, get_language("All\0\rLast\0\0", "Все тики\0\rПоследний тик\0\0"));
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x + size_spacing);
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Chams Arms", { size_mini_child_x, 208.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Chams Arms", "Чамсы На Руки"), &config.chams_arms);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color Arms More", "Цвет Рук Дополнительный"), config.color_chams_arms_more, color_edit_flags);
							ImGui::SameLine(175.f);
							ImGui::ColorEdit3(get_language("Color Arms", "Цвет Рук"), config.color_chams_arms, color_edit_flags);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::Combo(get_language("Chams Type", "Тип Чамсов"), &config.chams_arms_type, get_language("Flat\0\rTexture\0\rAnimated\0\rPearlescent\0\rGhost\0\0", "Плоские\0\rТекстурированные\0\rАнимированные\0\rПереливающиеся\0\rПризрачные\0\0"));
							ImGui::Checkbox(get_language("Wireframe", "Сетчатые"), &config.chams_arms_wireframe);
							if (config.chams_arms_type == 3)
								ImGui::SliderFloat(get_language("PEARLESCENT VALUE", "ПЕРЕЛИВАНИЕ"), &config.chams_arms_pearlescent_value, 0.0f, 10.0f, "%.2f");
							ImGui::Combo(get_language("Chams More", "Дополнительный Тип Чамсов"), &config.chams_arms_more_type, get_language("Off\0\rGlow\0\rStrong Glow\0\rGlow Pulse\0\rGloss\0\rCrystal\0\rGlass\0\0", "Выкл\0\rСвечение\0\rСильное Свечение\0\rПульсирующее Свечение\0\rБлеск\0\rКристал\0\rСтекло\0\0"));
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
						ImGui::BeginChild("Chams Weapon", { size_mini_child_x, 208.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::Checkbox(get_language("Chams Weapon", "Чамсы На Оружие"), &config.chams_weapon);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color Weapon More", "Цвет Оружия Дополнительный"), config.color_chams_weapon_more, color_edit_flags);
							ImGui::SameLine(175.f);
							ImGui::ColorEdit3(get_language("Color Weapon", "Цвет Оружия"), config.color_chams_weapon, color_edit_flags);
							ImGui::Combo(get_language("Chams Type##chams_weapon", "Тип Чамсов##chams_weapon"), &config.chams_weapon_type, get_language("Flat\0\rTexture\0\rAnimated\0\rPearlescent\0\rGhost\0\0", "Плоские\0\rТекстурированные\0\rАнимированные\0\rПереливающиеся\0\rПризрачные\0\0"));
							ImGui::Checkbox(get_language("Wireframe", "Сетчатые"), &config.chams_weapon_wireframe);
							if (config.chams_weapon_type == 3)
								ImGui::SliderFloat(get_language("PEARLESCENT VALUE", "ПЕРЕЛИВАНИЕ"), &config.chams_weapon_pearlescent_value, 0.0f, 10.0f, "%.2f");
							ImGui::Combo(get_language("Chams More", "Дополнительный Тип Чамсов"), &config.chams_weapon_more_type, get_language("Off\0\rGlow\0\rStrong Glow\0\rGlow Pulse\0\rGloss\0\rCrystal\0\rGlass\0\0", "Выкл\0\rСвечение\0\rСильное Свечение\0\rПульсирующее Свечение\0\rБлеск\0\rКристал\0\rСтекло\0\0"));
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x * 2 + size_spacing * 2);
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Glow Player", { size_mini_child_x, 156.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Glow Player", "Свечение На Игроков"), &config.glow_enable);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color CT Visible", "Цвет КТ Видимый"), config.color_glow_ct_visible, color_edit_flags);
							ImGui::SameLine(175.f);
							ImGui::ColorEdit3(get_language("Color T Visible", "Цвет Т Видимый"), config.color_glow_t_visible, color_edit_flags);
							ImGui::Checkbox(get_language("Enemy Only", "Только Противники"), &config.glow_enemy_only);
							ImGui::Checkbox(get_language("Invisible", "Невидимые"), &config.glow_invisible);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color CT Invisible", "Цвет КТ Невидимый"), config.color_glow_ct_invisible, color_edit_flags);
							ImGui::SameLine(175.f);
							ImGui::ColorEdit3(get_language("Color T Invisible", "Цвет Т Невидимый"), config.color_glow_t_invisible, color_edit_flags);
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::Combo(get_language("Style##glow_style", "Стиль##glow_style"), &config.glow_style, get_language("Default\0\rPulse\0\rOutline\0\rOutline Pulse\0\0", "Обычный\0\rПульсация\0\rОбводка\0\rПульсирующая Обводка\0\0"));
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
						ImGui::BeginChild("Glow World", { size_mini_child_x, 260.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Glow World", "Свечение На Земле"), &config.glow_world_enable);
							ImGui::Checkbox(get_language("Chicken", "Курицы"), &config.glow_chicken);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color Chicken", "Цвет Куриц"), config.color_glow_chicken, color_edit_flags);
							ImGui::Checkbox(get_language("Bomb", "Бомба"), &config.glow_bomb);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color Bomb", "Цвет Бомбы"), config.color_glow_bomb, color_edit_flags);
							ImGui::Checkbox(get_language("Planted Bomb", "Заложенная Бомба"), &config.glow_planted_bomb);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color Planted Bomb", "Цвет Заложенной Бомбы"), config.color_glow_planted_bomb, color_edit_flags);
							ImGui::Checkbox(get_language("Defuse", "Набор Сапера"), &config.glow_defuses);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color Defuse", "Цвет Набора Сапера"), config.color_glow_defuses, color_edit_flags);
							ImGui::Checkbox(get_language("Weapons", "Оружия"), &config.glow_weapons);
							ImGui::SameLine(200.f);
							ImGui::ColorEdit3(get_language("Color Weapons", "Цвет Оружий"), config.color_glow_weapons, color_edit_flags);
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					break;
				}
				case 6: {
					ImGui::BeginGroup(); {
						ImGui::PushStyleColor(ImGuiCol_ChildBg, g.Style.Colors[ImGuiCol_WindowBg]);
						ImGui::BeginChild("Item List", { size_mini_child_x, 426.f }, false); {
							ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
								for (const auto& weapon : weaponids.m_types[type_pistol]) {
									ImGui::PushID((void*)(intptr_t)weapon->get_definition_index());
									const bool item_selected = (weapon->get_definition_index() == inventory_definition_index);

									if (ImGui::ListButton(utils::localize_text(weapon->get_item_base_name()).c_str(), { size_mini_child_x, 42.6f }, item_selected)) {
										item_selected;
										inventory_definition_index = weapon->get_definition_index();
										for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
										inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = inventory_other = 0;
										inventory_wear = 0.f;
										filter_sticker.Clear();
									}

									ImGui::PopID();
								}

								for (const auto& weapon : weaponids.m_types[type_smg]) {
									ImGui::PushID((void*)(intptr_t)weapon->get_definition_index());
									const bool item_selected = (weapon->get_definition_index() == inventory_definition_index);

									if (ImGui::ListButton(utils::localize_text(weapon->get_item_base_name()).c_str(), { size_mini_child_x, 42.6f }, item_selected)) {
										item_selected;
										inventory_definition_index = weapon->get_definition_index();
										for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
										inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = inventory_other = 0;
										inventory_wear = 0.f;
										filter_sticker.Clear();
									}

									ImGui::PopID();
								}

								for (const auto& weapon : weaponids.m_types[type_shotgun]) {
									ImGui::PushID((void*)(intptr_t)weapon->get_definition_index());
									const bool item_selected = (weapon->get_definition_index() == inventory_definition_index);

									if (ImGui::ListButton(utils::localize_text(weapon->get_item_base_name()).c_str(), { size_mini_child_x, 42.6f }, item_selected)) {
										item_selected;
										inventory_definition_index = weapon->get_definition_index();
										for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
										inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = inventory_other = 0;
										inventory_wear = 0.f;
										filter_sticker.Clear();
									}

									ImGui::PopID();
								}

								for (const auto& weapon : weaponids.m_types[type_machinegun]) {
									ImGui::PushID((void*)(intptr_t)weapon->get_definition_index());
									const bool item_selected = (weapon->get_definition_index() == inventory_definition_index);

									if (ImGui::ListButton(utils::localize_text(weapon->get_item_base_name()).c_str(), { size_mini_child_x, 42.6f }, item_selected)) {
										item_selected;
										inventory_definition_index = weapon->get_definition_index();
										for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
										inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = inventory_other = 0;
										inventory_wear = 0.f;
										filter_sticker.Clear();
									}

									ImGui::PopID();
								}

								for (const auto& weapon : weaponids.m_types[type_rifle]) {
									ImGui::PushID((void*)(intptr_t)weapon->get_definition_index());
									const bool item_selected = (weapon->get_definition_index() == inventory_definition_index);

									if (ImGui::ListButton(utils::localize_text(weapon->get_item_base_name()).c_str(), { size_mini_child_x, 42.6f }, item_selected)) {
										item_selected;
										inventory_definition_index = weapon->get_definition_index();
										for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
										inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = inventory_other = 0;
										inventory_wear = 0.f;
										filter_sticker.Clear();
									}

									ImGui::PopID();
								}

								for (const auto& weapon : weaponids.m_types[type_sniper]) {
									ImGui::PushID((void*)(intptr_t)weapon->get_definition_index());
									const bool item_selected = (weapon->get_definition_index() == inventory_definition_index);

									if (ImGui::ListButton(utils::localize_text(weapon->get_item_base_name()).c_str(), { size_mini_child_x, 42.6f }, item_selected)) {
										item_selected;
										inventory_definition_index = weapon->get_definition_index();
										for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
										inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = inventory_other = 0;
										inventory_wear = 0.f;
										filter_sticker.Clear();
									}

									ImGui::PopID();
								}

								for (const auto& weapon : weaponids.m_types[type_knife]) {

									if (weapon->get_definition_index() < 500)
										continue;

									ImGui::PushID((void*)(intptr_t)weapon->get_definition_index());
									const bool item_selected = (weapon->get_definition_index() == inventory_definition_index);

									if (ImGui::ListButton(utils::localize_text(weapon->get_item_base_name()).c_str(), { size_mini_child_x, 42.6f }, item_selected)) {
										item_selected;
										inventory_definition_index = weapon->get_definition_index();
										for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
										inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = inventory_other = 0;
										inventory_wear = 0.f;
										filter_sticker.Clear();
									}

									ImGui::PopID();
								}

								for (const auto& weapon : weaponids.m_types[type_gloves]) {
									const auto hud_name_hash = get_hash(weapon->get_item_base_name());

									if (hud_name_hash == utils::get_const(XORSTR("#CSGO_Wearable_t_defaultgloves")) || hud_name_hash == utils::get_const(XORSTR("#CSGO_Wearable_ct_defaultgloves")))
										continue;

									ImGui::PushID((void*)(intptr_t)weapon->get_definition_index());
									const bool item_selected = (weapon->get_definition_index() == inventory_definition_index);

									if (ImGui::ListButton(utils::localize_text(weapon->get_item_base_name()).c_str(), { size_mini_child_x, 42.6f }, item_selected)) {
										item_selected;
										inventory_definition_index = weapon->get_definition_index();
										for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
										inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = inventory_other = 0;
										inventory_wear = 0.f;
										filter_sticker.Clear();
									}

									ImGui::PopID();
								}

								for (const auto& weapon : weaponids.m_types[type_music_kit]) {
									if (weapon->get_definition_index() != 58)
										continue;

									ImGui::PushID((void*)(intptr_t)weapon->get_definition_index());
									const bool item_selected = (weapon->get_definition_index() == inventory_definition_index);

									if (ImGui::ListButton(utils::localize_text(weapon->get_item_base_name()).c_str(), { size_mini_child_x, 42.6f }, item_selected)) {
										item_selected;
										inventory_definition_index = weapon->get_definition_index();
										for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
										inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = inventory_other = 0;
										inventory_wear = 0.f;
										filter_sticker.Clear();
									}

									ImGui::PopID();
								}

								if (ImGui::ListButton(utils::localize_text(XORSTR("#CSGO_Type_Collectible")).c_str(), { size_mini_child_x, 42.6f }, inventory_other == 1)) {
									inventory_other = 1;
									inventory_definition_index = 874;
									for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
									inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = 0;
									inventory_wear = 0.f;
									filter_sticker.Clear();
								}

								if (ImGui::ListButton(utils::localize_text(XORSTR("#Type_CustomPlayer")).c_str(), { size_mini_child_x, 42.6f }, inventory_other == 2)) {
									inventory_other = 2;
									inventory_definition_index = 5105;
									for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
									inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = 0;
									inventory_wear = 0.f;
									filter_sticker.Clear();
								}

								if (ImGui::ListButton(utils::localize_text(XORSTR("#CSGO_Type_WeaponCase")).c_str(), { size_mini_child_x, 42.6f }, inventory_other == 3)) {
									inventory_other = 3;
									inventory_definition_index = 4001;
									for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
									inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = 0;
									inventory_wear = 0.f;
									filter_sticker.Clear();
								}

								if (ImGui::ListButton(utils::localize_text(XORSTR("#CSGO_Tool_WeaponCase_KeyTag")).c_str(), { size_mini_child_x, 42.6f }, inventory_other == 4)) {
									inventory_other = 4;
									inventory_definition_index = 1203;
									for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
									inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = 0;
									inventory_wear = 0.f;
									filter_sticker.Clear();
								}
							}
							ImGui::PopStyleVar();
						}
						ImGui::EndChild();
						ImGui::PopStyleColor();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x + size_spacing);
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Skin preview", { size_mini_child_x, 120.f }, true); {
							static int rarity; 
							ImGuiWindow* window = ImGui::GetCurrentWindow();

							if (inventory_paint_kit) {
								if (get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_music_kit) 
									rarity = 3;
								else
									rarity = changer::get_rarity(weaponids.m_list[inventory_definition_index]->get_rarity(), config.inventory_skin_info[inventory_paint_kit]->get_rarity_value());
							}
							else {
								if (get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_knife)
									rarity = 6;
								else if (get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_collectible || get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_custom_player || 
									get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_case || get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_key)
									rarity = changer::get_rarity(weaponids.m_list[inventory_definition_index]->get_rarity(), 1);
								else
									rarity = 0;
							}

							switch (rarity) {
							case 1:
								color_rarity = ImVec4(0.69f, 0.76f, 0.85f, 1.f);
								break;
							case 2:
								color_rarity = ImVec4(0.36f, 0.60f, 0.85f, 1.f);
								break;
							case 3:
								color_rarity = ImVec4(0.29f, 0.41f, 1.f, 1.f);
								break;
							case 4:
								color_rarity = ImVec4(0.53f, 0.27f, 1.f, 1.f);
								break;
							case 5:
								color_rarity = ImVec4(0.82f, 0.17f, 0.90f, 1.f);
								break;
							case 6:
								color_rarity = ImVec4(0.92f, 0.29f, 0.29f, 1.f);
								break;
							case 7:
								color_rarity = ImVec4(0.89f, 0.68f, 0.22f, 1.f);
								break;
							default:
								color_rarity = ImVec4(0.f, 0.f, 0.f, 0.f);
								break;
							}

							window->DrawList->AddRectFilledMultiColor(window->Pos, { window->Pos.x + window->Size.x, window->Pos.y + window->Size.y }, ImGui::GetColorU32(color_rarity), ImGui::GetColorU32(color_clear), ImGui::GetColorU32(color_clear), ImGui::GetColorU32(color_rarity));

							if (inventory_definition_index != default_definition_index) {
								if (m_skin_texture) {
									m_skin_texture->Release();
									m_skin_texture = nullptr;
								}
							}
							default_definition_index = inventory_definition_index;

							if (inventory_definition_index) {
								if (default_paint_kit != inventory_paint_kit) {
									if (m_skin_texture) {
										m_skin_texture->Release();
										m_skin_texture = nullptr;
									}
								}
								default_paint_kit = inventory_paint_kit;
							}

							if (inventory_definition_index && !m_skin_texture) {
								void* entry;
								if (inventory_paint_kit) {
									if (get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_music_kit)
										entry = g_base_file_sys->open(std::string(XORSTR("resource/flash/") + std::string(config.inventory_music_kits_info[inventory_paint_kit]->get_inventory_image()) + ".png").c_str(), "r", "GAME");
									else
										entry = g_base_file_sys->open(std::string(XORSTR("resource/flash/econ/default_generated/") + std::string(weaponids.m_list[inventory_definition_index]->get_weapon_name()) + "_" + std::string(inventory_paint_kit == 542 ? XORSTR("cu_bizon_curse") : config.inventory_skin_info[inventory_paint_kit]->get_skin_name()) + XORSTR("_light_large.png")).c_str(), "r", "GAME");
								}
								else if (get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_gloves && !inventory_paint_kit || get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_music_kit && !inventory_paint_kit)
									entry = g_base_file_sys->open("", "r", "GAME");
								else
									entry = g_base_file_sys->open(std::string(XORSTR("resource/flash/") + std::string(weaponids.m_list[inventory_definition_index]->get_inventory_image()) + ".png").c_str(), "r", "GAME");

								if (entry) {
									int file_len = g_base_file_sys->size(entry);
									char* image = new char[file_len];

									g_base_file_sys->read(image, file_len, entry);
									g_base_file_sys->close(entry);

									D3DXCreateTextureFromFileInMemory(g_device, image, file_len, &m_skin_texture);

									delete[] image;
								}
							}

							if (m_skin_texture)
								window->DrawList->AddImage(m_skin_texture, { window->Pos.x + size_mini_child_x / 2.f - 76.f, window->Pos.y }, { window->Pos.x + size_mini_child_x / 2.f + 76.f, window->Pos.y + 112.f });
						}
						ImGui::EndChild();
						ImGui::PushStyleColor(ImGuiCol_ChildBg, g.Style.Colors[ImGuiCol_WindowBg]);
						ImGui::BeginChild("List", { size_mini_child_x, 296.f }, false); {
							ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
								if (inventory_other == 0) {
									if (get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_music_kit) {
										auto last_id = ImGui::GetItemID();
										for (const auto& kit : config.inventory_music_kits_info) {
											ImGui::PushID(last_id++);
											const auto item_selected = inventory_paint_kit == kit.first;
											if (ImGui::ListButton(utils::localize_text(kit.second->get_hud_name()).c_str(), { size_mini_child_x, 42.6f }, item_selected)) {
												item_selected;
												inventory_paint_kit = kit.first;
											}
											ImGui::PopID();
										}
									}
									else {
										auto last_id = ImGui::GetItemID();
										for (const auto& item : config.inventory_weapon_skins[inventory_definition_index]) {
											const auto item_selected = inventory_paint_kit == item->get_paint_kit();
											ImGui::PushID(++last_id);
											if (ImGui::ListButton(utils::localize_text(item->get_hud_name()).c_str(), { (inventory_definition_index == WEAPON_GLOVES_HYDRA || inventory_definition_index == WEAPON_GLOVES_BLOODHOUND) ? size_mini_child_x - g.Style.ScrollbarSize : size_mini_child_x, 42.6f }, item_selected)) {
												item_selected;
												inventory_paint_kit = item->get_paint_kit();
												for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
												inventory_sticker_slot = inventory_seed = inventory_stattrak = 0;
												inventory_wear = 0.f;
												filter_sticker.Clear();
											}
											ImGui::PopID();
										}
									}
								}
								else if (inventory_other == 1) {
									for (const auto& weapon : weaponids.m_types[type_collectible]) {

										if (weapon->get_definition_index() >= 6034 || weapon->get_definition_index() == 1353)
											continue;

										ImGui::PushID((void*)(intptr_t)weapon->get_definition_index());
										const bool item_selected = (weapon->get_definition_index() == inventory_definition_index);
										if (ImGui::ListButton(utils::localize_text(weapon->get_item_base_name()).c_str(), { size_mini_child_x, 42.6f }, item_selected)) {
											item_selected;
											inventory_definition_index = weapon->get_definition_index();
											for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
											inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = 0;
											inventory_wear = 0.f;
											filter_sticker.Clear();
										}
										ImGui::PopID();
									}
								}
								else if (inventory_other == 2) {
									for (const auto& weapon : weaponids.m_types[type_custom_player]) {

										if (weapon->get_definition_index() >= 5036 && weapon->get_definition_index() <= 5104 || weapon->get_definition_index() >= 5200 && weapon->get_definition_index() <= 5204  ||
											weapon->get_definition_index() >= 5300 && weapon->get_definition_index() <= 5304 || weapon->get_definition_index() == 5600)
											continue;

										ImGui::PushID((void*)(intptr_t)weapon->get_definition_index());
										const bool item_selected = (weapon->get_definition_index() == inventory_definition_index);
										if (ImGui::ListButton(utils::localize_text(weapon->get_item_base_name()).c_str(), { size_mini_child_x, 42.6f }, item_selected)) {
											item_selected;
											inventory_definition_index = weapon->get_definition_index();
											for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
											inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = 0;
											inventory_wear = 0.f;
											filter_sticker.Clear();
										}
										ImGui::PopID();
									}
								}
								else if (inventory_other == 3) {
									for (const auto& weapon : weaponids.m_types[type_case]) {

										if (weapon->get_definition_index() == 4006 || weapon->get_definition_index() == 4007 || weapon->get_definition_index() == 4012 ||
											weapon->get_definition_index() >= 4014 && weapon->get_definition_index() <= 4016 || 
											weapon->get_definition_index() == 4020 || weapon->get_definition_index() == 4021 ||
											weapon->get_definition_index() == 4030 || weapon->get_definition_index() >= 4038 && weapon->get_definition_index() <= 4060 || 
											weapon->get_definition_index() >= 4062 && weapon->get_definition_index() <= 4078 || 
											weapon->get_definition_index() >= 4086 && weapon->get_definition_index() <= 4088 || 
											weapon->get_definition_index() == 4090 || weapon->get_definition_index() >= 4092 && weapon->get_definition_index() <= 4130 || 
											weapon->get_definition_index() >= 4139 && weapon->get_definition_index() <= 4175 || 
											weapon->get_definition_index() >= 4183 && weapon->get_definition_index() <= 4185 || 
											weapon->get_definition_index() >= 4188 && weapon->get_definition_index() <= 4224 || 
											weapon->get_definition_index() >= 4234 && weapon->get_definition_index() <= 4235 || 
											weapon->get_definition_index() >= 4237 && weapon->get_definition_index() <= 4273 || 
											weapon->get_definition_index() >= 4282 && weapon->get_definition_index() <= 4287 || 
											weapon->get_definition_index() >= 4289 && weapon->get_definition_index() <= 4343 || 
											weapon->get_definition_index() >= 4357 && weapon->get_definition_index() <= 4395 || 
											weapon->get_definition_index() >= 4404 && weapon->get_definition_index() <= 4462 || weapon->get_definition_index() == 4470 || 
											weapon->get_definition_index() >= 4472 && weapon->get_definition_index() <= 4481 || 
											weapon->get_definition_index() >= 4483 && weapon->get_definition_index() <= 4539 || 
											weapon->get_definition_index() == 4547 || weapon->get_definition_index() >= 4559 && weapon->get_definition_index() <= 4589 || 
											weapon->get_definition_index() == 4597 || weapon->get_definition_index() >= 4599 && weapon->get_definition_index() <= 4618 || 
											weapon->get_definition_index() == 4621 || weapon->get_definition_index() >= 4629 && weapon->get_definition_index() <= 4659 || 
											weapon->get_definition_index() >= 4667 && weapon->get_definition_index() <= 4668 || weapon->get_definition_index() == 4670 || 
											weapon->get_definition_index() >= 4693 && weapon->get_definition_index() <= 4694 || 
											weapon->get_definition_index() >= 4696 && weapon->get_definition_index() <= 4697 || 
											weapon->get_definition_index() >= 6404 && weapon->get_definition_index() <= 6407 || 
											weapon->get_definition_index() >= 20000 && weapon->get_definition_index() <= 20170)
											continue;

										ImGui::PushID((void*)(intptr_t)weapon->get_definition_index());
										const bool item_selected = (weapon->get_definition_index() == inventory_definition_index);
										if (ImGui::ListButton(utils::localize_text(weapon->get_item_base_name()).c_str(), { size_mini_child_x, 42.6f }, item_selected)) {
											item_selected;
											inventory_definition_index = weapon->get_definition_index();
											for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
											inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = 0;
											inventory_wear = 0.f;
											filter_sticker.Clear();
										}
										ImGui::PopID();
									}
								}
								else if (inventory_other == 4) {
									for (const auto& weapon : weaponids.m_types[type_key]) {

										if (weapon->get_definition_index() == 1212 || weapon->get_definition_index() == 1304 ||
											weapon->get_definition_index() == 1305 || weapon->get_definition_index() == 1308 ||
											weapon->get_definition_index() >= 1309 && weapon->get_definition_index() <= 1311)
											continue;

										ImGui::PushID((void*)(intptr_t)weapon->get_definition_index());
										const bool item_selected = (weapon->get_definition_index() == inventory_definition_index);
										if (ImGui::ListButton(utils::localize_text(weapon->get_item_base_name()).c_str(), { size_mini_child_x, 42.6f }, item_selected)) {
											item_selected;
											inventory_definition_index = weapon->get_definition_index();
											for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
											inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = 0;
											inventory_wear = 0.f;
											filter_sticker.Clear();
										}
										ImGui::PopID();
									}
								}
							}
							ImGui::PopStyleVar();
						}
						ImGui::EndChild();
						ImGui::PopStyleColor();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x * 2 + size_spacing * 2);
					ImGui::BeginGroup(); {
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, ImGui::ColConvertToFloat(0.f, 0.f, 0.f, 0.f));
						ImGui::BeginChild("Pages", { size_mini_child_x, 30.f }, true); {
							ImGuiWindow* window = ImGui::GetCurrentWindow();
							ImVec2 size{ size_mini_child_x / 2.f, 30.f };
							ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f)); {
								if (ImGui::ButtonTabs("Page 1", size, skins_page == 0))
									skins_page = 0;
								ImGui::SameLine();
								if (ImGui::ButtonTabs("Page 2", size, skins_page == 1))
									skins_page = 1;
							}
							ImGui::PopStyleVar();

							static float current_sub_selected_pos = 0.f;
							static float new_sub_selected_pos = 0.f;

							if (current_sub_selected_pos < new_sub_selected_pos) {
								current_sub_selected_pos += 2000.f * ImGui::GetIO().DeltaTime;
								if (current_sub_selected_pos >= new_sub_selected_pos)
									current_sub_selected_pos = new_sub_selected_pos;
							}

							else if (current_sub_selected_pos > new_sub_selected_pos) {
								current_sub_selected_pos -= 2000.f * ImGui::GetIO().DeltaTime;
								if (current_sub_selected_pos <= new_sub_selected_pos)
									current_sub_selected_pos = new_sub_selected_pos;
							}

							new_sub_selected_pos = size.x * float(skins_page);

							window->DrawList->PushClipRectFullScreen();
							window->DrawList->AddRectFilled({ window->Pos.x + current_sub_selected_pos,  window->Pos.y }, { window->Pos.x + current_sub_selected_pos + size.x,  window->Pos.y + size.y }, ImGui::GetColorU32(ImGui::ColConvertToFloat(34.f, 52.f, 76.f)));

							const char* char_skins[] = { get_language("SETTINGS", "НАСТРОЙКИ"), get_language("STICKERS", "НАКЛЕЙКИ") };

							for (size_t i = 0; i < IM_ARRAYSIZE(char_skins); i++) {
								const auto text_size = ImGui::CalcTextSize(char_skins[i]);
								window->DrawList->AddText(ImVec2(window->Pos.x + (size.x * i) + (((window->Pos.x + size.x) - window->Pos.x) / 2.f) - (text_size.x / 2.f), window->Pos.y + 8.5f), ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)), char_skins[i]);
							}
							window->DrawList->PopClipRect();
						}
						ImGui::EndChild();
						ImGui::PopStyleColor();
						ImGui::BeginChild("Settings", { size_mini_child_x, 226.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::PushItemWidth(item_width_mini_child);
							if (skins_page == 0) {
								ImGui::Spacing();
								ImGui::InputInt(get_language("Seed", "Паттерн"), &inventory_seed);
								ImGui::InputInt(get_language("StatTrak", "Счетчик Убийств"), &inventory_stattrak);
								ImGui::SliderFloat(get_language("WEAR", "ИЗНОС"), &inventory_wear, 0.f, 1.f);
							}
							else if (skins_page == 1) {	
								const auto type = get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name());
								if ((type == type_pistol || type == type_smg || type == type_rifle || type == type_sniper || type == type_shotgun || type == type_machinegun) && inventory_paint_kit) {
									if (ImGui::ButtonSubtabs("0", ImVec2(56.5f, g.Style.ComboButtonSize.y), inventory_sticker_slot == 0)) {
										inventory_sticker_slot = 0;
										filter_sticker.Clear();
									}
									ImGui::SameLine();
									if (ImGui::ButtonSubtabs("1", ImVec2(56.5f, g.Style.ComboButtonSize.y), inventory_sticker_slot == 1)) {
										inventory_sticker_slot = 1;
										filter_sticker.Clear();
									}
									ImGui::SameLine();
									if (ImGui::ButtonSubtabs("2", ImVec2(56.5f, g.Style.ComboButtonSize.y), inventory_sticker_slot == 2)) {
										inventory_sticker_slot = 2;
										filter_sticker.Clear();
									}
									ImGui::SameLine();
									if (ImGui::ButtonSubtabs("3", ImVec2(56.5f, g.Style.ComboButtonSize.y), inventory_sticker_slot == 3)) {
										inventory_sticker_slot = 3;
										filter_sticker.Clear();
									}
									for (int i = 0; i < 4; i++) {
										if (default_sticker[i] != inventory_stickers[i].kit) {
											if (m_sticker_texture[i]) {
												m_sticker_texture[i]->Release();
												m_sticker_texture[i] = nullptr;
											}
										}
										default_sticker[i] = inventory_stickers[i].kit;

										if (inventory_definition_index && !m_sticker_texture[i]) {
											auto entry = g_base_file_sys->open(std::string(XORSTR("resource/flash/") + std::string(config.inventory_sticker_image[inventory_stickers[i].kit].c_str()) + ".png").c_str(), "r", "GAME");

											if (entry) {
												int file_len = g_base_file_sys->size(entry);
												char* image = new char[file_len];

												g_base_file_sys->read(image, file_len, entry);
												g_base_file_sys->close(entry);

												D3DXCreateTextureFromFileInMemory(g_device, image, file_len, &m_sticker_texture[i]);

												delete[] image;
											}
										}

										ImGuiWindow* window = ImGui::GetCurrentWindow();
										window->DrawList->PushClipRectFullScreen();
										if (m_sticker_texture[i])
											window->DrawList->AddImage(m_sticker_texture[i], { window->Pos.x + 25.75f + float(64.5f * i), window->Pos.y + 14.f }, { window->Pos.x + 50.75f + float(64.5f * i), window->Pos.y + 34.f });
										window->DrawList->PopClipRect();
									}

									ImGui::Spacing();
									if (ImGui::BeginCombo(get_language("Sticker", "Наклейка"), config.inventory_sticker_info[inventory_stickers[inventory_sticker_slot].kit].c_str())) {
										auto last_id = ImGui::GetItemID();
										ImGui::SameLine(10.f);
										ImGui::BeginGroup();
										ImGui::Spacing();
										filter_sticker.Draw("Search", 230.f);
										ImGui::BeginChild("Stickers Child", ImVec2(230.f, g.Style.ComboButtonSize.y * 3.f), false, ImGuiWindowFlags_::ImGuiWindowFlags_NoChildShadow); {
											for (const auto & sticker : config.inventory_sticker_info) {
												ImGui::PushID(last_id++);
												if (filter_sticker.PassFilter(sticker.second.c_str())) {
													const auto item_selected = inventory_stickers[inventory_sticker_slot].kit == sticker.first;
													ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
														if (ImGui::ComboButton(sticker.second.c_str(), ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y), item_selected)) {
															item_selected;
															inventory_stickers[inventory_sticker_slot].kit = sticker.first;
															ImGui::CloseCurrentPopup();
														}
													}
													ImGui::PopStyleVar();
												}
												ImGui::PopID();
											}
										}
										ImGui::EndChild();
										ImGui::EndGroup();
										ImGui::EndCombo();
									}
									ImGui::SliderFloat(get_language("WEAR##Sticker", "ИЗНОС##Sticker"), &inventory_stickers[inventory_sticker_slot].wear, 0.000f, 1.000f);
									ImGui::SliderFloat(get_language("SCALE##Sticker", "МАСШТАБ##Sticker"), &inventory_stickers[inventory_sticker_slot].scale, 1.0f, 5.0f);
									ImGui::SliderFloat(get_language("ROTATION##Sticker", "ВРАЩЕНИЕ##Sticker"), &inventory_stickers[inventory_sticker_slot].rotation, 0.f, 360.f);
								}
								else {
									ImGui::Text(get_language("You cannot add a sticker to this item", "Нельзя добавить наклейку на этот предмет"));
								}
							}
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
						ImGui::BeginChild("Item Remove List", { size_mini_child_x, 150.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::BeginChild("Remove List", ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y * 3.f), false, ImGuiWindowFlags_::ImGuiWindowFlags_NoChildShadow); {
								ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
									ImGui::PushFont(roboto_bold_elements);
									for (auto skins = config.inventory_item_list.begin(); skins != config.inventory_item_list.end();) {
										const auto item_name = std::string(utils::localize_text(weaponids.m_list[skins->second.definition_index]->get_item_base_name()) + (skins->second.paint_kit ? " | " + (get_hash(weaponids.m_list[skins->second.definition_index]->get_item_type_name()) == type_music_kit ? utils::localize_text(config.inventory_music_kits_info[skins->second.paint_kit]->get_hud_name()) : utils::localize_text(config.inventory_skin_info[skins->second.paint_kit]->get_hud_name())) : ""));
										if (ImGui::ComboButton(item_name.c_str(), ImVec2(item_width_mini_child, g.Style.ComboButtonSize.y))) {
											g_player_inventory->RemoveItem(g_player_inventory->GetInventoryItemByItemID(skins->first)->GetSOCData());
											skins = config.inventory_item_list.erase(skins);
										}
										else
											++skins;
									}
									ImGui::PopFont();
								}
								ImGui::PopStyleVar();
							}
							ImGui::EndChild();
							if (ImGui::Button(get_language("Add", "Добавить"), ImVec2(item_width_mini_child / 2 - 4, 30))) {
								if (inventory_paint_kit || get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_knife || get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_collectible || 
									get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_custom_player || get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_case || 
									get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_key) {
									auto item = g_player_inventory->CreateEconItem();
									*item->GetAccountID() = (uint32_t)g_player_inventory->get_owner_id().id;
									*item->GetDefinitionIndex() = inventory_definition_index;
									*item->GetItemID() = g_player_inventory->GetHighestItemID() + unique_id++;
									*item->GetInventory() = 0;
									if (get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) != type_music_kit)
										*item->GetFlags() = 0;
									*item->GetOriginalID() = 0;

									for (std::size_t i = 0; i < inventory_stickers.size(); ++i) {
										const auto& sticker = inventory_stickers[i];

										if (sticker.kit == 0)
											continue;

										item->AddSticker(i, sticker.kit, sticker.wear, sticker.scale, sticker.rotation);
									}
								
									if (get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) != type_music_kit) {
										item->SetPaintKit(inventory_paint_kit);
										item->SetPaintSeed(inventory_seed);
										item->SetPaintWear(inventory_wear);
									}
									item->SetOrigin(8);
									if (inventory_paint_kit) {
										if (get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_music_kit)
											item->SetRarity(3);
										else
											item->SetRarity(changer::get_rarity(weaponids.m_list[inventory_definition_index]->get_rarity(), config.inventory_skin_info[inventory_paint_kit]->get_rarity_value()));
									}
									else if (get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_collectible || get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_custom_player ||
										get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_case || get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_key)
										item->SetRarity(changer::get_rarity(weaponids.m_list[inventory_definition_index]->get_rarity(), 1));
									else
										item->SetRarity(6);

									if ((get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_pistol || get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_smg ||
										get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_rifle || get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_sniper ||
										get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_shotgun || get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_machinegun ||
										get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_knife || get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_music_kit))
										if (inventory_stattrak > 0)
											item->SetStatTrak(inventory_stattrak);

									item->SetLevel(1);
									item->SetInUse(false);

									if (get_hash(weaponids.m_list[*item->GetDefinitionIndex()]->get_item_type_name()) == type_knife || get_hash(weaponids.m_list[*item->GetDefinitionIndex()]->get_item_type_name()) == type_gloves)
										item->SetQuality(3);

									if (get_hash(weaponids.m_list[inventory_definition_index]->get_item_type_name()) == type_music_kit)
										item->SetAttributeValue<int>(166, inventory_paint_kit);

									g_player_inventory->get_item_base_type_cache()->add_object(item);
									g_player_inventory->so_created(g_player_inventory->get_owner_id(), (c_shared_object*)item, 4);

									const auto item_view = g_player_inventory->GetInventoryItemByItemID(*item->GetItemID());
									item_view->ClearInventoryImageRgba();
									config.inventory_item_list.insert({ *item->GetItemID(), { inventory_definition_index, inventory_paint_kit, inventory_seed, inventory_stattrak, inventory_wear, { inventory_stickers[0], inventory_stickers[1], inventory_stickers[2], inventory_stickers[3], inventory_stickers[4]} } });
								}
								for (auto i = 0; i < 4; i++) { inventory_stickers[i].kit = 0; inventory_stickers[i].wear = 0.f; inventory_stickers[i].rotation = 0.f; inventory_stickers[i].scale = 1.f; }
								inventory_definition_index = 1;
								inventory_paint_kit = inventory_sticker_slot = inventory_seed = inventory_stattrak = inventory_other = 0;
								inventory_wear = 0.f;
								filter_sticker.Clear();
							}
							ImGui::SameLine();
							if (ImGui::Button(get_language("Remove All", "Удалить Все"), ImVec2(item_width_mini_child / 2 - 4, 30))) {
								for (auto skins = config.inventory_item_list.begin(); skins != config.inventory_item_list.end();) {
									g_player_inventory->RemoveItem(g_player_inventory->GetInventoryItemByItemID(skins->first)->GetSOCData());
									skins = config.inventory_item_list.erase(skins);
								}
							}
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					break;
				}
				case 7: {
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Profile", { size_mini_child_x, 426.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							const char* dz_list[] = {
								get_language("Off",	"Выкл"),
								get_language("Lab Rat 1", "Лабораторная крыса 1"),
								get_language("Lab Rat 2", "Лабораторная крыса 2"),
								get_language("Sprinting Hare 1", "Бегущий заяц 1"),
								get_language("Sprinting Hare 2", "Бегущий заяц 2"),
								get_language("Wild Scout 1", "Дикий разведчик 1"),
								get_language("Wild Scout 2", "Дикий разведчик 2"),
								get_language("Wild Scout Elite", "Дикий разведчик Элита"),
								get_language("Hunter Fox 1", "Лис-охотник 1"),
								get_language("Hunter Fox 2", "Лис-охотник 2"),
								get_language("Hunter Fox 3", "Лис-охотник 3"),
								get_language("Hunter Fox Elite", "Лис-охотник Элита"),
								get_language("Timber Wolf", "Серый волк"),
								get_language("Ember Wolf", "Тлеющий волк"),
								get_language("Wildfire Wolf", "Пламенный волк"),
								get_language("The Howling Alpha", "Пламенный волк")
							};

							const char* rank_list[] = {
								get_language("Off",	"Выкл"),
								get_language("Silver I", "1 Сильвер"),
								get_language("Silver II", "2 Сильвер"),
								get_language("Silver III", "3 Сильвер"),
								get_language("Silver IV", "4 Сильвер"),
								get_language("Silver Elite", "5 Сильвер"),
								get_language("Silver Elite Master", "6 Сильвер"),
								get_language("Gold Nova I", "1 Звезда"),
								get_language("Gold Nova II", "2 Звезды"),
								get_language("Gold Nova III", "3 Звезды"),
								get_language("Gold Nova Master", "4 Звезды"),
								get_language("Master Guardian I", "Калаш"),
								get_language("Master Guardian II", "Калаш с Венками"),
								get_language("Master Guardian Elite", "2 Калаша"),
								get_language("Distinguished Master Guardian", "Биг Стар"),
								get_language("Legendary Eagle", "Беркут"),
								get_language("Legendary Eagle Master", "Лем"),
								get_language("Supreme Master First Class", "Суприм"),
								get_language("Global Elite", "Глобал")
							};

							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::Spacing();
							ImGui::Combo(get_language("Rank", "Ранк"), &config.profile_rank_id, rank_list, ARRAYSIZE(rank_list));
							ImGui::InputInt(get_language("Wins", "Победы"), &config.profile_wins);
							ImGui::Spacing();
							ImGui::Combo(get_language("Wingman Rank", "Ранк В Напарниках"), &config.profile_rank_id_wingmans, rank_list, ARRAYSIZE(rank_list));
							ImGui::InputInt(get_language("Wingman Wins", "Победы В Напарниках"), &config.profile_wins_wingmans);
							ImGui::Spacing();
							ImGui::Combo(get_language("Zone Rank", "Ранк В Запретной Зоне"), &config.profile_rank_id_dz, dz_list, ARRAYSIZE(dz_list));
							ImGui::InputInt(get_language("Zone Wins", "Победы В Запретной Зоне"), &config.profile_wins_dz);
							ImGui::Spacing();
							ImGui::Combo(get_language("Ban", "Блокировка"), &config.profile_player_ban, get_language("None\0\rYellow\0\rRed\0\rGreen\0\0", "Нет\0\rЖелтая\0\rКрасная\0\rЗеленая\0\0"));
							ImGui::InputInt(get_language("Ban Time", "Время Блокировка"), &config.profile_player_ban_time);
							ImGui::PopItemWidth();
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x + size_spacing);
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Profile 2", { size_mini_child_x, 426.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::SliderInt(get_language("LEVEL", "УРОВЕНЬ"), &config.profile_player_level, 0, 40);
							ImGui::SliderInt(get_language("XP", "ОПЫТ"), &config.profile_player_level_xp, 0, 5000);
							ImGui::Spacing();
							ImGui::InputInt(get_language("Teacher", "Учитель"), &config.profile_teaching);
							ImGui::InputInt(get_language("Friendly", "Дружелюбный"), &config.profile_friendly);
							ImGui::InputInt(get_language("Leader", "Лидер"), &config.profile_leader);
							ImGui::PopItemWidth();

							static auto get_update = [] {
								utils::send_matchmaking_client_hello();
								utils::send_client_rank_update_wingman(); 
								std::this_thread::sleep_for(std::chrono::milliseconds(700));
								utils::send_client_rank_update_zone();
							};

							if (ImGui::Button(get_language("Update All", "Обновить Все"), { item_width_mini_child, 35.f })) {
								std::thread apply(get_update);
								apply.detach();
							}

							ImGui::Spacing();

							if (ImGui::Button(get_language("Unlock All Achievements", "Разблокировать Все Достижения"), { item_width_mini_child, 35.f })) {
								FILE* fp;
								int32_t err;
								err = _wfopen_s(&fp, XORWSTR(L"csgo_achievement_unlocker.txt"), XORWSTR(L"a+"));
								if (err == 0 && fp != nullptr) {
									fprintf(fp, XORSTR("[csgo_achievement_unlocker]\n\n"));
									misc::unlock_all_achievements(fp);
									fprintf(fp, "\n");
									fclose(fp);
								}
							}
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x * 2 + size_spacing * 2);
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Lobby", { size_mini_child_x, 426.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {							ImGui::Checkbox(get_language("Popups Spam", u8"Спам Окнами"), &config.lobby_popups_spam);
						ImGui::Checkbox(get_language("Error Spam", u8"Спам Ошибками"), &config.lobby_error_spam);
						ImGui::Checkbox(get_language("Lobby Invitations", u8"Лобби Приглашения"), &config.lobby_invitations);

						if (ImGui::Button(get_language("Restore Last Match", u8"Переподключиться К Матчу"), { item_width_mini_child, 35.f }))
							misc::reconnect_to_on_going_match();

						if (ImGui::Button(get_language("Stop Matchmaking", u8"Остановить Поиск"), { item_width_mini_child, 35.f }))
							lobby::stop_matchmaking();
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					break;
				}
				case 8: {
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Main", { size_mini_child_x, 426.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("Rank Reveal", "Показ Званий"), &config.misc_rank_reveal);
							ImGui::Checkbox(get_language("Money Reveal", "Показ Денег"), &config.misc_money_reveal);
							ImGui::Checkbox(get_language("Overwatch Reveal", "Показ Информации В Патруле"), &config.misc_overwatch_reveal);
							ImGui::Checkbox(get_language("Auto Defuse", "Авто Обезвреживание бомбы"), &config.misc_auto_defuse);
							ImGui::Checkbox(get_language("Auto Accept", "Авто Принятие"), &config.misc_auto_accept);
							ImGui::Checkbox(get_language("Inventory Always", "Инвентарь В Игре"), &config.misc_inventory_always);
							ImGui::Checkbox(get_language("Name Stealer", "Стилер Имен"), &config.misc_name_stealer);
							ImGui::Checkbox(get_language("Knifebot", "Ножевой Бот"), &config.misc_knife_bot);
							ImGui::Checkbox(get_language("Spectators", "Наблюдатели"), &config.misc_spectators);
							ImGui::Checkbox(get_language("In Game Radar", "Внутриигровой Радар"), &config.misc_radar);
							ImGui::Checkbox(get_language("Thunder", "Гром"), &config.misc_thunder_enable);
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x + size_spacing);
					ImGui::BeginGroup(); {
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, ImGui::ColConvertToFloat(0.f, 0.f, 0.f, 0.f));
						ImGui::BeginChild("Groups", { size_big_child_x, 50.f }, true); {
							ImGuiWindow* window = ImGui::GetCurrentWindow();
							ImVec2 size{ size_big_child_x / 3.f, 50.f };
							ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f)); {
								if (ImGui::ButtonTabs("Page 1", size, misc_page == 0))
									misc_page = 0;
								ImGui::SameLine();
								if (ImGui::ButtonTabs("Page 2", size, misc_page == 1))
									misc_page = 1;
								ImGui::SameLine();
								if (ImGui::ButtonTabs("Page 3", size, misc_page == 2))
									misc_page = 2;
							}
							ImGui::PopStyleVar();

							static float current_sub_selected_pos = 0.f;
							static float new_sub_selected_pos = 0.f;

							if (current_sub_selected_pos < new_sub_selected_pos) {
								current_sub_selected_pos += 2000.f * ImGui::GetIO().DeltaTime;
								if (current_sub_selected_pos >= new_sub_selected_pos)
									current_sub_selected_pos = new_sub_selected_pos;
							}

							else if (current_sub_selected_pos > new_sub_selected_pos) {
								current_sub_selected_pos -= 2000.f * ImGui::GetIO().DeltaTime;
								if (current_sub_selected_pos <= new_sub_selected_pos)
									current_sub_selected_pos = new_sub_selected_pos;
							}

							new_sub_selected_pos = size.x * float(misc_page);

							window->DrawList->PushClipRectFullScreen();
							window->DrawList->AddRectFilled({ window->Pos.x + current_sub_selected_pos,  window->Pos.y }, { window->Pos.x + current_sub_selected_pos + size.x,  window->Pos.y + size.y }, ImGui::GetColorU32(ImGui::ColConvertToFloat(34.f, 52.f, 76.f)));
							window->DrawList->AddImage(settings_misc_image, { window->Pos.x + (size_big_child_x / 3.f) * 0.5f - 10.f, window->Pos.y + 15.f }, { window->Pos.x + (size_big_child_x / 3.f) * 0.5f + 10.f, window->Pos.y + 35.f });
							window->DrawList->AddImage(modulation_misc_image, { window->Pos.x + size_big_child_x * 0.5f - 10.f, window->Pos.y + 15.f }, { window->Pos.x + size_big_child_x * 0.5f + 10.f, window->Pos.y + 35.f });
							window->DrawList->AddImage(king_misc_image, { window->Pos.x + size_big_child_x - 10.f - (size_big_child_x / 3.f) * 0.5f, window->Pos.y + 15.f }, { window->Pos.x + size_big_child_x + 10.f - (size_big_child_x / 3.f) * 0.5f, window->Pos.y + 35.f });
							window->DrawList->PopClipRect();
						}
						ImGui::EndChild();
						ImGui::PopStyleColor();
						if (misc_page == 0) {
							ImGui::BeginGroup(); {
								ImGui::BeginChild("Removal", { size_mini_child_x, 138.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
									ImGui::Checkbox(get_language("Remove Smoke", "Убрать Дым"), &config.misc_no_smoke);
									ImGui::Checkbox(get_language("Remove Flash Effect", "Убрать Ослепление"), &config.misc_no_flash_effect);
									ImGui::Checkbox(get_language("Remove Scope Border", "Убрать Границы Прицела"), &config.misc_no_scope_border);
									ImGui::Checkbox(get_language("Remove Post Processing", "Убрать Постобработку"), &config.misc_disable_post_processing);
								}
								ImGui::EndChild();
								ImGui::BeginChild("Fog Changer", { size_mini_child_x, 218.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
									ImGui::Checkbox(get_language("Enable Fog Changer", "Включить Настройку Тумана"), &config.misc_fog_enable);
									ImGui::SameLine(200.f);
									ImGui::ColorEdit3(get_language("Color Fog", "Цвет Тумана"), config.color_fog, color_edit_flags);
									ImGui::Checkbox(get_language("Blend", "Смесь"), &config.misc_fog_blend);
									ImGui::PushItemWidth(item_width_mini_child);
									ImGui::SliderFloat(get_language("START##misc_fog_start", "НАЧАЛО##misc_fog_end"), &config.misc_fog_start, 0.0f, 3000.0f, "%.2f");
									ImGui::SliderFloat(get_language("END##misc_fog_end", "КОНЕЦ##misc_fog_end"), &config.misc_fog_end, 0.0f, 3000.0f, "%.2f");
									ImGui::SliderFloat(get_language("VALUE##misc_fog_density", "ЗНАЧЕНИЕ##misc_fog_density"), &config.misc_fog_density, 0.0f, 1.0f, "%.2f");
									ImGui::PopItemWidth();
								}
								ImGui::EndChild();
							}
							ImGui::EndGroup();
							ImGui::SameLine(size_mini_child_x + size_spacing);
							ImGui::BeginGroup(); {
								ImGui::BeginChild("FOV Changer", { size_mini_child_x, 366.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
									ImGui::Checkbox(get_language("Enable FOV Changer", "Включить Настройку ФОВА"), &config.misc_fov_changer);
									ImGui::PushItemWidth(item_width_mini_child);
									ImGui::SliderFloat(get_language("VALUE VIEWMODEL FOV", "ЗНАЧЕНИЕ ФОВ"), &config.misc_fov_viewmodel, 1.0f, 120.0f, "%.2f");
									ImGui::SliderFloat(get_language("VALUE OVERRIDE", "ЗНАЧЕНИЕ МИРА"), &config.misc_fov_override, 1.0f, 120.0f, "%.2f");
									ImGui::Checkbox(get_language("Enable Offset", "Включить Смещение"), &config.misc_fov_offset);
									ImGui::SliderFloat(get_language("VALUE VIEWMODEL X", "ЗНАЧЕНИЕ X"), &config.misc_fov_viewmodel_x, -20.0f, 20.0f, "%.2f");
									ImGui::SliderFloat(get_language("VALUE VIEWMODEL Y", "ЗНАЧЕНИЕ Y"), &config.misc_fov_viewmodel_y, -20.0f, 20.0f, "%.2f");
									ImGui::SliderFloat(get_language("VALUE VIEWMODEL Z", "ЗНАЧЕНИЕ Z"), &config.misc_fov_viewmodel_z, -20.0f, 20.0f, "%.2f");
									ImGui::PopItemWidth();
								}
								ImGui::EndChild();
							}
							ImGui::EndGroup();
						}
						else if (misc_page == 1) {
							ImGui::BeginGroup(); {
								ImGui::BeginChild("Ragdoll", { size_mini_child_x, 89.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
									ImGui::Checkbox(get_language("Enable Ragdoll", "Включить Рэгдолл"), &config.misc_ragdoll);
									ImGui::PushItemWidth(item_width_mini_child);
									ImGui::SliderInt(get_language("VALUE##misc_ragdoll_value", "ЗНАЧЕНИЕ##misc_ragdoll_value"), &config.misc_ragdoll_value, 0, 1000);
									ImGui::PopItemWidth();
								}
								ImGui::EndChild();
								ImGui::BeginChild("Aspect Ratio", { size_mini_child_x, 131.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
									ImGui::Checkbox(get_language("Enable Aspect Ratio", "Включить Соотношение Сторон"), &config.misc_aspect_ratio);
									ImGui::PushItemWidth(item_width_mini_child);
									ImGui::SliderFloat(get_language("VALUE##misc_aspect_ratio_value", "ЗНАЧЕНИЕ##misc_aspect_ratio_value"), &config.misc_aspect_ratio_value, 1.0f, 5.0f, "%.2f");
									ImGui::PopItemWidth();							
								}
								ImGui::EndChild();
								ImGui::BeginChild("Fake Latency", { size_mini_child_x, 126.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
									ImGui::Checkbox(get_language("Enable Fake Latency", "Включить Ложный Пинг"), &config.misc_fake_latency);
									ImGui::PushItemWidth(item_width_mini_child);
									ImGui::SliderInt(get_language("VALUE##misc_fake_latency_value", "ЗНАЧЕНИЕ##misc_fake_latency_value"), &config.misc_fake_latency_value, 1, 1000);
									ImGui::PopItemWidth();
								}
								ImGui::EndChild();
							}
							ImGui::EndGroup();
							ImGui::SameLine(size_mini_child_x + size_spacing);
							ImGui::BeginGroup(); {
								ImGui::BeginChild("Asus Walls", { size_mini_child_x, 89.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
									ImGui::Checkbox(get_language("Enable Asus Walls", "Включить Прозрачные Стены"), &config.misc_asus_walls_enable);
									ImGui::PushItemWidth(item_width_mini_child);
									ImGui::SliderFloat(get_language("VALUE##misc_asus_walls_value", "ЗНАЧЕНИЕ##misc_asus_walls_value"), &config.misc_asus_walls_value, 1.0f, 100.0f, "%.2f");
									ImGui::PopItemWidth();
								}
								ImGui::EndChild();
								ImGui::BeginChild("Custom World", { size_mini_child_x, 267.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
									ImGui::Checkbox(get_language("Night Mode", "Ночной Режим"), &config.misc_night_mode_enable);
									ImGui::PushItemWidth(item_width_mini_child);
									ImGui::SliderFloat(get_language("VALUE##misc_night_mode_value", "ЗНАЧЕНИЕ##misc_night_mode_value"), &config.misc_night_mode_value, 0.01f, 1.0f, "%.2f");
									ImGui::SliderFloat(get_language("BLOOM VALUE##misc_bloom_effect_value", "ЗНАЧЕНИЕ ЭФФЕКТА ЦВЕТЕНИЯ##misc_bloom_effect_value"), &config.misc_bloom_effect_value, 0.0f, 10.0f, "%.2f");
									ImGui::SliderFloat(get_language("BRIGHTNESS VALUE", "ЗНАЧЕНИЕ ЯРКОСТИ"), &config.misc_model_ambient_value, 0.0f, 10.0f, "%.2f");
									ImGui::PopItemWidth();
									ImGui::Checkbox(get_language("World Custom Color", "Кастомный Цвет Мира"), &config.misc_world_custom_color_enable);
									ImGui::SameLine(200.f);
									ImGui::ColorEdit3(get_language("Color World", "Цвет Мира"), config.color_misc_world, color_edit_flags);
									ImGui::SameLine(175.f);
									ImGui::ColorEdit3(get_language("Color Static Props", "Цвет Статичных Элементов"), config.color_misc_static_props, color_edit_flags);

								}
								ImGui::EndChild();
							}
							ImGui::EndGroup();
						}
						else if (misc_page == 2) {
							ImGui::BeginGroup(); {
								ImGui::BeginChild("Movement", { size_mini_child_x, 366.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
									ImGui::Checkbox(get_language("Bhop", "Распрыжка"), &config.misc_bhop);
									ImGui::Checkbox(get_language("Auto Strafe", "Авто Стрейф"), &config.misc_auto_strafe);
									ImGui::Checkbox(get_language("Edge Jump", "Прыжок На Краю"), &config.misc_edge_jump);
									ImGui::Checkbox(get_language("Mini Jump", "Минимальный Прыжок"), &config.misc_mini_jump);
									ImGui::Checkbox(get_language("Fast Ladder", "Быстрый Подъем По Лестнице"), &config.misc_fast_ladder);
								}
								ImGui::EndChild();
								/*ImGui::BeginChild("Desync", { size_mini_child_x, 90.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
									ImGui::Checkbox(get_language("Enable Desync", "Включить Десинхронизацию"), &config.legit_desync);
									ImGui::PushItemWidth(item_width_mini_child);
									ImGui::Hotkey(get_language("Key##legit_desync_key", "Кнопка##legit_desync_key"), &config.legit_desync_key);
									ImGui::PopItemWidth();
								}
								ImGui::EndChild();*/

							}
							ImGui::EndGroup();
							ImGui::SameLine(size_mini_child_x + size_spacing);
							ImGui::BeginGroup(); {
								ImGui::BeginChild("Clantag", { size_mini_child_x, 131.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
									ImGui::Checkbox(get_language("Enable Clantag Changer", "Включить Настройку Клантэга"), &config.misc_clan_tag);
									ImGui::PushItemWidth(item_width_mini_child);
									static char get_name[128] = "";
									config.misc_clan_tag_name = get_name;
									ImGui::Combo(get_language("Type##misc_clan_tag_type", "Тип##misc_clan_tag_type"), &config.misc_clan_tag_type, get_language("Static\0\rAnimated\0\rStatic Clear\0\rCustom\0\rCustom Animated\0\rValve\0\rValve Clear\0\rСlear\0\0", "Статичный\0\rАнимированный\0\rСтатичный Чистый\0\rКастомный\0\rКастомный Анимированный\0\rValve\0\rValve Чистый\0\rЧистый\0\0"));
									ImGui::InputText(get_language("Name##misc_clan_tag_name", "Название##misc_clan_tag_name"), get_name, 128);
									ImGui::PopItemWidth();
								}
								ImGui::EndChild();
								ImGui::BeginChild("Third Person", { size_mini_child_x, 225.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
									ImGui::PushItemWidth(item_width_mini_child);
									ImGui::Spacing();
									ImGui::Combo(get_language("Third Person Type Bind##misc_third_person_key_mode", "Тип Бинда##misc_third_person_key_mode"), &config.misc_third_person_key_mode, "Toggle\0\rHold\0\rForce Disable\0\rAlways on\0\0");
									ImGui::Hotkey(get_language("Third Person Key##misc_third_person_key", "Кнопка##misc_third_person_key"), &config.misc_third_person_key);
									ImGui::SliderInt(get_language("VALUE##misc_third_person_value", "ЗНАЧЕНИЕ##misc_third_person_value"), &config.misc_third_person_value, 50, 300);
									ImGui::PopItemWidth();

								}
								ImGui::EndChild();
							
							}
							ImGui::EndGroup();
						}
					}
					ImGui::EndGroup();
					break;
				}
				case 9: {
					static int config_index;
					static char config_name[128];
					static char config_rename[128];
					static char cfg_id[128];

					std::vector<std::string> files;
					config_manager::read_directory(XORSTR("C:/shark-hack/configs/"), files);

					ImGui::BeginGroup(); {
						ImGui::PushStyleColor(ImGuiCol_ChildBg, g.Style.Colors[ImGuiCol_WindowBg]);
						ImGui::BeginChild("Config List", { size_mini_child_x, 426.f }, files.size() > 10 ? false : true); {
							ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
								for (size_t i = 0; i < files.size(); i++) {
									if (ImGui::ListButton(files[i].c_str(), { size_mini_child_x, 42.6f }, config_index == i))
										config_index = i;
								}
							}
							ImGui::PopStyleVar();
						}
						ImGui::EndChild();
						ImGui::PopStyleColor();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x + size_spacing);
					ImGui::BeginGroup(); {
						ImGui::PushStyleColor(ImGuiCol_PopupBg, g.Style.Colors[ImGuiCol_WindowBg]);
						ImGui::BeginChild("Create", { size_mini_child_x, 164.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Spacing();
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::InputText(get_language("Name Config", "Название Конфига"), config_name, 128);
							ImGui::PopItemWidth();
							if (ImGui::Button(get_language("Create Config", "Создать Конфиг"), { item_width_mini_child, 35.f }))
								config_manager::save(std::string(config_name) + ".ini");
							if (ImGui::Button(get_language("Remove Config", "Удалить Конфиг"), { item_width_mini_child, 35.f }) && !files.empty())
								ImGui::OpenPopup(get_language("Remove?", "Удалить?"));

							ImGui::SetNextWindowPos({ ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f }, 0, ImVec2(0.5f, 0.5f));
							if (ImGui::BeginPopupModal(get_language("Remove?", "Удалить?"), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
								ImGui::Dummy({ 0.f, 30.f });
								if (ImGui::Button(get_language("Yes", "Да"), { 120.f, 35.f }) && !files.empty()) {
									remove(std::string(XORSTR("C:/shark-hack/configs/") + files.at(config_index)).c_str());
									ImGui::CloseCurrentPopup(); 
								}
								ImGui::SetItemDefaultFocus();
								ImGui::SameLine();
								if (ImGui::Button(get_language("Nope", "Нет"), { 120.f, 35.f }))
									ImGui::CloseCurrentPopup();
								ImGui::Dummy({ 0.f, 30.f });
								ImGui::EndPopup();
							}
						}
						ImGui::EndChild();
						ImGui::BeginChild("Load & Save", { size_mini_child_x, 121.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Spacing();
							if (ImGui::Button(get_language("Load Config", "Загрузить Конфиг"), { item_width_mini_child, 35.f }) && !files.empty()) {
								config_manager::load(files.at(config_index));
								ImGui::ResetAnimations();
								g_client_state->m_nDeltaTick = -1;
							}

							if (ImGui::Button(get_language("Save Config", "Сохранить Конфиг"), { item_width_mini_child, 35.f }) && !files.empty())
								ImGui::OpenPopup(get_language("Save?", "Сохранить?"));

							ImGui::SetNextWindowPos({ ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f }, 0, ImVec2(0.5f, 0.5f));
							if (ImGui::BeginPopupModal(get_language("Save?", "Сохранить?"), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
								ImGui::Dummy({ 0.f, 30.f });
								if (ImGui::Button(get_language("Yes", "Да"), { 120.f, 35.f }) && !files.empty()) {
									config_manager::save(files.at(config_index));
									ImGui::CloseCurrentPopup();
								}
								ImGui::SetItemDefaultFocus();
								ImGui::SameLine();
								if (ImGui::Button(get_language("Nope", "Нет"), { 120.f, 35.f }))
									ImGui::CloseCurrentPopup();
								ImGui::Dummy({ 0.f, 30.f });
								ImGui::EndPopup();
							}
						}
						ImGui::EndChild();
						ImGui::BeginChild("Rename", { size_mini_child_x, 121.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Spacing();
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::InputText(get_language("New Name Config", "Новое Название Конфига"), config_rename, 128);
							ImGui::PopItemWidth();

							if (ImGui::Button(get_language("Rename Config", "Переименовать Конфиг"), { item_width_mini_child, 35.f }) && !files.empty())
								ImGui::OpenPopup(get_language("Rename?", "Переименовать?"));

							ImGui::SetNextWindowPos({ ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f }, 0, ImVec2(0.5f, 0.5f));
							if (ImGui::BeginPopupModal(get_language("Rename?", "Переименовать?"), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
								ImGui::Dummy({ 0.f, 30.f });
								if (ImGui::Button(get_language("Yes", "Да"), { 120.f, 35.f }) && !files.empty()) {
									rename(std::string(XORSTR("C:/shark-hack/configs/") + files.at(config_index)).c_str(), std::string((std::string)XORSTR("C:/luno/configs/") + config_rename + ".ini").c_str());
									ImGui::CloseCurrentPopup();
								}
								ImGui::SetItemDefaultFocus();
								ImGui::SameLine();
								if (ImGui::Button(get_language("Nope", "Нет"), { 120.f, 35.f }))
									ImGui::CloseCurrentPopup();
								ImGui::Dummy({ 0.f, 30.f });
								ImGui::EndPopup();
							}
						}
						ImGui::EndChild();
						ImGui::PopStyleColor();
					}
					ImGui::EndGroup();
					ImGui::SameLine(size_mini_child_x * 2 + size_spacing * 2);
					ImGui::BeginGroup(); {
						ImGui::BeginChild("Share", { size_mini_child_x, 164.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							/*
							ImGui::Spacing();
							ImGui::PushItemWidth(item_width_mini_child);
							ImGui::InputText(get_language("Share Config ID", "Ключ Облачного Конфига"), cfg_id, 128);
							ImGui::PopItemWidth();
							if (ImGui::Button(get_language("Download Share Config", "Скачать Облачный Конфиг"), { item_width_mini_child, 35.f }))
								config_manager::download_share_settings(string(cfg_id));
							if (ImGui::Button(get_language("Upload Share Config", "Загрузить Конфиг На Облако"), { item_width_mini_child, 35.f }))
								config_manager::upload_share_settings(XORSTR("C:/luno/configs/") + files.at(config_index));*/
						}
						ImGui::EndChild();
						ImGui::BeginChild("Other", { size_mini_child_x, 252.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
							ImGui::Checkbox(get_language("OBS Spoof", "Обход OBS"), &config.settings_obs_spoof);
						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
					break;
				}
				case 10: {
					ImGui::BeginChild("Snake", { size_max_child_x, size_max_child_y }, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding); {
						ImVec2 winpos = ImGui::GetWindowPos();
						ImGuiWindow* window = ImGui::GetCurrentWindow();
						snake_game::game_scripts();
						window->DrawList->AddRectFilled({ winpos.x + 14, winpos.y + 12 }, { winpos.x + 14 + (25 * snake_game::pool.width) + 25 + 3, winpos.y + 12 + (25 * snake_game::pool.height) + 25 + 3 }, ImGui::GetColorU32(g.Style.Colors[ImGuiCol_WindowBg]));
						window->DrawList->AddRectFilled({ winpos.x + (snake_game::snake.pos[0].x * 25) + 16, winpos.y + (snake_game::snake.pos[0].y * 25) + 14 }, { winpos.x + (snake_game::snake.pos[0].x * 25) + 16 + 25, winpos.y + (snake_game::snake.pos[0].y * 25) + 14 + 25 }, ImColor(255, 255, 255, 255));

						for (int i = 1; i < snake_game::snake.length; i++)
							window->DrawList->AddRectFilled({ winpos.x + (snake_game::snake.pos[i].x * 25) + 16, winpos.y + (snake_game::snake.pos[i].y * 25) + 14 }, { winpos.x + (snake_game::snake.pos[i].x * 25) + 16 + 25, winpos.y + (snake_game::snake.pos[i].y * 25) + 14 + 25 }, ImGui::GetColorU32(ImGui::ColConvertToFloat(74.f, 156.f, 239.f)));

						if (snake_game::food.active)
							window->DrawList->AddRectFilled({ winpos.x + (snake_game::food.pos.x * 25) + 16, winpos.y + (snake_game::food.pos.y * 25) + 14 }, { winpos.x + (snake_game::food.pos.x * 25) + 16 + 25, winpos.y + (snake_game::food.pos.y * 25) + 14 + 25 }, ImColor(200, 0, 0, 255));

						window->DrawList->AddText({ winpos.x + 16, winpos.y + 14 }, ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)), std::string("TERIDICK: " + std::to_string(snake_game::snake.length - 4)).c_str());
					}
					ImGui::EndChild();		
					break;
				}
				}
			}
			ImGui::PopStyleVar();
			ImGui::PopFont();
		}
		ImGui::EndGroup();
	}
	ImGui::End();
}
