#include "render.h"
#include "../gui/font.h"

#include <mutex>

std::mutex render_mutex;

ImDrawList * render::draw_list_act;
ImDrawList * render::draw_list_rendering;
ImDrawList* render::draw_list;
ImDrawData render::draw_data;

void render::on_init() {
	draw_list = new ImDrawList(ImGui::GetDrawListSharedData());
	draw_list_act = new ImDrawList(ImGui::GetDrawListSharedData());
	draw_list_rendering = new ImDrawList(ImGui::GetDrawListSharedData());

	g_visuals_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(roboto_medium_compressed_data, roboto_medium_compressed_size, 16.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
}

void render::begin_scene() {
	draw_list->Clear();
	draw_list->PushClipRectFullScreen();
}

void render::end_scene() {
	draw_list->PopClipRect();

	render_mutex.lock();
	*draw_list_act = *draw_list;
	render_mutex.unlock();
}

ImDrawList* render::render_scene() {

	if (render_mutex.try_lock()) {
		*draw_list_rendering = *draw_list_act;
		render_mutex.unlock();
	}

	return draw_list_rendering;
}

void render::render_rect(const int x1, const int y1, const int x2, const int y2, Color color, const float rounding, const float thickness) {
	draw_list->AddRect(ImVec2(float(x1), float(y1)), ImVec2(float(x2), float(y2)), ImColor(color.r(), color.g(), color.b(), color.a()), rounding, ImDrawCornerFlags_All, thickness);
}

void render::render_rect_filled(const int x1, const int y1, const int x2, const int y2, Color color, const float rounding) {
	draw_list->AddRectFilled(ImVec2(float(x1), float(y1)), ImVec2(float(x2), float(y2)), ImColor(color.r(), color.g(), color.b(), color.a()), rounding, ImDrawCornerFlags_All);
}

void render::render_rect_filled_multicolor(const int x1, const int y1, const int x2, const int y2, Color color_up_left, Color color_down_left, Color color_up_right, Color color_down_right) {
	draw_list->AddRectFilledMultiColor(ImVec2(float(x1), float(y1)), ImVec2(float(x2), float(y2)), ImColor(color_up_left.r(), color_up_left.g(), color_up_left.b(), color_up_left.a()), ImColor(color_up_right.r(), color_up_right.g(), color_up_right.b(), color_up_right.a()), ImColor(color_down_right.r(), color_down_right.g(), color_down_right.b(), color_down_right.a()), ImColor(color_down_left.r(), color_down_left.g(), color_down_left.b(), color_down_left.a()));
}

void render::render_rect_corner(const float x, const float y, const float w, const float h, float thickness, Color color, bool outline, Color color_outline, float del) {
	float iw = w / del;
	float ih = h / del;

	render_line_float(x - 0.5f, y, x + iw, y, color, thickness);// |--
	render_line_float(x, y - 0.5f, x, y + ih, color, thickness);// |

	render_line_float(x + w - iw, y, x + w + 0.5f, y, color, thickness);// --|
	render_line_float(x + w, y - 0.5f, x + w, y + ih, color, thickness);//   |

	render_line_float(x, y + h - ih, x, y + h - 0.5f, color, thickness);// |
	render_line_float(x - 0.5f, y + h, x + iw, y + h, color, thickness);// |--

	render_line_float(x + w, y + h - ih, x + w, y + h + 0.5f, color, thickness);//   |
	render_line_float(x + w + 0.5f, y + h, x + w - iw, y + h, color, thickness);// --|

	if (outline) {
		render_rect_corner(x - 1, y - 1, w + 2, h + 2, thickness, color_outline, false, color_outline, del);
		render_rect_corner(x + 1, y + 1, w - 2, h - 2, thickness, color_outline, false, color_outline, del);

		render_line_float(x + iw, y + 1, x + iw, y - 1 - 0.5f, color_outline, thickness);
		render_line_float(x + w - iw, y + 1, x + w - iw, y - 1 - 0.5f, color_outline, thickness);
		render_line_float(x + 1, y + ih, x - 1 - 0.5f, y + ih, color_outline, thickness);
		render_line_float(x + 1 + w + 0.5f, y + ih, x - 1 + w, y + ih, color_outline, thickness);

		render_line_float(x + 1, y + h - ih, x - 1 - 0.5f, y + h - ih, color_outline, thickness);
		render_line_float(x + iw, y + 1 + h + 0.5f, x + iw, y - 1 + h, color_outline, thickness);
		render_line_float(x + 1 + w + 0.5f, y + h - ih, x - 1 + w, y + h - ih, color_outline, thickness);
		render_line_float(x + w - iw, y + 1 + h + 0.5f, x + w - iw, y - 1 + h, color_outline, thickness);
	}
}

void render::render_line_float(const float x1, const float y1, const float x2, const float y2, Color color, const float thickness) {
	draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImColor(color.r(), color.g(), color.b(), color.a()), thickness);
}

void render::render_line(const int x1, const int y1, const int x2, const int y2, Color color, const float thickness) {
	draw_list->AddLine(ImVec2(float(x1), float(y1)), ImVec2(float(x2), float(y2)), ImColor(color.r(), color.g(), color.b(), color.a()), thickness);
}

void render::render_circle(const int x, const int y, const float radius, const int points, Color color, const float thickness) {
	draw_list->AddCircle(ImVec2(float(x), float(y)), radius, ImColor(color.r(), color.g(), color.b(), color.a()), points, thickness);
}

void render::render_image(const int x1, const int y1, const int x2, const int y2, ImTextureID texture) {
	draw_list->AddImage(texture, ImVec2(float(x1), float(y1)), ImVec2(float(x2), float(y2)));
}

void render::render_text(const char* text, const int x, const int y, Color color, ImFont* font, const float size, const int effect) {

	if (!font->ContainerAtlas)
		return;

	draw_list->PushTextureID(font->ContainerAtlas->TexID);

	switch (effect) {
	case text_effect::text_outline:
		draw_list->AddText(font, size, ImVec2(float(x - 1), float(y)), ImColor(0, 0, 0, int(color.a() / 1.2f)), text);
		draw_list->AddText(font, size, ImVec2(float(x), float(y - 1)), ImColor(0, 0, 0, int(color.a() / 1.2f)), text);
		draw_list->AddText(font, size, ImVec2(float(x + 1), float(y)), ImColor(0, 0, 0, int(color.a() / 1.2f)), text);
		draw_list->AddText(font, size, ImVec2(float(x), float(y + 1)), ImColor(0, 0, 0, int(color.a() / 1.2f)), text);

		draw_list->AddText(font, size, ImVec2(float(x - 1), float(y - 1)), ImColor(0, 0, 0, int(color.a() / 1.2f)), text);
		draw_list->AddText(font, size, ImVec2(float(x + 1), float(y + 1)), ImColor(0, 0, 0, int(color.a() / 1.2f)), text);
		draw_list->AddText(font, size, ImVec2(float(x - 1), float(y + 1)), ImColor(0, 0, 0, int(color.a() / 1.2f)), text);
		draw_list->AddText(font, size, ImVec2(float(x + 1), float(y - 1)), ImColor(0, 0, 0, int(color.a() / 1.2f)), text);

		break;
	case text_effect::text_drop_shadow:
		draw_list->AddText(font, size, ImVec2(float(x + 1), float(y + 1)), ImColor(0, 0, 0, int(color.a() / 1.4f)), text);
		break;
	case text_effect::text_clear:
		break;
	}

	draw_list->AddText(font, size, ImVec2(float(x), float(y)), ImColor(color.r(), color.g(), color.b(), color.a()), text);
	draw_list->PopTextureID();
}
