#pragma once

#include "../imgui/imgui.h"
#include "../imgui/impl/imgui_impl_dx9.h"
#include "../imgui/impl/imgui_impl_win32.h"
#include "../imgui/imgui_internal.h"

#include "../sdk/interfaces.h"

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#include <thread>

inline IDirect3DTexture9* arrow_texture;
inline IDirect3DTexture9* bomb_texture;

namespace gui  {
	void styles();
	bool wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void on_init();
	void player_esp_preview();

	void render();

	extern bool menu_open;
}