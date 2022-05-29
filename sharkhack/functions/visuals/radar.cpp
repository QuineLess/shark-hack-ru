#include "radar.h"
#include "../../gui/gui.h"

void radar::calc_radar_point(C_BasePlayer* local_player, Vector origin, float& x, float& y) {
	Vector angle;

	g_engine->GetViewAngles(&angle);

	float range = ((float)config.radar_range) / 1000.f;

	ImVec2 draw_pos = ImGui::GetWindowPos();
	ImVec2 draw_size = ImGui::GetWindowSize();

	float r_1, r_2;
	float x_1, y_1;

	Vector eye_origin = local_player->get_eye_position();

	r_1 = -(origin.y - eye_origin.y);
	r_2 = origin.x - eye_origin.x;

	float yaw_to_radian = (float)(angle.y - 90.0f) * (float)(3.14159265358979323846 / 180.0F);
	x_1 = (float)(r_2 * (float)cos((double)(yaw_to_radian)) - r_1 * sin((double)(yaw_to_radian))) / 20.f;
	y_1 = (float)(r_2 * (float)sin((double)(yaw_to_radian)) + r_1 * cos((double)(yaw_to_radian))) / 20.f;

	x_1 *= range;
	y_1 *= range;

	float sizX = draw_size.x / 2, sizY = draw_size.y / 2;

	x_1 += sizX;
	y_1 += sizY;

	x_1 += draw_pos.x;
	y_1 += draw_pos.y;

	x = x_1;
	y = y_1;
}

radar::map_info radar::get_map() {
	return { Vector(map_origin.x, map_origin.y, 0.f), Vector(float(map_scale * 1000 + 100), float(map_scale * 1000 + 100), 0.f) };
}

void radar::draw_map(C_BasePlayer* local_player) {

	if (!config.radar_texture)
		return;

	if (!is_valid())
		return;

	map_info current_map = get_map();

	ImDrawList* draw = ImGui::GetWindowDrawList();

	float h = current_map.size.y, w = current_map.size.x;

	ImVec2 map_start_pos(current_map.pos.x, current_map.pos.y);

	ImVec2 map_point_1(map_start_pos.x + w, map_start_pos.y);
	ImVec2 map_point_2(map_start_pos.x + w, map_start_pos.y - h);
	ImVec2 map_point_3(map_start_pos.x, map_start_pos.y - h);

	calc_radar_point(local_player, Vector(map_start_pos.x, map_start_pos.y, 0), map_start_pos.x, map_start_pos.y);
	calc_radar_point(local_player, Vector(map_point_1.x, map_point_1.y, 0), map_point_1.x, map_point_1.y);
	calc_radar_point(local_player, Vector(map_point_2.x, map_point_2.y, 0), map_point_2.x, map_point_2.y);
	calc_radar_point(local_player, Vector(map_point_3.x, map_point_3.y, 0), map_point_3.x, map_point_3.y);

	ImVec2 uv_a(0, 0);
	ImVec2 uv_c(1, 1);
	ImVec2 uv_b(1, 0);
	ImVec2 uv_d(0, 1);

	draw->PushTextureID(map_image);
	draw->PrimReserve(6, 4);
	draw->PrimQuadUV(map_point_1, map_point_2, map_point_3, map_start_pos, uv_b, uv_c, uv_d, uv_a, 0xFFFFFFFF);
	draw->PopTextureID();
}

void radar::load_map(long map_overview) {
	map_origin.x = *(float*)(map_overview + 0x100);
	map_origin.y = *(float*)(map_overview + 0x104);
	map_scale = *(float*)(map_overview + 0x114);
}

void radar::draw_player(C_BasePlayer* local_player) {
	ImDrawList* draw = ImGui::GetWindowDrawList();

	float x = 0, y = 0;

	for (int i = 0; i <= g_ent_list->GetHighestEntityIndex(); i++) {
		const auto entity = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(i));

		if (!entity)
			continue;

		calc_radar_point(local_player, entity->get_origin(), x, y);

		if (config.radar_grenades && is_grenade(entity))
			draw->AddRectFilled(ImVec2((float)x, (float)y), ImVec2((float)x + 4, (float)y + 4), ImColor(config.color_radar_grenades[0], config.color_radar_grenades[1], config.color_radar_grenades[2]));
	}

	for (int i = 1; i <= g_ent_list->GetHighestEntityIndex(); i++) {
		const auto entity = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(i));

		if (!entity)
			continue;

		if (i <= g_globals->max_clients) {
			if (entity->IsDormant() || entity->GetClientClass()->m_ClassID != classids::get(XORSTR("CCSPlayer")))
				continue;

			if (!entity->is_alive())
				continue;

			if (entity->is_teammate(local_player) && config.radar_enemy_only)
				continue;

			bool is_visible = local_player->is_visible(entity, HITBOX_CHEST);

			if (!config.radar_invisible && !is_visible)
				continue;

			static float* color;

			calc_radar_point(local_player, entity->get_origin(), x, y);

			if (entity->get_team() == team_id::team_terrorist)
				color = is_visible ? config.color_radar_t_visible : config.color_radar_t_invisible;
			else if (entity->get_team() == team_id::team_counter_terrorist)
				color = is_visible ? config.color_radar_ct_visible : config.color_radar_ct_invisible;

			draw->AddRectFilled({ x - 3.f, y - 3.f }, { x + 3.f, y + 3.f }, ImColor(color[0], color[1], color[2]));
			draw->AddRect({ x - 4.f, y - 4.f }, { x + 4.f, y + 4.f }, ImColor(0, 0, 0));
		}
	}
}

void radar::render(C_BasePlayer* local_player) {

	if (!config.radar_enable)
		return;

	ImGui::SetNextWindowSize({ (float)config.radar_size, (float)config.radar_size });
	ImGui::Begin("radar", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize); {
		ImDrawList* draw = ImGui::GetWindowDrawList();
		ImVec2 draw_pos = ImGui::GetCursorScreenPos(), draw_size = ImGui::GetContentRegionAvail();

		if (g_engine->IsInGame())
			draw_map(local_player);

		for (int i = 0; i < 15; i++) {
			draw->AddLine(ImVec2(draw_pos.x, draw_pos.y + i * (draw_size.y / 15.f)), ImVec2(draw_pos.x + draw_size.x, draw_pos.y + i * (draw_size.y / 15.f)), ImColor(255, 255, 255, 10));
			draw->AddLine(ImVec2(draw_pos.x + i * (draw_size.x / 15.f), draw_pos.y), ImVec2(draw_pos.x + i * (draw_size.x / 15.f), draw_pos.y + draw_size.y), ImColor(255, 255, 255, 10));
		}

		if (g_engine->IsInGame())
			draw_player(local_player);

		draw->AddRect(ImVec2(draw_pos.x, draw_pos.y), ImVec2(draw_pos.x + draw_size.x, draw_pos.y + draw_size.y), ImColor(65, 121, 280, 255), 0, 15, 4.f);

		if (local_player)
			draw->AddImage(arrow_texture, ImVec2(draw_pos.x - 10.f + (float)config.radar_size / 2.f, draw_pos.y - 10.f + (float)config.radar_size / 2.f), ImVec2(draw_pos.x + 10.f + (float)config.radar_size / 2.f, draw_pos.y + 10.f + (float)config.radar_size / 2.f));
	}
	ImGui::End();
}
