#include "legitbot.h"
#include "../RageBot/RageBot.hpp"

bool	in_attack;
bool	can_shot;

bool	target_fov;
bool	target_silent_fov;
bool	target_change;

float	p_fov;
float	kill_delay_timer;
float	shot_delay_timer;

QAngle	m_oldangle;
QAngle	aimpunch;

Vector	best_vector_hitbox;
Vector  best_hitboxscreen;
int		best_target;
int		best_pretarget;
int		best_hitbox;

int		cur_weapon;

int legitbot::weapon_index_data[34] = {
	WEAPON_DEAGLE,WEAPON_ELITE,WEAPON_FIVESEVEN,
	WEAPON_GLOCK,WEAPON_HKP2000,WEAPON_P250,
	WEAPON_USP_SILENCER,WEAPON_CZ75A,WEAPON_REVOLVER,
	WEAPON_TEC9,

	WEAPON_AK47,WEAPON_AUG,WEAPON_FAMAS,WEAPON_GALILAR,
	WEAPON_M249,WEAPON_M4A1,WEAPON_M4A1_SILENCER,WEAPON_MAC10,
	WEAPON_P90,WEAPON_MP5,WEAPON_UMP45,WEAPON_XM1014,WEAPON_BIZON,
	WEAPON_MAG7,WEAPON_NEGEV,WEAPON_SAWEDOFF,
	WEAPON_MP7,WEAPON_MP9,WEAPON_NOVA,WEAPON_SG556,
	WEAPON_SCAR20,WEAPON_G3SG1,

	WEAPON_AWP,WEAPON_SSG08
};

const char* legitbot::weapon_data[34] = {
	"Desert Eagle","Dual Berettas","Five-SeveN",
	"Glock-18","P2000","P250",
	"USP-S","CZ75 Auto","R8 Revolver",
	"Tec-9",

	"AK-47","AUG","FAMAS","Galil AR",
	"M249","M4A4","M4A1-S","MAC-10"	,
	"P90","MP5-SD","UMP-45","XM1014","PP-Bizon",
	"MAG-7","Negev","Sawed-Off",
	"MP7","MP9","Nova","SG 553",
	"SCAR-20","G3SG1",

	"AWP","SSG 08"
};

int legitbot::get_weapon_select(C_BasePlayer* local_player) {
	for (size_t i = 0; i < (sizeof(weapon_data) / sizeof(*weapon_data)); i++) {
		if (weapon_index_data[i] == local_player->get_weapon_handle()->get_definition_index())
			return i;
	}

	return -1;
}

bool legitbot::is_valid(C_BasePlayer* local_player) {

	if (config.radar_enable)
		return false;

	if (!config.legit_enable)
		return false;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return false;

	if (!local_player || !local_player->is_alive())
		return false;

	auto weapon = local_player->get_weapon_handle();

	if (!weapon)
		return false;

	const auto type = get_hash(weaponids.m_list[weapon->get_definition_index()]->get_item_type_name());

	if (type == utils::get_const(XORSTR("#CSGO_Type_Knife")) || type == utils::get_const(XORSTR("#CSGO_Type_Equipment")) || type == utils::get_const(XORSTR("#CSGO_Type_Grenade")) || type == utils::get_const(XORSTR("#CSGO_Type_C4")))
		return false;

	if (weapon->is_empty() || weapon->is_reloading())
		return false;

	if (weapon->get_weapon_type() == WT_Sniper && config.legit_onlyscope && !local_player->is_scoped())
		return false;

	cur_weapon = get_weapon_select(local_player);

	return true;
}

float legitbot::get_player_fov(C_BasePlayer* local_player) { 
	float fov_best_value = 10.f;
	float fov_value;
	float fov;
	float p_fov;

	const auto weapon = local_player->get_weapon_handle();
	const auto target = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(best_target));

	if (config.legit_resize_fov) {
		if (weapon->get_zoom_level() == 1)
			p_fov = 2.72f;
		else if (weapon->get_zoom_level() == 2)
			p_fov = 3.36f;
		else
			p_fov = 1.f;
	}
	else
		p_fov = 1.f;

	if (config.legitbot_for_weapon[cur_weapon].legit_fov_after_enable && local_player->get_shots_fired() > 1 && (weapon->get_weapon_type() == WT_Rifle || weapon->get_weapon_type() == WT_Machinegun || weapon->get_weapon_type() == WT_Submg))
		fov_value = config.legitbot_for_weapon[cur_weapon].legit_fov_after * fov_best_value;
	else {
		if (config.legitbot_for_weapon[cur_weapon].legit_psilent_enable && config.legitbot_for_weapon[cur_weapon].legit_psilent_type == 0 && local_player->get_shots_fired() <= 1)
			fov_value = config.legitbot_for_weapon[cur_weapon].legit_psilent_fov * fov_best_value;
		else
			fov_value = config.legitbot_for_weapon[cur_weapon].legit_fov * fov_best_value;
	}

	if (config.legitbot_for_weapon[cur_weapon].legit_dynamic_fov) {
		if (target)
			fov = (powf(fov_value + 30.f, 2.0f) / target->GetRenderOrigin().DistTo(local_player->GetRenderOrigin()) * p_fov);
		else
			fov = (powf(fov_value + 30.f, 2.0f) / 200.f * p_fov);
	}
	else
		fov = (powf(fov_value + 30.f, 2.0f) / 200.f * p_fov);

	return fov;
}

float legitbot::get_player_silent_fov(C_BasePlayer* local_player) {
	float fov_best_value = 10.f;
	float fov_value;
	float fov;

	const auto weapon = local_player->get_weapon_handle();
	const auto target = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(best_target));

	if (config.legit_resize_fov) {
		if (weapon->get_zoom_level() == 1)
			p_fov = 2.72f;
		else if (weapon->get_zoom_level() == 2)
			p_fov = 3.36f;
		else
			p_fov = 1.f;
	}
	else
		p_fov = 1.f;

	fov_value = config.legitbot_for_weapon[cur_weapon].legit_psilent_fov * fov_best_value;

	if (config.legitbot_for_weapon[cur_weapon].legit_dynamic_fov) {
		if (target)
			fov = (powf(fov_value + 30.f, 2.0f) / target->GetRenderOrigin().DistTo(local_player->GetRenderOrigin()) * p_fov);
		else
			fov = (powf(fov_value + 30.f, 2.0f) / 200.f * p_fov);
	}
	else
		fov = (powf(fov_value + 30.f, 2.0f) / 200.f * p_fov);

	return fov;
}

void legitbot::on_draw(C_BasePlayer* local_player) {

	if (!is_valid(local_player))
		return;

	int width, height;

	height = int(ImGui::GetIO().DisplaySize.y / 2);
	width = int(ImGui::GetIO().DisplaySize.x / 2);

	if (!config.legit_enable)
		return;


	if (config.legitbot_for_weapon[cur_weapon].legit_psilent_enable && config.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1) {
		if (config.visuals_draw_fov &&  get_player_fov(local_player) >= 1.f && get_player_silent_fov(local_player) < get_player_fov(local_player)) {
			render::render_circle(width, height, get_player_fov(local_player), int(get_player_fov(local_player) * 4), Color::Black(), 2.5f);
			render::render_circle(width, height, get_player_fov(local_player), int(get_player_fov(local_player) * 4), Color::Red(), 2.0f);
		}
		if (config.visuals_draw_psilent_fov && get_player_silent_fov(local_player) >= 1.f) {
			render::render_circle(width, height, get_player_silent_fov(local_player), int(get_player_silent_fov(local_player) * 4), Color::Black(), 2.5f);
			render::render_circle(width, height, get_player_silent_fov(local_player), int(get_player_silent_fov(local_player) * 4), Color::Green(), 2.0f);
		}
	}
	else if (config.legitbot_for_weapon[cur_weapon].legit_psilent_enable && config.legitbot_for_weapon[cur_weapon].legit_psilent_type == 0 && local_player->get_shots_fired() <= 1) {
		if (config.visuals_draw_psilent_fov && get_player_fov(local_player) >= 1.f) {
			render::render_circle(width, height, get_player_fov(local_player), int(get_player_fov(local_player) * 4), Color::Black(), 2.5f);
			render::render_circle(width, height, get_player_fov(local_player), int(get_player_fov(local_player) * 4), Color::Green(), 2.0f);
		}
	}
	else {
		if (config.visuals_draw_fov &&  get_player_fov(local_player) >= 1.f) {
			render::render_circle(width, height, get_player_fov(local_player), int(get_player_fov(local_player) * 4), Color::Black(), 2.5f);
			render::render_circle(width, height, get_player_fov(local_player), int(get_player_fov(local_player) * 4), Color::Red(), 2.0f);
		}
	}
}

float legitbot::get_player_smooth(C_BasePlayer* local_player) {
	float smooth;

	const auto weapon = local_player->get_weapon_handle();

	if (config.legitbot_for_weapon[cur_weapon].legit_smooth_after_enable && local_player->get_shots_fired() > 1 && (weapon->get_weapon_type() == WT_Rifle || weapon->get_weapon_type() == WT_Machinegun || weapon->get_weapon_type() == WT_Submg))
		smooth = config.legitbot_for_weapon[cur_weapon].legit_smooth_after;
	else
		smooth = config.legitbot_for_weapon[cur_weapon].legit_smooth;

	return smooth;
}

void legitbot::get_player_rcs(C_BasePlayer* local_player, QAngle &angle, C_BasePlayer* entity) {
	int rcs_x = config.legitbot_for_weapon[cur_weapon].legit_rcs_x;
	int rcs_y = config.legitbot_for_weapon[cur_weapon].legit_rcs_y;

	if (rcs_x == 0 || rcs_y == 0)
		return;

	if (entity) {
		aimpunch = local_player->get_punch();
		angle.x -= aimpunch.x * (rcs_x / 50.f);
		angle.y -= aimpunch.y * (rcs_y / 50.f);
	}
}

bool legitbot::check_player_fov(Vector screen, float fov) {
	if (Vector(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2, 0).DistTo(screen) <= fov)
		return true;

	return false;
}

void legitbot::check_player_smooth(QAngle& view_angles, QAngle angles, QAngle &out_angles, float smoothing) {
	out_angles = angles;
	out_angles.Normalize();

	auto tickrate = 1.0f / g_globals->interval_per_tick;
	if (tickrate != 64.f)
		smoothing = tickrate * smoothing / 64.f;

	if (smoothing < 1.1f)
		return;

	Vector aim_vector;
	math::angle_vectors(angles, &aim_vector);

	Vector current_vector;
	math::angle_vectors(view_angles, &current_vector);

	auto delta = aim_vector - current_vector;
	const auto smoothed = current_vector + delta / smoothing;

	math::vector_angles(smoothed, out_angles);
	out_angles.Normalize();
}

int legitbot::get_best_target(C_BasePlayer* local_player) {
	float best_dist = 1000.f;
	int p_best_target = -1;

	for (int i = 0; i < g_ent_list->GetHighestEntityIndex(); i++) {
		const auto entity = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(i));

		if (entity && !entity->get_update(local_player)) {

			if (!config.legitbot_for_weapon[cur_weapon].legit_teamattack && entity->is_teammate(local_player))
				continue;

			if (config.legitbot_for_weapon[cur_weapon].legit_ignorejump && !(entity->get_flags() & entity_flags::fl_on_ground))
				continue;

			Vector p_hitboxscreen;
			Vector p_hitbox = entity->get_bone_position(best_hitbox);

			if (config.legitbot_for_weapon[cur_weapon].legit_autowall && !local_player->is_visible(entity, p_hitbox)) {

				float damage = autowall::can_hit(local_player, p_hitbox, entity);

				if (damage < config.legitbot_for_weapon[cur_weapon].legit_mindamage)
					continue;
			}
			else if (!local_player->is_visible(entity, p_hitbox)) {
				continue;
			}

			if (math::world_to_screen(p_hitbox, p_hitboxscreen)) {
				Vector2D player_scr = Vector2D(p_hitboxscreen.x, p_hitboxscreen.y);
				float dist = (float)(sqrt(pow(player_scr.x - Vector2D(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2).x, 2) + pow(player_scr.y - Vector2D(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2).y, 2)));
				if (dist < best_dist) {
					best_dist = dist;
					p_best_target = i;
				}
			}

		}
	}

	return p_best_target;
}

int legitbot::get_best_hitbox(C_BasePlayer* local_player) {
	bool nearest = config.legitbot_for_weapon[cur_weapon].legit_nearest;
	int legit_hitbox = config.legitbot_for_weapon[cur_weapon].legit_hitbox;

	int p_best_hitbox;
	float best_dist = 1000.f;

	int start_hitbox = nearest ? 0 : legit_hitbox;
	int end_hitbox = nearest ? 18 : legit_hitbox;

	const auto weapon = local_player->get_weapon_handle();

	if (weapon->get_weapon_type() == WT_Rifle || weapon->get_weapon_type() == WT_Machinegun || weapon->get_weapon_type() == WT_Submg) {
		if (best_hitbox && in_attack && !target_change)
			p_best_hitbox = best_hitbox;
	}

	const auto entity = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(best_target));

	if (entity && !entity->get_update(local_player)) {
		for (int hitbox = start_hitbox; hitbox <= end_hitbox; hitbox++) {
			Vector p_hitbox = entity->get_bone_position(hitbox);
			Vector p_hitboxscreen;

			if (math::world_to_screen(p_hitbox, p_hitboxscreen)) {

				Vector2D hitbox_src = Vector2D(p_hitboxscreen.x, p_hitboxscreen.y);
				float dist = (float)(sqrt(pow(hitbox_src.x - Vector2D(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2).x, 2) + pow(hitbox_src.y - Vector2D(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2).y, 2)));

				if (dist < best_dist) {
					best_dist = dist;
					p_best_hitbox = hitbox;
				}
			}
		}

		Vector p_hitbox = entity->get_bone_position(p_best_hitbox);
		if (math::world_to_screen(p_hitbox, best_hitboxscreen)) {
			best_vector_hitbox = p_hitbox;
			return p_best_hitbox;
		}

	}

	return p_best_hitbox;
}

void legitbot::on_events(C_BasePlayer* local_player, IGameEvent* event) {

	if (!config.legit_enable)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player || !local_player->is_alive())
		return;

	int userid = g_engine->GetPlayerForUserID(event->GetInt("userid"));
	int attacker = g_engine->GetPlayerForUserID(event->GetInt("attacker"));

	if (attacker == g_engine->GetLocalPlayer() && userid != g_engine->GetLocalPlayer()) {
		if (config.legitbot_for_weapon[cur_weapon].legit_delay_after_kill > 0.f)
			kill_delay_timer = g_globals->curtime + (config.legitbot_for_weapon[cur_weapon].legit_delay_after_kill / 25.f);
	}

	if (!strcmp(event->GetName(), "game_newmap"))
		kill_delay_timer = 0.f;
}

bool legitbot::on_move(C_BasePlayer* local_player, CUserCmd* cmd) {

	if (!is_valid(local_player))
		return false;

	if(config.legit_enable){
		for (int i = 0; i < 8; i++){
			config.m_aRageSettings[i].m_bEnabled = false;
		}//fuck you
	}
	const auto weapon = local_player->get_weapon_handle();

	can_shot = false;
	target_fov = false;
	target_silent_fov = false;

	in_attack = cmd->buttons & IN_ATTACK;

	best_pretarget = best_target;
	best_target = get_best_target(local_player);

	if (best_target == -1)
		return false;

	target_change = (best_pretarget != best_target && in_attack) ? true : false;
	best_hitbox = get_best_hitbox(local_player);

	if (best_hitbox == -1)
		return false;

	if (check_player_fov(best_hitboxscreen, get_player_fov(local_player)))
		target_fov = true;
	if (check_player_fov(best_hitboxscreen, get_player_silent_fov(local_player)))
		target_silent_fov = true;

	static bool p_in_attack = false;
	static bool p_check_in_attack = false;

	const auto entity = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(best_target));

	if (weapon->get_weapon_type() == WT_Rifle || weapon->get_weapon_type() == WT_Submg || weapon->get_weapon_type() == WT_Shotgun || weapon->get_weapon_type() == WT_Machinegun)
		p_check_in_attack = (in_attack && target_fov) ? true : false;
	else if (weapon->get_weapon_type() == WT_Sniper || weapon->get_weapon_type() == WT_Pistol) {

		if ((config.legitbot_for_weapon[cur_weapon].legit_psilent_enable && config.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1 && (get_player_silent_fov(local_player) > get_player_fov(local_player)) ? target_silent_fov : target_fov) && !in_attack)
			p_in_attack = true;
		else if ((config.legitbot_for_weapon[cur_weapon].legit_psilent_enable && config.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1 && (get_player_silent_fov(local_player) > get_player_fov(local_player)) ? !target_silent_fov : !target_fov) && in_attack)
			p_in_attack = false;
		else if ((config.legitbot_for_weapon[cur_weapon].legit_psilent_enable && config.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1 && (get_player_silent_fov(local_player) > get_player_fov(local_player)) ? !target_silent_fov : !target_fov) && !in_attack)
			p_in_attack = false;

		p_check_in_attack = (in_attack && p_in_attack && (config.legitbot_for_weapon[cur_weapon].legit_psilent_enable && config.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1 && (get_player_silent_fov(local_player) > get_player_fov(local_player)) ? target_silent_fov : target_fov)) ? true : false;
	}

	if (config.legitbot_for_weapon[cur_weapon].legit_autofire ? (config.legitbot_for_weapon[cur_weapon].legit_psilent_enable && config.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1 && (get_player_silent_fov(local_player) > get_player_fov(local_player)) ? target_silent_fov : target_fov) : (config.legitbot_for_weapon[cur_weapon].legit_aimlock ? (config.legitbot_for_weapon[cur_weapon].legit_psilent_enable && config.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1 && (get_player_silent_fov(local_player) > get_player_fov(local_player)) ? target_silent_fov : target_fov) : p_check_in_attack)) {

		QAngle angles, smoothed;

		math::vector_angles(best_vector_hitbox - local_player->get_eye_position(), angles);

		if (config.legitbot_for_weapon[cur_weapon].legit_ignoresmoke && utils::line_goes_through_smoke(local_player->get_eye_position(), best_vector_hitbox))
			return false;

		if (config.legitbot_for_weapon[cur_weapon].legit_ignoreflash && local_player->is_flashed())
			return false;

		if (config.legitbot_for_weapon[cur_weapon].legit_delay_after_kill > 0.f && kill_delay_timer > g_globals->curtime)
			return false;

		math::normalize_angles(angles);
		math::clamp_angles(angles);

		if (!config.legitbot_for_weapon[cur_weapon].legit_psilent_enable && local_player->get_shots_fired() < 1) {
			if (config.legitbot_for_weapon[cur_weapon].legit_delay_shot > 0.f && !shot_delay_timer)
				shot_delay_timer = g_globals->curtime + (config.legitbot_for_weapon[cur_weapon].legit_delay_shot / 25.f);

			if (shot_delay_timer <= g_globals->curtime)
				shot_delay_timer = 0.f;

			if (shot_delay_timer)
				cmd->buttons &= ~IN_ATTACK;
		}

		if (local_player->get_shots_fired() > 1)
			get_player_rcs(local_player, angles, entity);

		float smooth = get_player_smooth(local_player);

		if (config.legitbot_for_weapon[cur_weapon].legit_psilent_enable && config.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1) {
			if (!target_silent_fov) {
				check_player_smooth(cmd->viewangles, angles, smoothed, smooth);
				math::clamp_angles(smoothed);
				math::normalize_angles(smoothed);

				cmd->viewangles = smoothed;

				g_engine->SetViewAngles(cmd->viewangles);
			}
			else {
				if (local_player->get_shots_fired() > 1 && weapon->get_weapon_type() == WT_Rifle || weapon->get_weapon_type() == WT_Submg || weapon->get_weapon_type() == WT_Shotgun)
					get_player_rcs(local_player, angles, entity);

				check_player_smooth(cmd->viewangles, angles, smoothed, 0.f);
				math::clamp_angles(smoothed);
				math::normalize_angles(smoothed);

				cmd->viewangles = smoothed;
			}
		}
		else if (config.legitbot_for_weapon[cur_weapon].legit_psilent_enable && config.legitbot_for_weapon[cur_weapon].legit_psilent_type == 0 && local_player->get_shots_fired() <= 1) {
			check_player_smooth(cmd->viewangles, angles, smoothed, 0.f);
			math::clamp_angles(smoothed);
			math::normalize_angles(smoothed);

			cmd->viewangles = smoothed;
		}
		else {
			check_player_smooth(cmd->viewangles, angles, smoothed, smooth);
			math::clamp_angles(smoothed);
			math::normalize_angles(smoothed);

			cmd->viewangles = smoothed;

			g_engine->SetViewAngles(cmd->viewangles);

		}

		/*if (entity && config.legitbot_for_weapon[cur_weapon].legit_autofire) {
			if (!weapon->get_weapon_data()->full_auto) {
				if (cmd->command_number % 2 == 0) {
					cmd->buttons &= ~IN_ATTACK;
				}
				else {
					cmd->buttons |= IN_ATTACK;
				}
			}
			else {
				cmd->buttons |= IN_ATTACK;
			}
		}*/

	}

	return true;
}
