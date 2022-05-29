#pragma once
#include <string>
#include <sstream>
#include <stdint.h>

#include "../imgui/imgui.h"
#include "../sdk/interfaces.h"

enum text_effect {
	text_outline,
	text_drop_shadow,
	text_clear
};

inline ImFont* g_visuals_font;

namespace render {
	extern ImDrawList * draw_list_act;
	extern ImDrawList * draw_list_rendering;
	extern ImDrawList* draw_list;
	extern ImDrawData draw_data;

	void on_init();
	void begin_scene();
	void end_scene();
	ImDrawList* render_scene();

	void render_rect(int x1, int y1, int x2, int y2, Color color, float rounding = 0.f, float thickness = 1.f);
	void render_rect_filled(int x1, int y1, int x2, int y2, Color color, float rounding = 0.f);
	void render_rect_filled_multicolor(const int x1, const int y1, const int x2, const int y2, Color color_up_left, Color color_down_left, Color color_up_right, Color color_down_right);
	void render_rect_corner(const float x, const float y, const float w, const float h, float thickness, Color color, bool outline, Color color_outline, float del);
	void render_line_float(float x1, float y1, float x2, float y2, Color color, float thickness = 1.f);
	void render_line(int x1, int y1, int x2, int y2, Color color, float thickness = 1.f);
	void render_circle(int x, int y, float radius, int points, Color color, float thickness = 1.f);
	void render_image(int x1, int y1, int x2, int y2, ImTextureID texture);
	void render_text(const char* text, int x1, int y1, Color color, ImFont* font, float size, int effect = text_outline);
};
