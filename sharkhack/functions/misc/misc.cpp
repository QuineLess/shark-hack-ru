#include "misc.h"
#include "../Prediction/EnginePrediction.hpp"

void misc::auto_strafe(C_BasePlayer* local_player, CUserCmd* cmd) {

	if (!config.misc_auto_strafe)
		return;

	if (local_player->get_flags() & entity_flags::fl_on_ground && !(cmd->buttons & IN_JUMP))
		return;

	if (local_player->get_move_type() == move_type::move_type_noclip)
		return;

	static bool backward_style = false;

	if (cmd->buttons & IN_FORWARD)
		backward_style = false;
	else if (cmd->buttons & IN_BACK)
		backward_style = true;

	if (cmd->buttons & IN_FORWARD || cmd->buttons & IN_BACK || cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_MOVERIGHT)
		return;

	if (cmd->mousedx <= 1 && cmd->mousedx >= -1)
		return;

	if (backward_style)
		cmd->sidemove = cmd->mousedx < 0 ? 450.f : -450.f;
	else
		cmd->sidemove = cmd->mousedx < 0 ? -450.f : 450.f;
}

void misc::bunny_hop(C_BasePlayer* local_player, CUserCmd* cmd) {
	static auto was_last_time_on_ground = local_player->get_flags() & entity_flags::fl_on_ground;

	if (config.misc_bhop && !(local_player->get_flags() & entity_flags::fl_on_ground) && local_player->get_move_type() != move_type::move_type_ladder && !was_last_time_on_ground)
		cmd->buttons &= ~IN_JUMP;

	was_last_time_on_ground = local_player->get_flags() & entity_flags::fl_on_ground;
}

void misc::on_move(C_BasePlayer* local_player, CUserCmd* cmd ) 
{

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player)
		return;

	if (local_player->is_alive()) {
		if (config.misc_bhop) {
			static bool last_jumped = false;
			static bool should_fake = false;

			if (!last_jumped && should_fake) {
				should_fake = false;
				cmd->buttons |= IN_JUMP;
			}
			else if (cmd->buttons & IN_JUMP) {
				if (local_player->get_flags() & entity_flags::fl_on_ground)
					should_fake = last_jumped = true;
				else {
					cmd->buttons &= ~IN_JUMP;
					last_jumped = false;
				}
			}
			else
				should_fake = last_jumped = false;
		}

		auto_strafe(local_player, cmd);
		bunny_hop(local_player, cmd);
		fast_ladder(local_player, cmd);

		if (config.misc_mini_jump && local_player->get_fall_velocity() > 3.f && !(local_player->get_flags() & entity_flags::fl_on_ground))
			cmd->buttons |= IN_DUCK;

		if (config.misc_rank_reveal && ((cmd->buttons & IN_SCORE) != 0))
			g_client->DispatchUserMessage(50, 0, 0, nullptr);

		auto_defuse(local_player, cmd);
	}

	radar();
	name_stealer(local_player);
}

void misc::override_view(C_BasePlayer* local_player, CViewSetup* p_setup) {

	if (!config.misc_fov_changer)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player || !local_player->is_alive())
		return;

	if (local_player->is_scoped())
		return;

	p_setup->fov = float(config.misc_fov_override);
}

void misc::view_model_fov(C_BasePlayer* local_player, float& fov) {

	if (!config.misc_fov_changer)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player || !local_player->is_alive())
		return;

	if (local_player->is_scoped())
		return;

	fov = float(config.misc_fov_viewmodel);
}

void misc::edge_jump(C_BasePlayer* local_player, CUserCmd* cmd) {

	if (!config.misc_edge_jump)
		return;

	if (!local_player || !local_player->is_alive())
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	const auto get_move_type = local_player->get_move_type();

	if (get_move_type == move_type::move_type_ladder || get_move_type == move_type::move_type_noclip)
		return;

	if ((g_PredictionSystem->GetNetvars( cmd->command_number ).m_fFlags & 1) && !(local_player->get_flags() & 1))
		cmd->buttons |= IN_JUMP;
}

void misc::fast_ladder(C_BasePlayer* local_player, CUserCmd* cmd) {

	if (!config.misc_fast_ladder || !local_player)
		return;

	float up_down;

	if (local_player->get_move_type() == move_type::move_type_ladder) {

		if (cmd->viewangles.x < 30)
			up_down = -89.0f;
		else
			up_down = 89.0f;

		if (cmd->buttons & IN_FORWARD && !GetAsyncKeyState(0x41) && !GetAsyncKeyState(0x44))
		{
			cmd->viewangles.x = up_down;

			if (cmd->viewangles.y > 135 && cmd->viewangles.y < 180 || cmd->viewangles.y < -135 && cmd->viewangles.y > -180.0f) {
				if (cmd->viewangles.y != -89.0f || !(cmd->buttons & IN_MOVERIGHT)) {
					cmd->viewangles.y = -89.0f;
					cmd->buttons |= IN_MOVERIGHT;
				}
			}
			else if (cmd->viewangles.y < 135 && cmd->viewangles.y > 90 || cmd->viewangles.y < 90 && cmd->viewangles.y > 45) {
				if (cmd->viewangles.y != 179.150f || !(cmd->buttons & IN_MOVERIGHT)) {
					cmd->viewangles.y = 179.150f;
					cmd->buttons |= IN_MOVERIGHT;
				}
			}
			else if (cmd->viewangles.y > -90 && cmd->viewangles.y < -45 || cmd->viewangles.y < -90 && cmd->viewangles.y > -135) {
				if (cmd->viewangles.y != 0.20f || !(cmd->buttons & IN_MOVERIGHT)) {
					cmd->viewangles.y = 0.20f;
					cmd->buttons |= IN_MOVERIGHT;
				}
			}
			else if (cmd->viewangles.y < -0 && cmd->viewangles.y > -45 || cmd->viewangles.y < 45 && cmd->viewangles.y > 0) {
				if (cmd->viewangles.y != 89.20f || !(cmd->buttons & IN_MOVERIGHT)) {
					cmd->viewangles.y = 89.20f;
					cmd->buttons |= IN_MOVERIGHT;
				}
			}
		}
	}
}

void misc::ragdoll() {

	if (!config.misc_ragdoll)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	for (int i = 1; i <= g_ent_list->GetHighestEntityIndex(); i++) {
		const auto entity = reinterpret_cast<C_BaseEntity*>(g_ent_list->GetClientEntity(i));

		if (!entity || entity->IsDormant())
			continue;

		if (entity->GetClientClass()->m_ClassID != classids::get(XORSTR("CCSRagdoll")))
			continue;

		const auto get_ragdoll = reinterpret_cast<C_CSRagdoll*>(entity);
		const auto value = config.misc_ragdoll_value;

		get_ragdoll->get_force() *= (float)value;
		get_ragdoll->get_ragdoll_velocity() *= (float)value;
	}
}

void misc::auto_defuse(C_BasePlayer* local_player, CUserCmd* cmd) {

	if (!config.misc_auto_defuse)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (local_player->get_team() != team_id::team_counter_terrorist)
		return;

	for (int i = 1; i < g_ent_list->GetHighestEntityIndex(); i++) {
		const auto entity = reinterpret_cast<C_PlantedC4*>(g_ent_list->GetClientEntity(i));

		if (!entity || entity->IsDormant())
			continue;

		if (entity->GetClientClass()->m_ClassID != classids::get(XORSTR("CPlantedC4")))
			continue;

		if (entity->get_bomb_defused())
			continue;

		if (local_player->get_origin().DistTo(entity->get_origin()) <= 75.f)
			cmd->buttons |= IN_USE;
	}
}

void misc::radar() {

	if (!config.misc_radar)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	for (auto i = 1; i <= g_globals->max_clients; i++) {

		if (i == g_engine->GetLocalPlayer())
			continue;

		const auto player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(i));

		if (!player)
			continue;

		player->get_spotted() = true;
	}
}

void set_clan_tag(const char* tag) {
	static auto set_clantag_fn = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(((DWORD)memory.set_clantag));
	set_clantag_fn(tag, tag);
}

void misc::clan_tag() {

	static auto animated_tag = [](std::string& clan_tag) {
		std::string temp = clan_tag;
		clan_tag.erase(0, 1);
		clan_tag += temp[0];
	};

	static auto was_on = false;

	if (!config.misc_clan_tag) {
		if (was_on)
			set_clan_tag("");

		was_on = false;
		return;
	}

	was_on = true;

	if (config.misc_clan_tag_type == 0)
		set_clan_tag(XORSTR("shark-hack"));
	else if (config.misc_clan_tag_type == 1) {
		switch (int(g_globals->curtime * 2.4) % 27) {
		case 21: set_clan_tag("shark-hack   "); break;
		case 20: set_clan_tag(" shark-hack  "); break;
		case 19: set_clan_tag("  shark-hack "); break;
		case 18: set_clan_tag("   shark-hack"); break;
		case 17: set_clan_tag("    shark-hac"); break;
		case 16: set_clan_tag("     shark-ha"); break;
		case 15: set_clan_tag("      shark-h"); break;
		case 14: set_clan_tag("       shark-"); break;
		case 13: set_clan_tag("        shark"); break;
		case 12: set_clan_tag("         shar"); break;
		case 11: set_clan_tag("          sha"); break;
		case 10: set_clan_tag("           sh"); break;
		case 9:  set_clan_tag("            s"); break;
		case 8:  set_clan_tag("k            "); break;
		case 7:  set_clan_tag("ck           "); break;
		case 6:  set_clan_tag("ack          "); break;
		case 5:  set_clan_tag("hack         "); break;
		case 4:  set_clan_tag("-hack        "); break;
		case 3:  set_clan_tag("k-hack       "); break;
		case 2:  set_clan_tag("rk-hack      "); break;
		case 1:  set_clan_tag("ark-hack     "); break;
		case 0:  set_clan_tag("hark-hack    "); break;
		}
	}
	else if (config.misc_clan_tag_type == 2)
		set_clan_tag("shark-hack \r");
	else if (config.misc_clan_tag_type == 3)
		set_clan_tag(config.misc_clan_tag_name.c_str());
	else if (config.misc_clan_tag_type == 4) {
		static float old_time;
		if (g_globals->curtime - old_time >= 0.5f) {
			animated_tag(config.misc_clan_tag_name);
			set_clan_tag(config.misc_clan_tag_name.c_str());
			old_time = g_globals->curtime;
		}
	}
	else if (config.misc_clan_tag_type == 5)
		set_clan_tag(XORSTR("[VALV\xE1\xB4\xB1]"));
	else if (config.misc_clan_tag_type == 6)
		set_clan_tag(XORSTR("[VALV\xE1\xB4\xB1] \r"));
	else if (config.misc_clan_tag_type == 7)
		set_clan_tag(XORSTR(" \r"));
}

bool misc::change_name(bool reconnect, const char* new_name, float delay) {

	static bool initialized = false;
	static auto name = g_cvar->FindVar(XORSTR("name"));

	if (reconnect) {
		initialized = false;
		return false;
	}

	if (!initialized && g_engine->IsInGame()) {
		if (player_info_t player_info; g_engine->GetPlayerInfo(g_engine->GetLocalPlayer(), player_info) && (!strcmp(player_info.szName, "?empty") || !strcmp(player_info.szName, "\n\xAD\xAD\xAD"))) 
			initialized = true;
		else {
			name->on_change_callbacks.m_Size = 0;
			name->SetValue(XORSTR("\n\xAD\xAD\xAD"));
			return false;
		}
	}

	static float next_change_time = 0.0f;

	if (next_change_time <=g_globals->realtime) {
		name->SetValue(new_name);
		next_change_time = g_globals->realtime + delay;
		return true;
	}

	return false;
}

void misc::name_stealer(C_BasePlayer* local_player) {

	if (!config.misc_name_stealer)
		return;

	bool all_names_stolen = true;
	static std::vector<int> stolen_ids;

	for (int i = 1; i <= g_engine->GetMaxClients(); i++) {
		const auto entity = g_ent_list->GetClientEntity(i);

		if (!entity)
			continue;

		if (entity == local_player)
			continue;

		player_info_t player_info;

		if (g_engine->GetPlayerInfo(entity->EntIndex(), player_info) && !player_info.fakeplayer && std::find(std::begin(stolen_ids), std::end(stolen_ids), player_info.userId) == std::end(stolen_ids)) {

			all_names_stolen = false;

			if (change_name(false, std::string{ player_info.szName }.append("\x1").c_str(), 1.0f))
				stolen_ids.push_back(player_info.userId);

			break;
		}
	}

	if (all_names_stolen)
		stolen_ids.clear();
}

std::vector<int> get_observervators(int id) {
	std::vector<int> spectator_list;

	auto player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(id));

	if (!player)
		return spectator_list;

	if (!player->is_alive()) {
		const auto observer_target = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntityFromHandle(player->get_observer_target()));

		if (!observer_target)
			return spectator_list;

		player = observer_target;
	}

	for (int index = 0; index < g_engine->GetMaxClients(); index++) {
		auto check_player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(index));

		if (!check_player)
			continue;

		if (check_player->IsDormant() || check_player->is_alive())
			continue;

		const auto observer_target = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntityFromHandle(check_player->get_observer_target()));

		if (!observer_target)
			continue;

		if (player != observer_target)
			continue;

		spectator_list.push_back(index);
	}

	return spectator_list;
}

void misc::spectator_list() {

	if (!config.misc_spectators)
		return;

	int specs = 0;
	int modes = 0;
	std::string spect = "";
	std::string mode = "";
	int index = 1;

	for (int player_id : get_observervators(g_engine->GetLocalPlayer())) {
		if (player_id == g_engine->GetLocalPlayer())
			continue;

		auto player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(player_id));

		if (!player)
			continue;

		player_info_t info;
		g_engine->GetPlayerInfo(player_id, info);

		spect += info.szName;
		spect += u8"\n";
		specs++;

	}

	ImGui::PushFont(roboto_bold_elements);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::Begin("Spectator list", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
		ImGui::PopFont();

		if (specs > 0) 
			spect += "\n";

		if (modes > 0) 
			mode += "\n";

		ImGui::PushFont(roboto_regular_checkbox);
		ImGui::SetWindowSize(ImVec2(200.f, 25.f + ImGui::CalcTextSize(spect.c_str()).y));
		ImGui::Text(spect.c_str());
		ImGui::PopFont();
		index++;
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void __stdcall invoke_award_achievement(int id, int slot) {
	__asm {
		push    ebp
		mov     ebp, esp
		sub     esp, 10h
		push    ebx
		mov     ebx, id
		push    esi
		mov     esi, ecx
		mov     dword ptr[ebp - 8], esi
		push    edi
		jmp     eax
	}
}

void invoke_stub_award_achievement(void* jump_target, void* thisptr, int id, int slot) {
	__asm {
		mov     eax, jump_target
		mov     ecx, thisptr
		push    slot
		push    id
		call    invoke_award_achievement
	}
}

bool misc::unlock_all_achievements(FILE* fp) {
	void* jump_target = utils::make_ptr<void*>(memory.unlock_all_achievements);
	IAchievementMgr* achievement_manager = g_engine->GetAchievementMgr();
	void* thisptr = utils::make_ptr<void*>(reinterpret_cast<void*>(utils::make_ptr<uintptr_t*>(achievement_manager)[1]));

	uintptr_t* vtable = *utils::make_ptr<uintptr_t**>(achievement_manager);
	int32_t were_cheats_ever_on_offset = *utils::make_ptr<int32_t*>(reinterpret_cast<void*>(vtable[11]), 0x5);

	*utils::make_ptr<bool*>(thisptr, were_cheats_ever_on_offset) = false;

	for (int i = 0; i < achievement_manager->GetAchievementCount(); i++) {
		IAchievement* achievement = achievement_manager->GetAchievementByIndex(i, 0);

		if (!achievement)
			continue;

		if (!achievement->IsAchieved())
			invoke_stub_award_achievement(jump_target, thisptr, achievement->GetAchievementID(), 0);
	}

	return true;
}

void misc::get_match_cookie() {
	static bool flag = 0;
	uint64_t get_cookie = *reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(g_client_state) + 0x3C0);

	if (get_cookie != 0 && flag == 0) {
		cookie = get_cookie;
		flag = 1;
	}

	if (get_cookie != cookie && get_cookie != 0 && flag == 1)
		cookie = get_cookie;
}

void misc::reconnect_to_on_going_match() {
	*reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(g_client_state) + 0x3C0) = cookie;
	int32_t& ban_flag = **reinterpret_cast<int32_t**>(memory.action_reconnect_to_ongoing_match);

	DWORD temp_ban = ban_flag |= 1;
	ban_flag = (temp_ban << 1);

	g_engine->ClientCmd(XORSTR("retry"));
}

void TraceHull(const Vector& vec_start, const Vector& vec_end, const unsigned int n_mask, const Vector& extents, trace_t* p_trace)
{
	CTraceFilterWorldAndPropsOnly Filter;

	Ray_t ray;
	ray.Init(vec_start, vec_end);
	ray.m_Extents = extents;
	ray.m_IsRay = false;

	g_engine_trace->TraceRay(ray, n_mask, &Filter, p_trace);
}

void misc::third_person(C_BasePlayer* local_player)
{
	if (!local_player)
		return;

	/*if (!local_player->is_alive())
	{
		g_input->camera_in_thirdperson = false;
		m_flThirdpersonDistance = 50.0f;

		local_player->get_observer_mode() = 5;
		return;
	}*/

	bool bIsEnabled = utils::key_bind(config.misc_third_person_key, config.misc_third_person_key_mode);
	if (bIsEnabled)
		m_flThirdpersonDistance = min(max(m_flThirdpersonDistance, 30.0f) + 6.0f, config.misc_third_person_value);
	else
		m_flThirdpersonDistance = max(m_flThirdpersonDistance - 6.0f, 30.0f);

	if (m_flThirdpersonDistance <= (bIsEnabled ? 50.0f : 30.0f))
	{
		g_input->camera_in_thirdperson = false;
		return;
	}

	Vector vecEyePos = local_player->get_eye_position();

	QAngle angViewAngles;
	g_engine->GetViewAngles(&angViewAngles);

	g_input->camera_in_thirdperson = true;
	g_input->camera_offset = Vector(angViewAngles.x, angViewAngles.y, m_flThirdpersonDistance);

	CGameTrace Trace;

	float flExtent = 12.f + (config.misc_third_person_value / 4.8f - 18.f);

	Vector vecForward = Vector(0, 0, 0);
	math::angle_vectors(QAngle(angViewAngles.x, angViewAngles.y, 0.0f), &vecForward);
	TraceHull(vecEyePos, vecEyePos - vecForward * g_input->camera_offset.z, MASK_SOLID & ~CONTENTS_MONSTER, Vector(flExtent, flExtent, flExtent), &Trace);
	g_input->camera_offset.z *= Trace.fraction;

	if (g_input->camera_offset.z < min(30.0f, config.misc_third_person_value))
		g_input->camera_in_thirdperson = false;
}

void misc::disable_post_processing() {

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	static bool* o_override_post_processing = *(bool**)(memory.o_override_post_processing);
	*o_override_post_processing = config.misc_disable_post_processing;
}


void misc::thunder() {

	if (!config.misc_thunder_enable)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	static clock_t start_t = clock();
	double time_so_far = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	static int choose;

	const char* thunder_sounds[]{ XORSTR("ambient/playonce/weather/thunder4"),  XORSTR("ambient/playonce/weather/thunder5"),
		XORSTR("ambient/playonce/weather/thunder6"), XORSTR("ambient/playonce/weather/thunder_distant_01"),
		XORSTR("ambient/playonce/weather/thunder_distant_02") , XORSTR("ambient/playonce/weather/thunder_distant_06")
	};

	if (time_so_far > 6.0) {
		get_play_sound(thunder_sounds[choose]);
		start_t = clock();
	}
	else
		choose = utils::random(1, 6);
}