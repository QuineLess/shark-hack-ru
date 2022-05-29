#include "backtrack.h"
#undef max

float get_lerp_time() {
	static IConVar* cl_interp = g_cvar->FindVar(XORSTR("cl_interp"));
	static IConVar* cl_interp_ratio = g_cvar->FindVar(XORSTR("cl_interp_ratio"));
	static IConVar* sv_client_min_interp_ratio = g_cvar->FindVar(XORSTR("sv_client_min_interp_ratio"));
	static IConVar* sv_client_max_interp_ratio = g_cvar->FindVar(XORSTR("sv_client_max_interp_ratio"));
	static IConVar* cl_updaterate = g_cvar->FindVar(XORSTR("cl_updaterate"));
	static IConVar* sv_maxupdaterate = g_cvar->FindVar(XORSTR("sv_maxupdaterate"));

	const auto ratio = clamp(cl_interp_ratio->GetFloat(), sv_client_min_interp_ratio->GetFloat(), sv_client_max_interp_ratio->GetFloat());
	return max(cl_interp->GetFloat(), (ratio / (sv_maxupdaterate ? sv_maxupdaterate->GetFloat() : cl_updaterate->GetFloat())));
}

bool is_valid(float simtime) {
	static IConVar* sv_maxunlag = g_cvar->FindVar(XORSTR("sv_maxunlag"));
	const auto network = g_engine->GetNetChannel();
	if (!network)
		return false;

	const auto delta = clamp(network->GetLatency(0) + get_lerp_time(), 0.f, sv_maxunlag->GetFloat()) - (g_globals->curtime - simtime);
	return std::fabsf(delta) <= 0.2f;
}

void backtrack::frame_update(C_BasePlayer* local_player) {

	if (!config.legit_backtrack) {
		for (auto& record : records)
			record.clear();

		return;
	}

	for (int i = 1; i <= g_engine->GetMaxClients(); i++) {
		const auto entity = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(i));

		if (!entity || entity == local_player || entity->IsDormant() || !entity->is_alive()) {
			records[i].clear();
			continue;
		}

		if (entity->is_teammate(local_player))
			continue;

		if (!records[i].empty() && (records[i].front().simtime == entity->get_simulation_time()))
			continue;

		backtrack_data record;
		record.hitbox_position = entity->get_bone_position(HITBOX_HEAD);
		record.simtime = entity->get_simulation_time();

		entity->SetupBones(record.bone_matrix, 128, BONE_USED_BY_ANYTHING, g_globals->curtime);
		records[i].push_front(record);

		while (records[i].size() > 3 && records[i].size() > static_cast<size_t>(TIME_TO_TICKS(static_cast<float>(config.legit_backtrack_time) / 1000.f)))
			records[i].pop_back();
		auto invalid = std::find_if(std::cbegin(records[i]), std::cend(records[i]), [](const backtrack_data & rec) { return !is_valid(rec.simtime); });
		if (invalid != std::cend(records[i]))
			records[i].erase(invalid, std::cend(records[i]));
	}
}

void backtrack::on_move(C_BasePlayer* local_player, CUserCmd* cmd) {

	if (!config.legit_backtrack)
		return;

	if (!(cmd->buttons & IN_ATTACK))
		return;

	const auto eye_pos = local_player->get_eye_position();

	float best_fov = FLT_MAX;
	C_BasePlayer* best_target = nullptr;
	int best_target_index = NULL;
	int best_record = NULL;

	for (int i = 1; i <= g_engine->GetMaxClients(); i++) {
		const auto entity = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(i));
		if (!entity || entity == local_player || entity->IsDormant() || !entity->is_alive())
			continue;

		if (entity->is_teammate(local_player))
			continue;

		auto hitbox = entity->get_bone_position(HITBOX_CHEST);

		const auto angle = math::calc_angle(eye_pos, hitbox, cmd->viewangles + local_player->get_punch() * 2.f);
		const auto fov = std::hypotf(angle.x, angle.y);

		if (fov < best_fov) {
			best_fov = fov;
			best_target = entity;
			best_target_index = i;
		}
	}

	if (best_target) {
		if (records[best_target_index].size() <= 3)
			return;

		best_fov = FLT_MAX;

		for (size_t i = 0; i < records[best_target_index].size(); i++) {
			auto& record = records[best_target_index][i];

			if (!is_valid(record.simtime))
				continue;

			const auto angle = math::calc_angle(eye_pos, record.hitbox_position, cmd->viewangles + local_player->get_punch() * 2.f);
			const auto fov = std::hypotf(angle.x, angle.y);
			if (fov < best_fov) {
				best_fov = fov;
				best_record = i;
			}
		}
	}

	if (best_record) {
		const auto record = records[best_target_index][best_record];
		cmd->tick_count = TIME_TO_TICKS(record.simtime);
	}
}
