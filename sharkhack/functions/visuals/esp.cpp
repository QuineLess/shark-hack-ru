#include "esp.h"

RECT get_bbox(C_BaseEntity* ent) {
	RECT rect{};
	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t& trans = ent->get_coordinate_frame();

	Vector points[] = {
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		math::vector_transform(points[i], trans, pointsTransformed[i]);
	}

	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++) {
		if (!math::world_to_screen(pointsTransformed[i], screen_points[i]))
			return rect;
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++) {
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top > screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom < screen_points[i].y)
			bottom = screen_points[i].y;
	}

	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}

Color player_esp::get_color(C_BasePlayer* local_player, C_BasePlayer* entity, bool visible) {
	Color color;

	if (entity->get_team() == team_id::team_terrorist)
		color = visible ? Color(config.color_esp_t_visible) : Color(config.color_esp_t_invisible);
	else if (entity->get_team() == team_id::team_counter_terrorist)
		color = visible ? Color(config.color_esp_ct_visible) : Color(config.color_esp_ct_invisible);

	return color;
}

void player_esp::on_draw(C_BasePlayer* local_player) {

	if (!config.esp_enable)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	for (int32_t i = 0; i < g_engine->GetMaxClients(); i++) {
		const auto entity = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(i));

		if (!entity)
			continue;

		if (entity == local_player && !g_input->camera_in_thirdperson)
			continue;

		if (entity->IsDormant() || entity->GetClientClass()->m_ClassID != classids::get(XORSTR("CCSPlayer")))
			continue;

		if (!entity->is_alive())
			continue;

		RECT bbox = get_bbox(entity);

		static float font_size = 12.5f;
		static auto max_value = utils::is_danger_zone() ? 120 : 100;
		static auto pos = [](int y1, int y2, int value) { return (fabs(y1 - y2)) / max_value * (max_value - value); };

		if (entity->is_teammate(local_player) && config.esp_enemy_only)
			continue;

		bool is_visible = local_player->is_visible_with_smoke_check(entity, HITBOX_CHEST);

		if (!config.esp_invisible && !is_visible)
			continue;

		Color color = get_color(local_player, entity, is_visible);
		Vector end_screen;

		if (math::world_to_screen(entity->get_origin(), end_screen)) {

			if (config.esp_line) {
				render::render_line_float(ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y, end_screen.x, end_screen.y, Color::Black(), 2.5f);
				render::render_line_float(ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y, end_screen.x, end_screen.y, Color(config.color_esp_line), 1.5f);
			}

			if (config.esp_box) {
				if (config.esp_box_filled) {
					if (config.esp_box_filled_fade)
						render::render_rect_filled_multicolor(bbox.left, bbox.top, bbox.right, bbox.bottom, Color(0, 0, 0, 0), Color(color.r(), color.g(), color.b(), config.esp_box_filled_alpha), Color(0, 0, 0, 0), Color(color.r(), color.g(), color.b(), config.esp_box_filled_alpha));
					else
						render::render_rect_filled(bbox.left, bbox.top, bbox.right, bbox.bottom, Color(color.r(), color.g(), color.b(), config.esp_box_filled_alpha), (float)config.esp_box_rounding);
				}
				switch (config.esp_box_type) {
				case 0:
					if (config.esp_box_outline) {
						render::render_rect(bbox.left - 1, bbox.top - 1, bbox.right + 1, bbox.bottom + 1, Color::Black(), (float)config.esp_box_rounding);
						render::render_rect(bbox.left + 1, bbox.top + 1, bbox.right - 1, bbox.bottom - 1, Color::Black(), (float)config.esp_box_rounding);
					}
					render::render_rect(bbox.left, bbox.top, bbox.right, bbox.bottom, color, (float)config.esp_box_rounding);
					break;
				case 1:
					if (config.esp_box_outline) {
						render::render_rect(bbox.left - 1, bbox.top - 1, bbox.right + 1, bbox.bottom + 1, Color::Black(), (float)config.esp_box_rounding, 1.f);
						render::render_rect(bbox.left + 1, bbox.top + 1, bbox.right - 1, bbox.bottom - 1, Color::Black(), (float)config.esp_box_rounding, 1.f);
					}
					render::render_rect(bbox.left, bbox.top, bbox.right, bbox.bottom, color, (float)config.esp_box_rounding, 2.f);
					break;
				case 2:
					render::render_rect_corner((float)bbox.left, (float)bbox.top, (float)fabs((float)bbox.left - (float)bbox.right) - 1.f, (float)fabs((float)bbox.top - (float)bbox.bottom) - 1.f, 1.f, color, config.esp_box_outline, Color::Black(), 4.f);
					break;
				}
			}

			if (config.esp_health) {
				int health_pos = int(pos(bbox.bottom, bbox.top, entity->get_health()));
				int health_pos_horizontal = int(pos(bbox.right, bbox.left, entity->get_health()));

				float max_color = utils::is_danger_zone() ? 2.125f : 2.55f;
				int green = int(entity->get_health() * max_color);
				int red = 255 - green;

				if (config.esp_health_background) {
					if (config.esp_health_pos == health_pos::health_pos_left_1) {
						render::render_rect(bbox.left - 6, bbox.top, bbox.left - 3, bbox.bottom, Color::Black(), 0.f, 2.5f);
						render::render_rect_filled(bbox.left - 6, bbox.top, bbox.left - 3, bbox.bottom, Color(35, 35, 35));
					}
					else if (config.esp_health_pos == health_pos::health_pos_left_2) {
						if (entity->get_armor()) {
							render::render_rect(bbox.left - 13, bbox.top, bbox.left - 10, bbox.bottom, Color::Black(), 0.f, 2.5f);
							render::render_rect_filled(bbox.left - 13, bbox.top, bbox.left - 10, bbox.bottom, Color(35, 35, 35));
						}
						else {
							render::render_rect(bbox.left - 6, bbox.top, bbox.left - 3, bbox.bottom, Color::Black(), 0.f, 2.5f);
							render::render_rect_filled(bbox.left - 6, bbox.top, bbox.left - 3, bbox.bottom, Color(35, 35, 35));
						}
					}
					else if (config.esp_health_pos == health_pos::health_pos_right_1) {
						render::render_rect(bbox.right + 3, bbox.top, bbox.right + 6, bbox.bottom, Color::Black(), 0.f, 2.5f);
						render::render_rect_filled(bbox.right + 3, bbox.top, bbox.right + 6, bbox.bottom, Color(35, 35, 35));
					}
					else if (config.esp_health_pos == health_pos::health_pos_right_2) {
						if (entity->get_armor()) {
							render::render_rect(bbox.right + 10, bbox.top, bbox.right + 13, bbox.bottom, Color::Black(), 0.f, 2.5f);
							render::render_rect_filled(bbox.right + 10, bbox.top, bbox.right + 13, bbox.bottom, Color(35, 35, 35));
						}
						else {
							render::render_rect(bbox.right + 3, bbox.top, bbox.right + 6, bbox.bottom, Color::Black(), 0.f, 2.5f);
							render::render_rect_filled(bbox.right + 3, bbox.top, bbox.right + 6, bbox.bottom, Color(35, 35, 35));
						}
					}
					else if (config.esp_health_pos == health_pos::health_pos_top_1) {
						render::render_rect(bbox.left, bbox.top - 6, bbox.right, bbox.top - 3, Color::Black(), 0.f, 2.5f);
						render::render_rect_filled(bbox.left, bbox.top - 6, bbox.right, bbox.top - 3, Color(35, 35, 35));
					}
					else if (config.esp_health_pos == health_pos::health_pos_top_2) {
						if (entity->get_armor()) {
							render::render_rect(bbox.left, bbox.top - 13, bbox.right, bbox.top - 10, Color::Black(), 0.f, 2.5f);
							render::render_rect_filled(bbox.left, bbox.top - 13, bbox.right, bbox.top - 10, Color(35, 35, 35));
						}
						else {
							render::render_rect(bbox.left, bbox.top - 6, bbox.right, bbox.top - 3, Color::Black(), 0.f, 2.5f);
							render::render_rect_filled(bbox.left, bbox.top - 6, bbox.right, bbox.top - 3, Color(35, 35, 35));
						}
					}
					else if (config.esp_health_pos == health_pos::health_pos_bottom_1) {
						render::render_rect(bbox.left, bbox.bottom + 3, bbox.right, bbox.bottom + 6, Color::Black(), 0.f, 2.5f);
						render::render_rect_filled(bbox.left, bbox.bottom + 3, bbox.right, bbox.bottom + 6, Color(35, 35, 35));
					}
					else if (config.esp_health_pos == health_pos::health_pos_bottom_2) {
						if (entity->get_armor()) {
							render::render_rect(bbox.left, bbox.bottom + 10, bbox.right, bbox.bottom + 13, Color::Black(), 0.f, 2.5f);
							render::render_rect_filled(bbox.left, bbox.bottom + 10, bbox.right, bbox.bottom + 13, Color(35, 35, 35));
						}
						else {
							render::render_rect(bbox.left, bbox.bottom + 3, bbox.right, bbox.bottom + 6, Color::Black(), 0.f, 2.5f);
							render::render_rect_filled(bbox.left, bbox.bottom + 3, bbox.right, bbox.bottom + 6, Color(35, 35, 35));
						}
					}
				}

				if (config.esp_health_pos == health_pos::health_pos_left_1) {
					if (config.esp_health_fade)
						render::render_rect_filled_multicolor(bbox.left - 6, bbox.top + health_pos, bbox.left - 3, bbox.bottom, Color::Green(), Color::Red(), Color::Green(), Color::Red());
					else
						render::render_rect_filled(bbox.left - 6, bbox.top + health_pos, bbox.left - 3, bbox.bottom, Color(red, green, 0));
				}
				else if (config.esp_health_pos == health_pos::health_pos_left_2) {
					if (entity->get_armor()) {
						if (config.esp_health_fade)
							render::render_rect_filled_multicolor(bbox.left - 13, bbox.top + health_pos, bbox.left - 10, bbox.bottom, Color::Green(), Color::Red(), Color::Green(), Color::Red());
						else
							render::render_rect_filled(bbox.left - 13, bbox.top + health_pos, bbox.left - 10, bbox.bottom, Color(red, green, 0));
					}
					else {
						if (config.esp_health_fade)
							render::render_rect_filled_multicolor(bbox.left - 6, bbox.top + health_pos, bbox.left - 3, bbox.bottom, Color::Green(), Color::Red(), Color::Green(), Color::Red());
						else
							render::render_rect_filled(bbox.left - 6, bbox.top + health_pos, bbox.left - 3, bbox.bottom, Color(red, green, 0));
					}
				}
				else if (config.esp_health_pos == health_pos::health_pos_right_1) {
					if (config.esp_health_fade)
						render::render_rect_filled_multicolor(bbox.right + 3, bbox.top + health_pos, bbox.right + 6, bbox.bottom, Color::Green(), Color::Red(), Color::Green(), Color::Red());
					else
						render::render_rect_filled(bbox.right + 3, bbox.top + health_pos, bbox.right + 6, bbox.bottom, Color(red, green, 0));
				}
				else if (config.esp_health_pos == health_pos::health_pos_right_2) {
					if (entity->get_armor()) {
						if (config.esp_health_fade)
							render::render_rect_filled_multicolor(bbox.right + 10, bbox.top + health_pos, bbox.right + 13, bbox.bottom, Color::Green(), Color::Red(), Color::Green(), Color::Red());
						else
							render::render_rect_filled(bbox.right + 10, bbox.top + health_pos, bbox.right + 13, bbox.bottom, Color(red, green, 0));
					}
					else {
						if (config.esp_health_fade)
							render::render_rect_filled_multicolor(bbox.right + 3, bbox.top + health_pos, bbox.right + 6, bbox.bottom, Color::Green(), Color::Red(), Color::Green(), Color::Red());
						else
							render::render_rect_filled(bbox.right + 3, bbox.top + health_pos, bbox.right + 6, bbox.bottom, Color(red, green, 0));
					}
				}
				else if (config.esp_health_pos == health_pos::health_pos_top_1) {
					if (config.esp_health_fade)
						render::render_rect_filled_multicolor(bbox.left, bbox.top - 6, bbox.right - health_pos_horizontal, bbox.top - 3, Color::Green(), Color::Green(), Color::Red(), Color::Red());
					else
						render::render_rect_filled(bbox.left, bbox.top - 6, bbox.right - health_pos_horizontal, bbox.top - 3, Color(red, green, 0));
				}
				else if (config.esp_health_pos == health_pos::health_pos_top_2) {
					if (entity->get_armor()) {
						if (config.esp_health_fade)
							render::render_rect_filled_multicolor(bbox.left, bbox.top - 13, bbox.right - health_pos_horizontal, bbox.top - 10, Color::Green(), Color::Green(), Color::Red(), Color::Red());
						else
							render::render_rect_filled(bbox.left, bbox.top - 13, bbox.right - health_pos_horizontal, bbox.top - 10, Color(red, green, 0));
					}
					else {
						if (config.esp_health_fade)
							render::render_rect_filled_multicolor(bbox.left, bbox.top - 6, bbox.right - health_pos_horizontal, bbox.top - 3, Color::Green(), Color::Green(), Color::Red(), Color::Red());
						else
							render::render_rect_filled(bbox.left, bbox.top - 6, bbox.right - health_pos_horizontal, bbox.top - 3, Color(red, green, 0));
					}
				}
				else if (config.esp_health_pos == health_pos::health_pos_bottom_1) {
					if (config.esp_health_fade)
						render::render_rect_filled_multicolor(bbox.left, bbox.bottom + 3, bbox.right - health_pos_horizontal, bbox.bottom + 6, Color::Green(), Color::Green(), Color::Red(), Color::Red());
					else
						render::render_rect_filled(bbox.left, bbox.bottom + 3, bbox.right - health_pos_horizontal, bbox.bottom + 6, Color(red, green, 0));
				}
				else if (config.esp_health_pos == health_pos::health_pos_bottom_2) {
					if (entity->get_armor()) {
						if (config.esp_health_fade)
							render::render_rect_filled_multicolor(bbox.left, bbox.bottom + 10, bbox.right - health_pos_horizontal, bbox.bottom + 13, Color::Green(), Color::Green(), Color::Red(), Color::Red());
						else
							render::render_rect_filled(bbox.left, bbox.bottom + 10, bbox.right - health_pos_horizontal, bbox.bottom + 13, Color(red, green, 0));
					}
					else {
						if (config.esp_health_fade)
							render::render_rect_filled_multicolor(bbox.left, bbox.bottom + 3, bbox.right - health_pos_horizontal, bbox.bottom + 6, Color::Green(), Color::Green(), Color::Red(), Color::Red());
						else
							render::render_rect_filled(bbox.left, bbox.bottom + 10, bbox.right - health_pos_horizontal, bbox.bottom + 13, Color(red, green, 0));
					}
				}
			}

			if (config.esp_armor && entity->get_armor()) {
				int armor_pos = int(pos(bbox.bottom, bbox.top, entity->get_armor()));
				int armor_pos_horizontal = int(pos(bbox.right, bbox.left, entity->get_armor()));

				if (config.esp_armor_background) {
					if (config.esp_armor_pos == armor_pos::armor_pos_left_1) {
						render::render_rect(bbox.left - 6, bbox.top, bbox.left - 3, bbox.bottom, Color::Black(), 0.f, 2.5f);
						render::render_rect_filled(bbox.left - 6, bbox.top, bbox.left - 3, bbox.bottom, Color(35, 35, 35));
					}
					else if (config.esp_armor_pos == armor_pos::armor_pos_left_2) {
						render::render_rect(bbox.left - 13, bbox.top, bbox.left - 10, bbox.bottom, Color::Black(), 0.f, 2.5f);
						render::render_rect_filled(bbox.left - 13, bbox.top, bbox.left - 10, bbox.bottom, Color(35, 35, 35));
					}
					else if (config.esp_armor_pos == armor_pos::armor_pos_right_1) {
						render::render_rect(bbox.right + 3, bbox.top, bbox.right + 6, bbox.bottom, Color::Black(), 0.f, 2.5f);
						render::render_rect_filled(bbox.right + 3, bbox.top, bbox.right + 6, bbox.bottom, Color(35, 35, 35));
					}
					else if (config.esp_armor_pos == armor_pos::armor_pos_right_2) {
						render::render_rect(bbox.right + 10, bbox.top, bbox.right + 13, bbox.bottom, Color::Black(), 0.f, 2.5f);
						render::render_rect_filled(bbox.right + 10, bbox.top, bbox.right + 13, bbox.bottom, Color(35, 35, 35));
					}
					else if (config.esp_armor_pos == armor_pos::armor_pos_top_1) {
						render::render_rect(bbox.left, bbox.top - 6, bbox.right, bbox.top - 3, Color::Black(), 0.f, 2.5f);
						render::render_rect_filled(bbox.left, bbox.top - 6, bbox.right, bbox.top - 3, Color(35, 35, 35));
					}
					else if (config.esp_armor_pos == armor_pos::armor_pos_top_2) {
						render::render_rect(bbox.left, bbox.top - 13, bbox.right, bbox.top - 10, Color::Black(), 0.f, 2.5f);
						render::render_rect_filled(bbox.left, bbox.top - 13, bbox.right, bbox.top - 10, Color(35, 35, 35));
					}
					else if (config.esp_armor_pos == armor_pos::armor_pos_bottom_1) {
						render::render_rect(bbox.left, bbox.bottom + 3, bbox.right, bbox.bottom + 6, Color::Black(), 0.f, 2.5f);
						render::render_rect_filled(bbox.left, bbox.bottom + 3, bbox.right, bbox.bottom + 6, Color(35, 35, 35));
					}
					else if (config.esp_armor_pos == armor_pos::armor_pos_bottom_2) {
						render::render_rect(bbox.left, bbox.bottom + 10, bbox.right, bbox.bottom + 13, Color::Black(), 0.f, 2.5f);
						render::render_rect_filled(bbox.left, bbox.bottom + 10, bbox.right, bbox.bottom + 13, Color(35, 35, 35));
					}
				}

				if (config.esp_armor_pos == armor_pos::armor_pos_left_1) {
					if (config.esp_armor_fade)
						render::render_rect_filled_multicolor(bbox.left - 6, bbox.top + armor_pos, bbox.left - 3, bbox.bottom, Color(10, 140, 200), Color::White(), Color(10, 140, 200), Color::White());
					else
						render::render_rect_filled(bbox.left - 6, bbox.top + armor_pos, bbox.left - 3, bbox.bottom, Color(10, 140, 200));
				}
				else if (config.esp_armor_pos == armor_pos::armor_pos_left_2) {
					if (config.esp_armor_fade)
						render::render_rect_filled_multicolor(bbox.left - 13, bbox.top + armor_pos, bbox.left - 10, bbox.bottom, Color(10, 140, 200), Color::White(), Color(10, 140, 200), Color::White());
					else
						render::render_rect_filled(bbox.left - 13, bbox.top + armor_pos, bbox.left - 10, bbox.bottom, Color(10, 140, 200));
				}
				else if (config.esp_armor_pos == armor_pos::armor_pos_right_1) {
					if (config.esp_armor_fade)
						render::render_rect_filled_multicolor(bbox.right + 3, bbox.top + armor_pos, bbox.right + 6, bbox.bottom, Color(10, 140, 200), Color::White(), Color(10, 140, 200), Color::White());
					else
						render::render_rect_filled(bbox.right + 3, bbox.top + armor_pos, bbox.right + 6, bbox.bottom, Color(10, 140, 200));
				}
				else if (config.esp_armor_pos == armor_pos::armor_pos_right_2) {
					if (config.esp_armor_fade)
						render::render_rect_filled_multicolor(bbox.right + 10, bbox.top + armor_pos, bbox.right + 13, bbox.bottom, Color(10, 140, 200), Color::White(), Color(10, 140, 200), Color::White());
					else
						render::render_rect_filled(bbox.right + 10, bbox.top + armor_pos, bbox.right + 13, bbox.bottom, Color(10, 140, 200));
				}
				else if (config.esp_armor_pos == armor_pos::armor_pos_top_1) {
					if (config.esp_armor_fade)
						render::render_rect_filled_multicolor(bbox.left, bbox.top - 6, bbox.right - armor_pos_horizontal, bbox.top - 3, Color(10, 140, 200), Color(10, 140, 200), Color::White(), Color::White());
					else
						render::render_rect_filled(bbox.left, bbox.top - 6, bbox.right - armor_pos_horizontal, bbox.top - 3, Color(10, 140, 200));
				}
				else if (config.esp_armor_pos == armor_pos::armor_pos_top_2) {
					if (config.esp_armor_fade)
						render::render_rect_filled_multicolor(bbox.left, bbox.top - 13, bbox.right - armor_pos_horizontal, bbox.top - 10, Color(10, 140, 200), Color(10, 140, 200), Color::White(), Color::White());
					else
						render::render_rect_filled(bbox.left, bbox.top - 13, bbox.right - armor_pos_horizontal, bbox.top - 10, Color(10, 140, 200));
				}
				else if (config.esp_armor_pos == armor_pos::armor_pos_bottom_1) {
					if (config.esp_armor_fade)
						render::render_rect_filled_multicolor(bbox.left, bbox.bottom + 3, bbox.right - armor_pos_horizontal, bbox.bottom + 6, Color(10, 140, 200), Color(10, 140, 200), Color::White(), Color::White());
					else
						render::render_rect_filled(bbox.left, bbox.bottom + 3, bbox.right - armor_pos_horizontal, bbox.bottom + 6, Color(10, 140, 200));
				}
				else if (config.esp_armor_pos == armor_pos::armor_pos_bottom_2) {
					if (config.esp_armor_fade)
						render::render_rect_filled_multicolor(bbox.left, bbox.bottom + 10, bbox.right - armor_pos_horizontal, bbox.bottom + 13, Color(10, 140, 200), Color(10, 140, 200), Color::White(), Color::White());
					else
						render::render_rect_filled(bbox.left, bbox.bottom + 10, bbox.right - armor_pos_horizontal, bbox.bottom + 13, Color(10, 140, 200));
				}
			}

			const auto get_right = bbox.right - bbox.left;

			if (config.esp_name) {
				const auto name = entity->get_player_info().szName;
				const auto name_size = roboto_medium->CalcTextSizeA(font_size, FLT_MAX, 0.0f, name);
				const auto name_col = Color(config.color_esp_name);

				if (config.esp_name_pos == name_pos::name_pos_top_1)
					render::render_text(name, int((bbox.left + get_right / 2.0f) - name_size.x / 2.0f), int(bbox.top - name_size.y - 2.0f), name_col, roboto_medium, font_size, text_effect::text_outline);
				else if (config.esp_name_pos == name_pos::name_pos_top_2) {
					if (config.esp_armor && config.esp_armor_pos == armor_pos::armor_pos_top_1) {
						if (entity->get_armor())
							render::render_text(name, int((bbox.left + get_right / 2.0f) - name_size.x / 2.0f), int(bbox.top - name_size.y - 10.0f), name_col, roboto_medium, font_size, text_effect::text_outline);
						else
							render::render_text(name, int((bbox.left + get_right / 2.0f) - name_size.x / 2.0f), int(bbox.top - name_size.y - 2.0f), name_col, roboto_medium, font_size, text_effect::text_outline);
					}
					else
						render::render_text(name, int((bbox.left + get_right / 2.0f) - name_size.x / 2.0f), int(bbox.top - name_size.y - 10.0f), name_col, roboto_medium, font_size, text_effect::text_outline);
				}
				else if (config.esp_name_pos == name_pos::name_pos_top_3) {
					if (entity->get_armor())
						render::render_text(name, int((bbox.left + get_right / 2.0f) - name_size.x / 2.0f), int(bbox.top - name_size.y - 18.0f), name_col, roboto_medium, font_size, text_effect::text_outline);
					else
						render::render_text(name, int((bbox.left + get_right / 2.0f) - name_size.x / 2.0f), int(bbox.top - name_size.y - 10.0f), name_col, roboto_medium, font_size, text_effect::text_outline);
				}
				else if (config.esp_name_pos == name_pos::name_pos_left_1)
					render::render_text(name, int(bbox.left - 4 - name_size.x), bbox.top, name_col, roboto_medium, font_size, text_effect::text_outline);
				else if (config.esp_name_pos == name_pos::name_pos_left_2) {
					if (config.esp_armor && config.esp_armor_pos == armor_pos::armor_pos_left_1) {
						if (entity->get_armor())
							render::render_text(name, int(bbox.left - 12 - name_size.x), bbox.top, name_col, roboto_medium, font_size, text_effect::text_outline);
						else
							render::render_text(name, int(bbox.left - 4 - name_size.x), bbox.top, name_col, roboto_medium, font_size, text_effect::text_outline);
					}
					else
						render::render_text(name, int(bbox.left - 12 - name_size.x), bbox.top, name_col, roboto_medium, font_size, text_effect::text_outline);
				}
				else if (config.esp_name_pos == name_pos::name_pos_left_3) {
					if (entity->get_armor())
						render::render_text(name, int(bbox.left - 20 - name_size.x), bbox.top, name_col, roboto_medium, font_size, text_effect::text_outline);
					else
						render::render_text(name, int(bbox.left - 12 - name_size.x), bbox.top, name_col, roboto_medium, font_size, text_effect::text_outline);
				}
				else if (config.esp_name_pos == name_pos::name_pos_right_1)
					render::render_text(name, bbox.right + 4, bbox.top, name_col, roboto_medium, font_size, text_effect::text_outline);
				else if (config.esp_name_pos == name_pos::name_pos_right_2) {
					if (config.esp_armor && config.esp_armor_pos == armor_pos::armor_pos_right_1) {
						if (entity->get_armor())
							render::render_text(name, bbox.right + 12, bbox.top, name_col, roboto_medium, font_size, text_effect::text_outline);
						else
							render::render_text(name, bbox.right + 4, bbox.top, name_col, roboto_medium, font_size, text_effect::text_outline);
					}
					else
						render::render_text(name, bbox.right + 12, bbox.top, name_col, roboto_medium, font_size, text_effect::text_outline);
				}
				else if (config.esp_name_pos == name_pos::name_pos_right_3) {
					if (entity->get_armor())
						render::render_text(name, bbox.right + 20, bbox.top, name_col, roboto_medium, font_size, text_effect::text_outline);
					else
						render::render_text(name, bbox.right + 12, bbox.top, name_col, roboto_medium, font_size, text_effect::text_outline);
				}
			}

			if (config.esp_weapon) {
				if (!entity->get_weapon_handle())
					continue;
				const auto weapon = config.esp_weapon_icon ? entity->get_weapon_handle()->get_weapon_icon() : entity->get_weapon_handle()->get_weapon();
				const auto font = config.esp_weapon_icon ? weapon_icons : roboto_medium;
				const auto weapon_size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, weapon);
				const auto weapon_col = Color(config.color_esp_weapon);

				if (config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_1)
					render::render_text(weapon, int((bbox.left + get_right / 2.0f) - weapon_size.x / 2.0f), bbox.bottom + 2, weapon_col, font, font_size, text_effect::text_outline);
				else if (config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_2) {
					if (config.esp_armor && config.esp_armor_pos == armor_pos::armor_pos_bottom_1) {
						if (entity->get_armor())
							render::render_text(weapon, int((bbox.left + get_right / 2.0f) - weapon_size.x / 2.0f), bbox.bottom + 10, weapon_col, font, font_size, text_effect::text_outline);
						else
							render::render_text(weapon, int((bbox.left + get_right / 2.0f) - weapon_size.x / 2.0f), bbox.bottom + 2, weapon_col, font, font_size, text_effect::text_outline);
					}
					else
						render::render_text(weapon, int((bbox.left + get_right / 2.0f) - weapon_size.x / 2.0f), bbox.bottom + 10, weapon_col, font, font_size, text_effect::text_outline);
				}
				else if (config.esp_weapon_pos == weapon_pos::weapon_pos_bottom_3) {
					if (entity->get_armor())
						render::render_text(weapon, int((bbox.left + get_right / 2.0f) - weapon_size.x / 2.0f), bbox.bottom + 18, weapon_col, font, font_size, text_effect::text_outline);
					else
						render::render_text(weapon, int((bbox.left + get_right / 2.0f) - weapon_size.x / 2.0f), bbox.bottom + 10, weapon_col, font, font_size, text_effect::text_outline);
				}
				else if (config.esp_weapon_pos == weapon_pos::weapon_pos_left_1)
					render::render_text(weapon, int(bbox.left - 4 - weapon_size.x), int(bbox.bottom - weapon_size.y), weapon_col, font, font_size, text_effect::text_outline);
				else if (config.esp_weapon_pos == weapon_pos::weapon_pos_left_2) {
					if (config.esp_armor && config.esp_armor_pos == armor_pos::armor_pos_left_1) {
						if (entity->get_armor())
							render::render_text(weapon, int(bbox.left - 12 - weapon_size.x), int(bbox.bottom - weapon_size.y), weapon_col, font, font_size, text_effect::text_outline);
						else
							render::render_text(weapon, int(bbox.left - 4 - weapon_size.x), int(bbox.bottom - weapon_size.y), weapon_col, font, font_size, text_effect::text_outline);
					}
					else
						render::render_text(weapon, int(bbox.left - 12 - weapon_size.x), int(bbox.bottom - weapon_size.y), weapon_col, font, font_size, text_effect::text_outline);
				}
				else if (config.esp_weapon_pos == weapon_pos::weapon_pos_left_3) {
					if (entity->get_armor())
						render::render_text(weapon, int(bbox.left - 20 - weapon_size.x), int(bbox.bottom - weapon_size.y), weapon_col, font, font_size, text_effect::text_outline);
					else
						render::render_text(weapon, int(bbox.left - 12 - weapon_size.x), int(bbox.bottom - weapon_size.y), weapon_col, font, font_size, text_effect::text_outline);
				}
				else if (config.esp_weapon_pos == weapon_pos::weapon_pos_right_1)
					render::render_text(weapon, bbox.right + 4, int(bbox.bottom - weapon_size.y), weapon_col, font, font_size, text_effect::text_outline);
				else if (config.esp_weapon_pos == weapon_pos::weapon_pos_right_2) {
					if (config.esp_armor && config.esp_armor_pos == armor_pos::armor_pos_right_1) {
						if (entity->get_armor())
							render::render_text(weapon, bbox.right + 12, int(bbox.bottom - weapon_size.y), weapon_col, font, font_size, text_effect::text_outline);
						else
							render::render_text(weapon, bbox.right + 4, int(bbox.bottom - weapon_size.y), weapon_col, font, font_size, text_effect::text_outline);
					}
					else
						render::render_text(weapon, bbox.right + 12, int(bbox.bottom - weapon_size.y), weapon_col, font, font_size, text_effect::text_outline);
				}
				else if (config.esp_weapon_pos == weapon_pos::weapon_pos_right_3) {
					if (entity->get_armor())
						render::render_text(weapon, bbox.right + 20, int(bbox.bottom - weapon_size.y), weapon_col, font, font_size, text_effect::text_outline);
					else
						render::render_text(weapon, bbox.right + 12, int(bbox.bottom - weapon_size.y), weapon_col, font, font_size, text_effect::text_outline);
				}
			}
		}
	}
}

void world_esp::get_esp(C_BaseEntity* entity, const char* name, bool enable, bool box, Color color, bool icon) {

	if (!enable)
		return;

	RECT bbox = get_bbox(entity);
	static float font_size = 12.5f;
	const auto get_right = bbox.right - bbox.left;
	const auto get_font = icon ? weapon_icons : roboto_medium;
	const auto item_size = get_font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, name);

	Vector end_screen;
	if (math::world_to_screen(entity->get_origin(), end_screen)) {
		render::render_text(name, int((bbox.left + get_right / 2.0f) - item_size.x / 2.0f), int(bbox.top - item_size.y - 2.0f), color, get_font, font_size, text_effect::text_outline);
		if (box)
			render::render_rect_corner((float)bbox.left, (float)bbox.top, (float)fabs((float)bbox.left - (float)bbox.right), (float)fabs((float)bbox.top - (float)bbox.bottom), 1.f, color, false, color, 4.f);
	}
}

void world_esp::on_draw(C_BasePlayer* local_player) {

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	for (int i = 1; i <= g_ent_list->GetHighestEntityIndex(); i++) {
		const auto entity = reinterpret_cast<C_BaseEntity*>(g_ent_list->GetClientEntity(i));

		if (!entity || entity->IsDormant())
			continue;

		if (entity == local_player && !g_input->camera_in_thirdperson)
			continue;

		if (entity->is_weapon() && entity->get_owner_entity() == -1) {
			if (config.esp_world_weapon_enable)
				get_esp(entity, reinterpret_cast<C_BaseCombatWeapon*>(entity)->get_weapon_icon(), config.esp_world_weapon_weapons, config.esp_world_weapon_box, Color(config.color_esp_world_weapon), true);
		}
		else {
			auto class_id = entity->GetClientClass()->m_ClassID;

			if (config.esp_world_weapon_enable) {
				if (class_id == classids::get(XORSTR("CPlantedC4"))) {
					get_esp(entity, "N", config.esp_world_weapon_planted_bomb, config.esp_world_weapon_box, Color(config.color_esp_world_weapon), true);
				}
			}

			auto zone_distance = local_player->get_origin().DistTo(entity->get_origin());

			if (config.esp_danger_zone_enable && zone_distance <= (config.esp_danger_zone_distance + 500.f)) {
				auto color = Color(config.color_esp_danger_zone);
				if (class_id == classids::get(XORSTR("CDronegun")))
					get_esp(entity, utils::to_utf8(std::wstring{ g_localize->Find(XORSTR("#SFUI_WPNHUD_AutoSentry")) }.append(L" (").append(std::to_wstring(entity->get_sentry_health())).append(XORWSTR(L" HP)")).c_str()).c_str(), config.esp_danger_zone[0], config.esp_danger_zone_box, color);
				else if (class_id == classids::get(XORSTR("CDrone")))
					get_esp(entity, XORSTR("Drone"), config.esp_danger_zone[1], config.esp_danger_zone_box, color);
				else if (class_id == classids::get(XORSTR("CItemCash")))
					get_esp(entity, XORSTR("Cash"), config.esp_danger_zone[2], config.esp_danger_zone_box, color);
				else if (class_id == classids::get(XORSTR("CPhysPropLootCrate"))) {
					const auto model_name = entity->GetModel()->szName;

					if (strstr(model_name, XORSTR("dufflebag")))
						get_esp(entity, XORSTR("Cash Dufflebag"), config.esp_danger_zone[3], config.esp_danger_zone_box, color);
					else if (strstr(model_name, XORSTR("case_pistol")))
						get_esp(entity, XORSTR("Pistol Case"), config.esp_danger_zone[4], config.esp_danger_zone_box, color);
					else if (strstr(model_name, XORSTR("case_light")))
						get_esp(entity, XORSTR("Light Case"), config.esp_danger_zone[5], config.esp_danger_zone_box, color);
					else if (strstr(model_name, XORSTR("case_heavy")))
						get_esp(entity, XORSTR("Heavy Case"), config.esp_danger_zone[6], config.esp_danger_zone_box, color);
					else if (strstr(model_name, XORSTR("case_explosive")))
						get_esp(entity, XORSTR("Explosive Case"), config.esp_danger_zone[7], config.esp_danger_zone_box, color);
					else if (strstr(model_name, XORSTR("case_tools")))
						get_esp(entity, XORSTR("Tools Case"), config.esp_danger_zone[8], config.esp_danger_zone_box, color);
				}
				else if (class_id == classids::get(XORSTR("CPhysPropWeaponUpgrade"))) {
					const auto model_name = entity->GetModel()->szName;

					if (strstr(model_name, XORSTR("dz_armor_helmet")))
						get_esp(entity, XORSTR("Full Armor"), config.esp_danger_zone[9], config.esp_danger_zone_box, color);
					else if (strstr(model_name, XORSTR("dz_armor")))
						get_esp(entity, XORSTR("Armor"), config.esp_danger_zone[10], config.esp_danger_zone_box, color);
					else if (strstr(model_name, XORSTR("dz_helmet")))
						get_esp(entity, XORSTR("Helmet"), config.esp_danger_zone[11], config.esp_danger_zone_box, color);
					else if (strstr(model_name, XORSTR("parachutepack")))
						get_esp(entity, XORSTR("Parachute"), config.esp_danger_zone[12], config.esp_danger_zone_box, color);
					else if (strstr(model_name, XORSTR("briefcase")))
						get_esp(entity, XORSTR("Briefcase"), config.esp_danger_zone[13], config.esp_danger_zone_box, color);
					else if (strstr(model_name, XORSTR("upgrade_tablet")))
						get_esp(entity, XORSTR("Tablet Upgrade"), config.esp_danger_zone[14], config.esp_danger_zone_box, color);
					else if (strstr(model_name, XORSTR("exojump")))
						get_esp(entity, XORSTR("ExoJump"), config.esp_danger_zone[15], config.esp_danger_zone_box, color);
				}
				else if (class_id == classids::get(XORSTR("CPhysPropAmmoBox")))
					get_esp(entity, XORSTR("Ammobox"), config.esp_danger_zone[16], config.esp_danger_zone_box, color);
				else if (class_id == classids::get(XORSTR("CPhysPropRadarJammer")))
					get_esp(entity, utils::to_utf8(g_localize->Find(XORSTR("#TabletJammer"))).c_str(), config.esp_danger_zone[17], config.esp_danger_zone_box, color);
			}
		}
	}
}