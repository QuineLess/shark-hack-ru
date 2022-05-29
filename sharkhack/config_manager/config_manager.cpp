#include "../sdk/interfaces.h"
#include "../functions/changer/skins.h"
#include "../functions/aimbot/legitbot.h"

#include <wininet.h>
#pragma comment(lib, "wininet.lib")

void  config_manager::read_directory(const std::string& name, std::vector<std::string>& v) {
	std::string pattern(name);
	pattern.append("\\*");

	WIN32_FIND_DATA data;
	HANDLE find;

	if ((find = FindFirstFileA(pattern.c_str(), (LPWIN32_FIND_DATAA)&data)) != INVALID_HANDLE_VALUE) {
		do {
			std::string filename((char*)data.cFileName);
			if (filename.substr(filename.find_last_of(".") + 1) == XORSTR("ini"))
				v.push_back(filename);
		} 
		while (FindNextFileA(find, (LPWIN32_FIND_DATAA)&data) != 0);
		FindClose(find);
	}
}

void config_manager::save(std::string config_file) {
	std::string file = XORSTR("C:/shark-hack/configs/") + config_file;

	for (int type = 0; type < config.m_aRageSettings.size(); type++) {
		
		save_value("rage", std::string("m_iFOV_" + std::to_string(type)).c_str(), std::to_string(config.m_aRageSettings[type].m_iFOV).c_str());
		save_value("rage", std::string("m_iHitChance_" + std::to_string(type)).c_str(), std::to_string(config.m_aRageSettings[type].m_iHitChance).c_str());
		save_value("rage", std::string("m_iDoubleTapHitChance_" + std::to_string(type)).c_str(), std::to_string(config.m_aRageSettings[type].m_iDoubleTapHitChance).c_str());
		save_value("rage", std::string("m_iMinDamage_" + std::to_string(type)).c_str(), std::to_string(config.m_aRageSettings[type].m_iMinDamage).c_str());
		save_value("rage", std::string("m_iMinDamageOverride_" + std::to_string(type)).c_str(), std::to_string(config.m_aRageSettings[type].m_iMinDamageOverride).c_str());
		save_value("rage", std::string("m_iAccuracyBoost_" + std::to_string(type)).c_str(), std::to_string(config.m_aRageSettings[type].m_iAccuracyBoost).c_str());
		save_value("rage", std::string("m_iHeadScale_" + std::to_string(type)).c_str(), std::to_string(config.m_aRageSettings[type].m_iHeadScale).c_str());
		save_value("rage", std::string("m_iBodyScale_" + std::to_string(type)).c_str(), std::to_string(config.m_aRageSettings[type].m_iBodyScale).c_str());
		save_value("rage", std::string("m_bPreferBody_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_bPreferBody ? "true" : "false");
		save_value("rage", std::string("m_bAutoScope_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_bAutoScope ? "true" : "false");
		save_value("rage", std::string("m_bAutoStop_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_bAutoStop ? "true" : "false");
		save_value("rage", std::string("m_bEnabled_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_bEnabled ? "true" : "false");
		save_value("rage", std::string("m_bLethalSafety_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_bLethalSafety ? "true" : "false");
		save_value("rage", std::string("m_bPreferSafe_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_bPreferSafe ? "true" : "false");

		for (int i = 0; i < 6; i++) {
			save_value("rage", std::string("m_Hitboxes_" + std::to_string(type) + "_" + std::to_string(i)).c_str(), config.m_aRageSettings[type].m_Hitboxes[i] ? "true" : "false");
			save_value("rage", std::string("m_SafeHitboxes_" + std::to_string(type) + "_" + std::to_string(i)).c_str(), config.m_aRageSettings[type].m_SafeHitboxes[i] ? "true" : "false");
			save_value("rage", std::string("m_Multipoints_" + std::to_string(type) + "_" + std::to_string(i)).c_str(), config.m_aRageSettings[type].m_Multipoints[i] ? "true" : "false");
		}

		for (int i = 0; i < 2; i++) {
			save_value("rage", std::string("m_AutoStopOptions_" + std::to_string(type) + "_" + std::to_string(i)).c_str(), config.m_aRageSettings[type].m_AutoStopOptions[i] ? "true" : "false");
		}

		for (int i = 0; i < 2; i++) {
			save_value("rage", std::string("m_DoubleTapOptions_" + std::to_string(type) + "_" + std::to_string(i)).c_str(), config.m_aRageSettings[type].m_DoubleTapOptions[i] ? "true" : "false");
		}
	}
	

	save_value("rage", "m_bFakeDuck", config.m_bFakeDuck ? "true" : "false");
	save_value("rage", "m_bAutoDirection", config.m_bAutoDirection ? "true" : "false");
	save_value("rage", "m_bJitterMove", config.m_bJitterMove ? "true" : "false");
	save_value("rage", "m_bSlowWalk", config.m_bSlowWalk ? "true" : "false");
	save_value("rage", "m_bFakeLagEnabled", config.m_bFakeLagEnabled ? "true" : "false");
	save_value("rage", "m_bAntiAim", config.m_bAntiAim ? "true" : "false");
	save_value("rage", "m_bAntiAimAtTargets", config.m_bAntiAimAtTargets ? "true" : "false");
	save_value("rage", "m_iLeftFakeLimit", std::to_string(config.m_iLeftFakeLimit).c_str());
	save_value("rage", "m_iLagLimit", std::to_string(config.m_iLagLimit).c_str());
	save_value("rage", "m_iTriggerLimit", std::to_string(config.m_iTriggerLimit).c_str());
	save_value("rage", "m_iRightFakeLimit", std::to_string(config.m_iRightFakeLimit).c_str());
	save_value("rage", "m_iJitterAmount", std::to_string(config.m_iJitterAmount).c_str());
	save_value("rage", "m_iYawAddOffset", std::to_string(config.m_iYawAddOffset).c_str());
	save_value("rage", "m_iPitchMode", std::to_string(config.m_iPitchMode).c_str());
	save_value("rage", "m_iLegMovement", std::to_string(config.m_iLegMovement).c_str());

	save_value("rage", "double_tap_key", std::to_string(config.double_tap_key).c_str());
	save_value("rage", "double_tap_key_mode", std::to_string(config.double_tap_key_mode).c_str());
	save_value("rage", "hide_shots_key", std::to_string(config.hide_shots_key).c_str());
	save_value("rage", "hide_shots_key_mode", std::to_string(config.hide_shots_key_mode).c_str());
	save_value("rage", "safe_point_key", std::to_string(config.safe_point_key).c_str());
	save_value("rage", "safe_point_key_mode", std::to_string(config.safe_point_key_mode).c_str());
	save_value("rage", "manual_back_key", std::to_string(config.manual_back_key).c_str());
	save_value("rage", "manual_left_key", std::to_string(config.manual_left_key).c_str());
	save_value("rage", "manual_right_key", std::to_string(config.manual_right_key).c_str());
	save_value("rage", "inverter_key", std::to_string(config.inverter_key).c_str());
	save_value("rage", "inverter_key_mode", std::to_string(config.inverter_key_mode).c_str());
	save_value("rage", "m_bOverrideDamage_key", std::to_string(config.m_bOverrideDamage_key).c_str());
	save_value("rage", "m_bOverrideDamage_key_mode", std::to_string(config.m_bOverrideDamage_key_mode).c_str());

	save_value("legit", "legit_enable", config.legit_enable ? "true" : "false");
	save_value("legit", "legit_onlyscope", config.legit_onlyscope ? "true" : "false");
	save_value("legit", "legit_resize_fov", config.legit_resize_fov ? "true" : "false");
	save_value("legit", "legit_autoselectweapon", config.legit_autoselectweapon ? "true" : "false");
	save_value("legit", "legit_desync", config.legit_desync ? "true" : "false");
	save_value("legit", "legit_desync_key", std::to_string(config.legit_desync_key).c_str());
	save_value("legit", "legit_backtrack", config.legit_backtrack ? "true" : "false");
	save_value("legit", "legit_backtrack_time", std::to_string(config.legit_backtrack_time).c_str());
	save_value("legit", "legit_backtrack_chams_enable", config.legit_backtrack_chams_enable ? "true" : "false");
	save_value("legit", "legit_backtrack_chams", std::to_string(config.legit_backtrack_chams).c_str());

	for (size_t i = 0; i < (sizeof(legitbot::weapon_data) / sizeof(*legitbot::weapon_data)); i++) {
		save_value(legitbot::weapon_data[i], "legit_nearest", config.legitbot_for_weapon[i].legit_nearest ? "true" : "false");
		save_value(legitbot::weapon_data[i], "legit_psilent_enable", config.legitbot_for_weapon[i].legit_psilent_enable ? "true" : "false");
		save_value(legitbot::weapon_data[i], "legit_teamattack", config.legitbot_for_weapon[i].legit_teamattack ? "true" : "false");
		save_value(legitbot::weapon_data[i], "legit_ignoresmoke", config.legitbot_for_weapon[i].legit_ignoresmoke ? "true" : "false");
		save_value(legitbot::weapon_data[i], "legit_ignoreflash", config.legitbot_for_weapon[i].legit_ignoreflash ? "true" : "false");
		save_value(legitbot::weapon_data[i], "legit_ignoreflash", config.legitbot_for_weapon[i].legit_ignorejump ? "true" : "false");
		save_value(legitbot::weapon_data[i], "legit_aimlock", config.legitbot_for_weapon[i].legit_aimlock ? "true" : "false");
		save_value(legitbot::weapon_data[i], "legit_autofire", config.legitbot_for_weapon[i].legit_autofire ? "true" : "false");
		save_value(legitbot::weapon_data[i], "legit_dynamic_fov", config.legitbot_for_weapon[i].legit_dynamic_fov ? "true" : "false");
		save_value(legitbot::weapon_data[i], "legit_autowall", config.legitbot_for_weapon[i].legit_autowall ? "true" : "false");
		save_value(legitbot::weapon_data[i], "legit_fov_after_enable", config.legitbot_for_weapon[i].legit_fov_after_enable ? "true" : "false");
		save_value(legitbot::weapon_data[i], "legit_smooth_after_enable", config.legitbot_for_weapon[i].legit_smooth_after_enable ? "true" : "false");

		save_value(legitbot::weapon_data[i], "legit_hitbox", std::to_string(config.legitbot_for_weapon[i].legit_hitbox).c_str());
		save_value(legitbot::weapon_data[i], "legit_rcs_x", std::to_string(config.legitbot_for_weapon[i].legit_rcs_x).c_str());
		save_value(legitbot::weapon_data[i], "legit_rcs_y", std::to_string(config.legitbot_for_weapon[i].legit_rcs_y).c_str());
		save_value(legitbot::weapon_data[i], "legit_psilent_type", std::to_string(config.legitbot_for_weapon[i].legit_psilent_type).c_str());

		save_value(legitbot::weapon_data[i], "legit_fov", std::to_string(config.legitbot_for_weapon[i].legit_fov).c_str());
		save_value(legitbot::weapon_data[i], "legit_smooth", std::to_string(config.legitbot_for_weapon[i].legit_smooth).c_str());
		save_value(legitbot::weapon_data[i], "legit_fov_after", std::to_string(config.legitbot_for_weapon[i].legit_fov_after).c_str());
		save_value(legitbot::weapon_data[i], "legit_smooth_after", std::to_string(config.legitbot_for_weapon[i].legit_smooth_after).c_str());
		save_value(legitbot::weapon_data[i], "legit_psilent_fov", std::to_string(config.legitbot_for_weapon[i].legit_psilent_fov).c_str());
		save_value(legitbot::weapon_data[i], "legit_delay_after_kill", std::to_string(config.legitbot_for_weapon[i].legit_delay_after_kill).c_str());
		save_value(legitbot::weapon_data[i], "legit_delay_shot", std::to_string(config.legitbot_for_weapon[i].legit_delay_shot).c_str());
		save_value(legitbot::weapon_data[i], "legit_mindamage", std::to_string(config.legitbot_for_weapon[i].legit_mindamage).c_str());
	}

	save_value("visuals", "esp_enable", config.esp_enable ? "true" : "false");
	save_value("visuals", "esp_enemy_only", config.esp_enemy_only ? "true" : "false");
	save_value("visuals", "esp_invisible", config.esp_invisible ? "true" : "false");
	save_value("visuals", "esp_box", config.esp_box ? "true" : "false");
	save_value("visuals", "esp_box_filled", config.esp_box_filled ? "true" : "false");
	save_value("visuals", "esp_box_outline", config.esp_box_outline ? "true" : "false");
	save_value("visuals", "esp_box_filled_fade", config.esp_box_filled_fade ? "true" : "false");
	save_value("visuals", "esp_box_filled_alpha", std::to_string(config.esp_box_filled_alpha).c_str());
	save_value("visuals", "esp_box_rounding", std::to_string(config.esp_box_rounding).c_str());
	save_value("visuals", "esp_box_type", std::to_string(config.esp_box_type).c_str());
	save_value("visuals", "esp_health", config.esp_health ? "true" : "false");
	save_value("visuals", "esp_health_fade", config.esp_health_fade ? "true" : "false");
	save_value("visuals", "esp_health_background", config.esp_health_background ? "true" : "false");
	save_value("visuals", "esp_health_pos", std::to_string(config.esp_health_pos).c_str());
	save_value("visuals", "esp_armor", config.esp_armor ? "true" : "false");
	save_value("visuals", "esp_armor_fade", config.esp_armor_fade ? "true" : "false");
	save_value("visuals", "esp_armor_background", config.esp_armor_background ? "true" : "false");
	save_value("visuals", "esp_armor_pos", std::to_string(config.esp_armor_pos).c_str());
	save_value("visuals", "esp_sound_enable", config.esp_sound_enable ? "true" : "false");
	save_value("visuals", "esp_sound_size", std::to_string(config.esp_sound_size).c_str());
	save_value("visuals", "esp_name", config.esp_name ? "true" : "false");
	save_value("visuals", "esp_name_pos", std::to_string(config.esp_name_pos).c_str());
	save_value("visuals", "esp_weapon", config.esp_weapon ? "true" : "false");
	save_value("visuals", "esp_weapon_icon", config.esp_weapon_icon ? "true" : "false");
	save_value("visuals", "esp_weapon_pos", std::to_string(config.esp_weapon_pos).c_str());
	save_value("visuals", "esp_line", config.esp_line ? "true" : "false");
	save_value("visuals", "esp_danger_zone_enable", config.esp_danger_zone_enable ? "true" : "false");
	save_value("visuals", "esp_danger_zone_box", config.esp_danger_zone_box ? "true" : "false");
	save_value("visuals", "esp_danger_zone_distance", std::to_string(config.esp_danger_zone_distance).c_str());
	for (int i = 0; i < 18; i++) {
		save_value("visuals", std::string("esp_danger_zone_" + std::to_string(i)).c_str(), config.esp_danger_zone[i] ? "true" : "false");
	}
	save_value("visuals", "esp_world_weapon_enable", config.esp_world_weapon_enable ? "true" : "false");
	save_value("visuals", "esp_world_weapon_weapons", config.esp_world_weapon_weapons ? "true" : "false");
	save_value("visuals", "esp_world_weapon_planted_bomb", config.esp_world_weapon_planted_bomb ? "true" : "false");
	save_value("visuals", "esp_world_weapon_box", config.esp_world_weapon_box ? "true" : "false");
	save_value("visuals", "chams_enable", config.chams_enable ? "true" : "false");
	save_value("visuals", "chams_enemy_only", config.chams_enemy_only ? "true" : "false");
	save_value("visuals", "chams_invisible", config.chams_invisible ? "true" : "false");
	save_value("visuals", "chams_wireframe", config.chams_wireframe ? "true" : "false");
	save_value("visuals", "chams_pearlescent_value", std::to_string(config.chams_pearlescent_value).c_str());
	save_value("visuals", "chams_brightness", std::to_string(config.chams_brightness).c_str());
	save_value("visuals", "chams_type", std::to_string(config.chams_type).c_str());
	save_value("visuals", "chams_more", config.chams_more ? "true" : "false");
	save_value("visuals", "chams_more_type", std::to_string(config.chams_more_type).c_str());
	save_value("visuals", "chams_arms", config.chams_arms ? "true" : "false");
	save_value("visuals", "chams_arms_wireframe", config.chams_arms_wireframe ? "true" : "false");
	save_value("visuals", "chams_arms_pearlescent_value", std::to_string(config.chams_arms_pearlescent_value).c_str());
	save_value("visuals", "chams_arms_type", std::to_string(config.chams_arms_type).c_str());
	save_value("visuals", "chams_arms_more_type", std::to_string(config.chams_arms_more_type).c_str());
	save_value("visuals", "chams_weapon", config.chams_weapon ? "true" : "false");
	save_value("visuals", "chams_weapon_wireframe", config.chams_weapon_wireframe ? "true" : "false");
	save_value("visuals", "chams_weapon_pearlescent_value", std::to_string(config.chams_weapon_pearlescent_value).c_str());
	save_value("visuals", "chams_weapon_type", std::to_string(config.chams_weapon_type).c_str());
	save_value("visuals", "chams_weapon_more_type", std::to_string(config.chams_weapon_more_type).c_str());
	save_value("visuals", "glow_enable", config.glow_enable ? "true" : "false");
	save_value("visuals", "glow_enemy_only", config.glow_enemy_only ? "true" : "false");
	save_value("visuals", "glow_invisible", config.glow_invisible ? "true" : "false");
	save_value("visuals", "glow_style", std::to_string(config.glow_style).c_str());
	save_value("visuals", "glow_world_enable", config.glow_world_enable ? "true" : "false");
	save_value("visuals", "glow_defuses", config.glow_defuses ? "true" : "false");
	save_value("visuals", "glow_chicken", config.glow_chicken ? "true" : "false");
	save_value("visuals", "glow_bomb", config.glow_bomb ? "true" : "false");
	save_value("visuals", "glow_weapons", config.glow_weapons ? "true" : "false");
	save_value("visuals", "visuals_dynamic_lights", config.visuals_dynamic_lights ? "true" : "false");
	save_value("visuals", "visuals_spread_crosshair", config.visuals_spread_crosshair ? "true" : "false");
	save_value("visuals", "visuals_taser_range", config.visuals_taser_range ? "true" : "false");
	save_value("visuals", "visuals_draw_fov", config.visuals_draw_fov ? "true" : "false");
	save_value("visuals", "visuals_draw_psilent_fov", config.visuals_draw_psilent_fov ? "true" : "false");
	save_value("visuals", "visuals_grenade_prediction", config.visuals_grenade_prediction ? "true" : "false");
	save_value("visuals", "visuals_hit_effect", config.visuals_hit_effect ? "true" : "false");
	save_value("visuals", "visuals_bullet_tracers", config.visuals_bullet_tracers ? "true" : "false");
	save_value("visuals", "visuals_bullet_tracers_life", std::to_string(config.visuals_bullet_tracers_life).c_str());
	save_value("visuals", "visuals_bullet_tracers_width", std::to_string(config.visuals_bullet_tracers_width).c_str());
	save_value("visuals", "visuals_damage_indicator", config.visuals_damage_indicator ? "true" : "false");
	save_value("visuals", "visuals_damage_indicator_speed", std::to_string(config.visuals_damage_indicator_speed).c_str());
	save_value("visuals", "visuals_hit_marker", config.visuals_hit_marker ? "true" : "false");
	save_value("visuals", "visuals_hit_sound", std::to_string(config.visuals_hit_sound).c_str());
	save_value("visuals", "visuals_sniper_crosshair", config.visuals_sniper_crosshair ? "true" : "false");
	save_value("visuals", "visuals_hit_hitbox", config.visuals_hit_hitbox ? "true" : "false");
	save_value("visuals", "visuals_enemy_circle", config.visuals_enemy_circle ? "true" : "false");
	save_value("visuals", "visuals_bomb_timer", config.visuals_bomb_timer ? "true" : "false");

	save_value("radar", "radar_enable", config.radar_enable ? "true" : "false");
	save_value("radar", "radar_enemy_only", config.radar_enemy_only ? "true" : "false");
	save_value("radar", "radar_invisible", config.radar_invisible ? "true" : "false");
	save_value("radar", "radar_texture", config.radar_texture ? "true" : "false");
	save_value("radar", "radar_grenades", config.radar_grenades ? "true" : "false");
	save_value("radar", "radar_range", std::to_string(config.radar_range).c_str());
	save_value("radar", "radar_size", std::to_string(config.radar_size).c_str());

	for (int i = 0; i < 3; i++) {
		save_value("colors", std::string("color_esp_ct_invisible_" + std::to_string(i)).c_str(), std::to_string(config.color_esp_ct_invisible[i]).c_str());
		save_value("colors", std::string("color_esp_ct_visible_" + std::to_string(i)).c_str(), std::to_string(config.color_esp_ct_visible[i]).c_str());
		save_value("colors", std::string("color_esp_t_invisible_" + std::to_string(i)).c_str(), std::to_string(config.color_esp_t_invisible[i]).c_str());
		save_value("colors", std::string("color_esp_t_visible_" + std::to_string(i)).c_str(), std::to_string(config.color_esp_t_visible[i]).c_str());
		save_value("colors", std::string("color_esp_name_" + std::to_string(i)).c_str(), std::to_string(config.color_esp_name[i]).c_str());
		save_value("colors", std::string("color_esp_weapon_" + std::to_string(i)).c_str(), std::to_string(config.color_esp_weapon[i]).c_str());
		save_value("colors", std::string("color_esp_line_" + std::to_string(i)).c_str(), std::to_string(config.color_esp_line[i]).c_str())
		save_value("colors", std::string("color_esp_sound_" + std::to_string(i)).c_str(), std::to_string(config.color_esp_sound[i]).c_str());
		save_value("colors", std::string("color_chams_ct_invisible_" + std::to_string(i)).c_str(), std::to_string(config.color_chams_ct_invisible[i]).c_str());
		save_value("colors", std::string("color_chams_ct_visible_" + std::to_string(i)).c_str(), std::to_string(config.color_chams_ct_visible[i]).c_str());
		save_value("colors", std::string("color_chams_t_invisible_" + std::to_string(i)).c_str(), std::to_string(config.color_chams_t_invisible[i]).c_str());
		save_value("colors", std::string("color_chams_t_visible_" + std::to_string(i)).c_str(), std::to_string(config.color_chams_t_visible[i]).c_str());
		save_value("colors", std::string("color_chams_more_ct_" + std::to_string(i)).c_str(), std::to_string(config.color_chams_more_ct[i]).c_str());
		save_value("colors", std::string("color_chams_more_t_" + std::to_string(i)).c_str(), std::to_string(config.color_chams_more_t[i]).c_str());
		save_value("colors", std::string("color_chams_arms_" + std::to_string(i)).c_str(), std::to_string(config.color_chams_arms[i]).c_str());
		save_value("colors", std::string("color_chams_weapon_" + std::to_string(i)).c_str(), std::to_string(config.color_chams_weapon[i]).c_str());
		save_value("colors", std::string("color_chams_arms_more_" + std::to_string(i)).c_str(), std::to_string(config.color_chams_arms_more[i]).c_str());
		save_value("colors", std::string("color_chams_weapon_more_" + std::to_string(i)).c_str(), std::to_string(config.color_chams_weapon_more[i]).c_str());
		save_value("colors", std::string("color_color_chams_arms_t_" + std::to_string(i)).c_str(), std::to_string(config.color_chams_arms[i]).c_str());
		save_value("colors", std::string("color_glow_ct_invisible_" + std::to_string(i)).c_str(), std::to_string(config.color_glow_ct_invisible[i]).c_str());
		save_value("colors", std::string("color_glow_ct_visible_" + std::to_string(i)).c_str(), std::to_string(config.color_glow_ct_visible[i]).c_str());
		save_value("colors", std::string("color_glow_t_invisible_" + std::to_string(i)).c_str(), std::to_string(config.color_glow_t_invisible[i]).c_str());
		save_value("colors", std::string("color_glow_t_visible_" + std::to_string(i)).c_str(), std::to_string(config.color_glow_t_visible[i]).c_str());
		save_value("colors", std::string("color_glow_defuses_" + std::to_string(i)).c_str(), std::to_string(config.color_glow_defuses[i]).c_str());
		save_value("colors", std::string("color_glow_chicken_" + std::to_string(i)).c_str(), std::to_string(config.color_glow_chicken[i]).c_str());
		save_value("colors", std::string("color_glow_bomb_" + std::to_string(i)).c_str(), std::to_string(config.color_glow_bomb[i]).c_str());
		save_value("colors", std::string("color_glow_weapons_" + std::to_string(i)).c_str(), std::to_string(config.color_glow_weapons[i]).c_str());
		save_value("colors", std::string("color_visuals_dynamic_lights_" + std::to_string(i)).c_str(), std::to_string(config.color_visuals_dynamic_lights[i]).c_str());
		save_value("colors", std::string("color_visuals_spread_crosshair_" + std::to_string(i)).c_str(), std::to_string(config.color_visuals_spread_crosshair[i]).c_str());
		save_value("colors", std::string("color_visuals_grenade_prediction_" + std::to_string(i)).c_str(), std::to_string(config.color_visuals_grenade_prediction[i]).c_str());
		save_value("colors", std::string("color_fog_" + std::to_string(i)).c_str(), std::to_string(config.color_fog[i]).c_str());
		save_value("colors", std::string("color_radar_ct_invisible_" + std::to_string(i)).c_str(), std::to_string(config.color_radar_ct_invisible[i]).c_str());
		save_value("colors", std::string("color_radar_ct_visible_" + std::to_string(i)).c_str(), std::to_string(config.color_radar_ct_visible[i]).c_str());
		save_value("colors", std::string("color_radar_t_invisible_" + std::to_string(i)).c_str(), std::to_string(config.color_radar_t_invisible[i]).c_str());
		save_value("colors", std::string("color_radar_t_visible_" + std::to_string(i)).c_str(), std::to_string(config.color_radar_t_visible[i]).c_str());
		save_value("colors", std::string("color_radar_grenades_" + std::to_string(i)).c_str(), std::to_string(config.color_radar_grenades[i]).c_str());
		save_value("colors", std::string("color_visuals_bullet_tracers_" + std::to_string(i)).c_str(), std::to_string(config.color_visuals_bullet_tracers[i]).c_str());
		save_value("colors", std::string("color_visuals_damage_indicator_" + std::to_string(i)).c_str(), std::to_string(config.color_visuals_damage_indicator[i]).c_str());
		save_value("colors", std::string("color_esp_danger_zone_" + std::to_string(i)).c_str(), std::to_string(config.color_esp_danger_zone[i]).c_str());
		save_value("colors", std::string("color_esp_world_weapon_" + std::to_string(i)).c_str(), std::to_string(config.color_esp_world_weapon[i]).c_str());
		save_value("colors", std::string("color_visuals_hit_hitbox_" + std::to_string(i)).c_str(), std::to_string(config.color_visuals_hit_hitbox[i]).c_str());
		save_value("colors", std::string("color_misc_static_props_" + std::to_string(i)).c_str(), std::to_string(config.color_misc_static_props[i]).c_str());
		save_value("colors", std::string("color_misc_world_" + std::to_string(i)).c_str(), std::to_string(config.color_misc_world[i]).c_str());
	}

	save_value("misc", "misc_bhop", config.misc_bhop ? "true" : "false");
	save_value("misc", "misc_auto_strafe", config.misc_auto_strafe ? "true" : "false");
	save_value("misc", "misc_fast_ladder", config.misc_fast_ladder ? "true" : "false");
	save_value("misc", "misc_mini_jump", config.misc_mini_jump ? "true" : "false");
	save_value("misc", "misc_rank_reveal", config.misc_rank_reveal ? "true" : "false");
	save_value("misc", "misc_money_reveal", config.misc_money_reveal ? "true" : "false");
	save_value("misc", "misc_aspect_ratio", config.misc_aspect_ratio ? "true" : "false");
	save_value("misc", "misc_aspect_ratio_value", std::to_string(config.misc_aspect_ratio_value).c_str());
	save_value("misc", "misc_no_scope_border", config.misc_no_scope_border ? "true" : "false");
	save_value("misc", "misc_no_flash_effect", config.misc_no_flash_effect ? "true" : "false");
	save_value("misc", "misc_no_smoke", config.misc_no_smoke ? "true" : "false");
	save_value("misc", "misc_no_visual_recoil", config.misc_no_visual_recoil ? "true" : "false");
	save_value("misc", "misc_fov_changer", config.misc_fov_changer ? "true" : "false");
	save_value("misc", "misc_fov_viewmodel", std::to_string(config.misc_fov_viewmodel).c_str());
	save_value("misc", "misc_fov_offset", config.misc_fov_offset ? "true" : "false");
	save_value("misc", "misc_fov_viewmodel_x", std::to_string(config.misc_fov_viewmodel_x).c_str());
	save_value("misc", "misc_fov_viewmodel_y", std::to_string(config.misc_fov_viewmodel_y).c_str());
	save_value("misc", "misc_fov_viewmodel_z", std::to_string(config.misc_fov_viewmodel_z).c_str());
	save_value("misc", "misc_fov_override", std::to_string(config.misc_fov_override).c_str());
	save_value("misc", "misc_night_mode_enable", config.misc_night_mode_enable ? "true" : "false");
	save_value("misc", "misc_ragdoll", config.misc_ragdoll ? "true" : "false");
	save_value("misc", "misc_ragdoll_value", std::to_string(config.misc_ragdoll_value).c_str());
	save_value("misc", "misc_auto_defuse", config.misc_auto_defuse ? "true" : "false");
	save_value("misc", "misc_overwatch_reveal", config.misc_overwatch_reveal ? "true" : "false");
	save_value("misc", "misc_asus_walls_enable", config.misc_asus_walls_enable ? "true" : "false");
	save_value("misc", "misc_asus_walls_value", std::to_string(config.misc_asus_walls_value).c_str());
	save_value("misc", "misc_edge_jump", config.misc_edge_jump ? "true" : "false");
	save_value("misc", "misc_night_mode_value", std::to_string(config.misc_night_mode_value).c_str());
	save_value("misc", "misc_bloom_effect_value", std::to_string(config.misc_bloom_effect_value).c_str());
	save_value("misc", "misc_model_ambient_value", std::to_string(config.misc_model_ambient_value).c_str());
	save_value("misc", "misc_auto_accept", config.misc_auto_accept ? "true" : "false");
	save_value("misc", "misc_inventory_always", config.misc_inventory_always ? "true" : "false");
	save_value("misc", "misc_fog_enable", config.misc_fog_enable ? "true" : "false");
	save_value("misc", "misc_fog_blend", config.misc_fog_blend ? "true" : "false");
	save_value("misc", "misc_fog_start", std::to_string(config.misc_fog_start).c_str());
	save_value("misc", "misc_fog_end", std::to_string(config.misc_fog_end).c_str());
	save_value("misc", "misc_fog_density", std::to_string(config.misc_fog_density).c_str());
	save_value("misc", "misc_radar", config.misc_radar ? "true" : "false");
	save_value("misc", "misc_clan_tag", config.misc_clan_tag ? "true" : "false");
	save_value("misc", "misc_clan_tag_type", std::to_string(config.misc_clan_tag_type).c_str());
	save_value("misc", "misc_clan_tag_name", config.misc_clan_tag_name.c_str());
	save_value("misc", "misc_name_stealer", config.misc_name_stealer ? "true" : "false");
	save_value("misc", "misc_knife_bot", config.misc_knife_bot ? "true" : "false");
	save_value("misc", "misc_spectators", config.misc_spectators ? "true" : "false");
	//save_value("misc", "misc_russian", config.misc_russian ? "true" : "false");
	save_value("misc", "misc_fake_latency", config.misc_fake_latency ? "true" : "false");
	save_value("misc", "misc_fake_latency_value", std::to_string(config.misc_fake_latency_value).c_str());
	save_value("misc", "misc_third_person_value", std::to_string(config.misc_third_person_value).c_str());
	save_value("misc", "misc_third_person_key", std::to_string(config.misc_third_person_key).c_str());
	save_value("misc", "misc_third_person_key_mode", std::to_string(config.misc_third_person_key_mode).c_str());
	save_value("misc", "misc_disable_post_processing", config.misc_disable_post_processing ? "true" : "false");
	save_value("misc", "misc_thunder_enable", config.misc_thunder_enable ? "true" : "false");
	save_value("misc", "misc_world_custom_color_enable", config.misc_world_custom_color_enable ? "true" : "false");

	save_value("profile", "profile_friendly", std::to_string(config.profile_friendly).c_str());
	save_value("profile", "profile_leader", std::to_string(config.profile_leader).c_str());
	save_value("profile", "profile_player_ban", std::to_string(config.profile_player_ban).c_str());
	save_value("profile", "profile_player_ban_time", std::to_string(config.profile_player_ban_time).c_str());
	save_value("profile", "profile_player_level", std::to_string(config.profile_player_level).c_str());
	save_value("profile", "profile_player_level_xp", std::to_string(config.profile_player_level_xp).c_str());
	save_value("profile", "profile_rank_id", std::to_string(config.profile_rank_id).c_str());
	save_value("profile", "profile_rank_id_dz", std::to_string(config.profile_rank_id_dz).c_str());
	save_value("profile", "profile_rank_id_wingmans", std::to_string(config.profile_rank_id_wingmans).c_str());
	save_value("profile", "profile_teaching", std::to_string(config.profile_teaching).c_str());
	save_value("profile", "profile_wins", std::to_string(config.profile_wins).c_str());
	save_value("profile", "profile_wins_dz", std::to_string(config.profile_wins_dz).c_str());
	save_value("profile", "profile_wins_wingmans", std::to_string(config.profile_wins_wingmans).c_str());

	save_value("inventory", "count", std::to_string(config.inventory_item_list.size()).c_str());
	size_t i = 0;

	for (const auto& item : config.inventory_item_list) {
		static int in_use_ct;
		static int in_use_t;
		static int in_use_medal;
		static int in_use_music;

		const auto slot_name = "inventory_" + std::to_string(i);
		const auto equipped_position = weaponids.m_list[item.second.definition_index]->get_loadout_slot();

		const auto t_item_in_loadout = g_player_inventory->get_item_in_loadout(2, equipped_position);
		if (t_item_in_loadout) {
			const auto soc_data = t_item_in_loadout->GetSOCData();

			if (soc_data)
				in_use_t = *soc_data->GetItemID() == item.first;
		}

		const auto ct_item_in_loadout = g_player_inventory->get_item_in_loadout(3, equipped_position);
		if (ct_item_in_loadout) {
			const auto soc_data = ct_item_in_loadout->GetSOCData();

			if (soc_data)
				in_use_ct = *soc_data->GetItemID() == item.first;
		}

		const auto medal_item_in_loadout = g_player_inventory->get_item_in_loadout(0, 55);
		if (medal_item_in_loadout) {
			const auto soc_data = medal_item_in_loadout->GetSOCData();

			if (soc_data)
				in_use_medal = *soc_data->GetItemID() == item.first;
		}

		const auto music_item_in_loadout = g_player_inventory->get_item_in_loadout(0, 54);
		if (music_item_in_loadout) {
			const auto soc_data = music_item_in_loadout->GetSOCData();

			if (soc_data)
				in_use_music = *soc_data->GetItemID() == item.first;
		}

		save_value("inventory", std::string(slot_name + "_definition_index").c_str(), std::to_string(item.second.definition_index).c_str());
		save_value("inventory", std::string(slot_name + "_paint_kit").c_str(), std::to_string(item.second.paint_kit).c_str());
		save_value("inventory", std::string(slot_name + "_wear").c_str(), std::to_string(item.second.wear).c_str());
		save_value("inventory", std::string(slot_name + "_seed").c_str(), std::to_string(item.second.seed).c_str());
		save_value("inventory", std::string(slot_name + "_stattrak").c_str(), std::to_string(item.second.stattrak).c_str());
		save_value("inventory", std::string(slot_name + "_in_use_t").c_str(), std::to_string(in_use_t).c_str());
		save_value("inventory", std::string(slot_name + "_in_use_ct").c_str(), std::to_string(in_use_ct).c_str());
		save_value("inventory", std::string(slot_name + "_in_use_medal").c_str(), std::to_string(in_use_medal).c_str());
		save_value("inventory", std::string(slot_name + "_in_use_music").c_str(), std::to_string(in_use_music).c_str());

		for (size_t j = 0; j < 5; j++) {

			if (!item.second.stickers[j].kit)
				continue;

			const auto sticker_slot_name = slot_name + "_sticker_slot_" + std::to_string(j);

			save_value("inventory", std::string(sticker_slot_name + "_kit").c_str(), std::to_string(item.second.stickers[j].kit).c_str());
			save_value("inventory", std::string(sticker_slot_name + "_wear").c_str(), std::to_string(item.second.stickers[j].wear).c_str());
			save_value("inventory", std::string(sticker_slot_name + "_rotation").c_str(), std::to_string(item.second.stickers[j].rotation).c_str());
			save_value("inventory", std::string(sticker_slot_name + "_scale").c_str(), std::to_string(item.second.stickers[j].scale).c_str());
		}

		i++;
	}

	save_value("settings", "settings_obs_spoof", config.settings_obs_spoof ? "true" : "false");
}

void config_manager::load(std::string config_file) {
	std::string file = XORSTR("C:/shark-hack/configs/") + config_file;
	char value_l[32] = { '\0' };
	/*
	for (int type = 0; type < 7; type++) {
		for (int i = 0; i < 8; i++) {
			load_value("rage", std::string("rage_hitscan_" + std::to_string(type) + "_" + std::to_string(i)).c_str(), config.ragebot_for_group[type].rage_hitscan[i], !strcmp, "true");
		}
		load_value("rage", std::string("rage_autowall_damage_min_damage_" + std::to_string(type)).c_str(), config.ragebot_for_group[type].rage_autowall_damage_min_damage, atoi);
		load_value("rage", std::string("rage_visible_damage_min_damage_" + std::to_string(type)).c_str(), config.ragebot_for_group[type].rage_visible_damage_min_damage, atoi);
		load_value("rage", std::string("rage_hitchance_" + std::to_string(type)).c_str(), config.ragebot_for_group[type].rage_hitchance, atoi);
		load_value("rage", std::string("rage_head_scale_" + std::to_string(type)).c_str(), config.ragebot_for_group[type].rage_head_scale, (float)atof);
		load_value("rage", std::string("rage_body_scale_" + std::to_string(type)).c_str(), config.ragebot_for_group[type].rage_body_scale, (float)atof);
	}
	*/

	for (int type = 0; type < config.m_aRageSettings.size(); type++) {

		load_value("rage", std::string("m_iFOV_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_iFOV, atoi);
		load_value("rage", std::string("m_iHitChance_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_iHitChance, atoi);
		load_value("rage", std::string("m_iDoubleTapHitChance_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_iDoubleTapHitChance, atoi);
		load_value("rage", std::string("m_iMinDamage_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_iMinDamage, atoi);
		load_value("rage", std::string("m_iMinDamageOverride_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_iMinDamageOverride, atoi);
		load_value("rage", std::string("m_iAccuracyBoost_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_iAccuracyBoost, atoi);
		load_value("rage", std::string("m_iHeadScale_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_iHeadScale, atoi);
		load_value("rage", std::string("m_iBodyScale_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_iBodyScale, atoi);
		load_value("rage", std::string("m_bPreferBody_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_bPreferBody, !strcmp, "true");
		load_value("rage", std::string("m_bAutoScope_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_bAutoScope, !strcmp, "true");
		load_value("rage", std::string("m_bAutoStop_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_bAutoStop, !strcmp, "true");
		load_value("rage", std::string("m_bEnabled_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_bEnabled, !strcmp, "true");
		load_value("rage", std::string("m_bLethalSafety_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_bLethalSafety, !strcmp, "true");
		load_value("rage", std::string("m_bPreferSafe_" + std::to_string(type)).c_str(), config.m_aRageSettings[type].m_bPreferSafe, !strcmp, "true");

		for (int i = 0; i < 6; i++) {
			load_value("rage", std::string("m_Hitboxes_" + std::to_string(type) + "_" + std::to_string(i)).c_str(), config.m_aRageSettings[type].m_Hitboxes[i], !strcmp, "true");
			load_value("rage", std::string("m_SafeHitboxes_" + std::to_string(type) + "_" + std::to_string(i)).c_str(), config.m_aRageSettings[type].m_SafeHitboxes[i], !strcmp, "true");
			load_value("rage", std::string("m_Multipoints_" + std::to_string(type) + "_" + std::to_string(i)).c_str(), config.m_aRageSettings[type].m_Multipoints[i], !strcmp, "true");
		}

		for (int i = 0; i < 2; i++) {
			load_value("rage", std::string("m_AutoStopOptions_" + std::to_string(type) + "_" + std::to_string(i)).c_str(), config.m_aRageSettings[type].m_AutoStopOptions[i], !strcmp, "true");
		}

		for (int i = 0; i < 2; i++) {
			load_value("rage", std::string("m_DoubleTapOptions_" + std::to_string(type) + "_" + std::to_string(i)).c_str(), config.m_aRageSettings[type].m_DoubleTapOptions[i], !strcmp, "true");
		}
	}

	load_value("rage", "m_bFakeDuck", config.m_bFakeDuck, !strcmp, "true");
	load_value("rage", "m_bAutoDirection", config.m_bAutoDirection, !strcmp, "true");
	load_value("rage", "m_bJitterMove", config.m_bJitterMove, !strcmp, "true");
	load_value("rage", "m_bSlowWalk", config.m_bSlowWalk, !strcmp, "true");
	load_value("rage", "m_bFakeLagEnabled", config.m_bFakeLagEnabled, !strcmp, "true");
	load_value("rage", "m_bAntiAim", config.m_bAntiAim, !strcmp, "true");
	load_value("rage", "m_bAntiAimAtTargets", config.m_bAntiAimAtTargets, !strcmp, "true");
	load_value("rage", "m_iLeftFakeLimit", config.m_iLeftFakeLimit, atoi);
	load_value("rage", "m_iLagLimit", config.m_iLagLimit, atoi);
	load_value("rage", "m_iTriggerLimit", config.m_iTriggerLimit, atoi);
	load_value("rage", "m_iRightFakeLimit", config.m_iRightFakeLimit, atoi);
	load_value("rage", "m_iJitterAmount", config.m_iJitterAmount, atoi);
	load_value("rage", "m_iYawAddOffset", config.m_iYawAddOffset, atoi);
	load_value("rage", "m_iPitchMode", config.m_iPitchMode, atoi);
	load_value("rage", "m_iLegMovement", config.m_iLegMovement, atoi);

	load_value("rage", "hide_shots_key", config.hide_shots_key, atoi);
	load_value("rage", "hide_shots_key_mode", config.hide_shots_key_mode, atoi);
	load_value("rage", "double_tap_key", config.double_tap_key, atoi);
	load_value("rage", "double_tap_key_mode", config.double_tap_key_mode, atoi);
	load_value("rage", "safe_point_key", config.safe_point_key, atoi);
	load_value("rage", "safe_point_key_mode", config.safe_point_key_mode, atoi);
	load_value("rage", "manual_back_key", config.manual_back_key, atoi);
	load_value("rage", "manual_left_key", config.manual_left_key, atoi);
	load_value("rage", "manual_right_key", config.manual_right_key, atoi);
	load_value("rage", "inverter_key", config.inverter_key, atoi);
	load_value("rage", "inverter_key_mode", config.inverter_key_mode, atoi);
	load_value("rage", "m_bOverrideDamage_key", config.m_bOverrideDamage_key, atoi);
	load_value("rage", "m_bOverrideDamage_key_mode", config.m_bOverrideDamage_key_mode, atoi);

	load_value("legit", "legit_enable", config.legit_enable, !strcmp, "true");
	load_value("legit", "legit_onlyscope", config.legit_onlyscope, !strcmp, "true");
	load_value("legit", "legit_resize_fov", config.legit_resize_fov, !strcmp, "true");
	load_value("legit", "legit_autoselectweapon", config.legit_autoselectweapon, !strcmp, "true");
	load_value("legit", "legit_desync", config.legit_desync, !strcmp, "true");
	load_value("legit", "legit_desync_key", config.legit_desync_key, atoi);
	load_value("legit", "legit_backtrack", config.legit_backtrack, !strcmp, "true");
	load_value("legit", "legit_backtrack_time", config.legit_backtrack_time, atoi);
	load_value("legit", "legit_backtrack_chams_enable", config.legit_backtrack_chams_enable, !strcmp, "true");
	load_value("legit", "legit_backtrack_chams", config.legit_backtrack_chams, atoi);

	for (size_t i = 0; i < (sizeof(legitbot::weapon_data) / sizeof(*legitbot::weapon_data)); i++) {
		load_value(legitbot::weapon_data[i], "legit_nearest", config.legitbot_for_weapon[i].legit_nearest, !strcmp, "true");
		load_value(legitbot::weapon_data[i], "legit_psilent_enable", config.legitbot_for_weapon[i].legit_psilent_enable, !strcmp, "true");
		load_value(legitbot::weapon_data[i], "legit_teamattack", config.legitbot_for_weapon[i].legit_teamattack, !strcmp, "true");
		load_value(legitbot::weapon_data[i], "legit_ignoresmoke", config.legitbot_for_weapon[i].legit_ignoresmoke, !strcmp, "true");
		load_value(legitbot::weapon_data[i], "legit_ignoreflash", config.legitbot_for_weapon[i].legit_ignoreflash, !strcmp, "true");
		load_value(legitbot::weapon_data[i], "legit_ignoreflash", config.legitbot_for_weapon[i].legit_ignorejump, !strcmp, "true");
		load_value(legitbot::weapon_data[i], "legit_aimlock", config.legitbot_for_weapon[i].legit_aimlock, !strcmp, "true");
		load_value(legitbot::weapon_data[i], "legit_autofire", config.legitbot_for_weapon[i].legit_autofire, !strcmp, "true");
		load_value(legitbot::weapon_data[i], "legit_dynamic_fov", config.legitbot_for_weapon[i].legit_dynamic_fov, !strcmp, "true");
		load_value(legitbot::weapon_data[i], "legit_autowall", config.legitbot_for_weapon[i].legit_autowall, !strcmp, "true");
		load_value(legitbot::weapon_data[i], "legit_fov_after_enable", config.legitbot_for_weapon[i].legit_fov_after_enable, !strcmp, "true");
		load_value(legitbot::weapon_data[i], "legit_smooth_after_enable", config.legitbot_for_weapon[i].legit_smooth_after_enable, !strcmp, "true");

		load_value(legitbot::weapon_data[i], "legit_hitbox", config.legitbot_for_weapon[i].legit_hitbox, atoi);
		load_value(legitbot::weapon_data[i], "legit_rcs_x", config.legitbot_for_weapon[i].legit_rcs_x, atoi);
		load_value(legitbot::weapon_data[i], "legit_rcs_y", config.legitbot_for_weapon[i].legit_rcs_y, atoi);
		load_value(legitbot::weapon_data[i], "legit_psilent_type", config.legitbot_for_weapon[i].legit_psilent_type, atoi);

		load_value(legitbot::weapon_data[i], "legit_fov", config.legitbot_for_weapon[i].legit_fov, (float)atof);
		load_value(legitbot::weapon_data[i], "legit_smooth", config.legitbot_for_weapon[i].legit_smooth, (float)atof);
		load_value(legitbot::weapon_data[i], "legit_fov_after", config.legitbot_for_weapon[i].legit_fov_after, (float)atof);
		load_value(legitbot::weapon_data[i], "legit_smooth_after", config.legitbot_for_weapon[i].legit_smooth_after, (float)atof);
		load_value(legitbot::weapon_data[i], "legit_psilent_fov", config.legitbot_for_weapon[i].legit_psilent_fov, (float)atof);
		load_value(legitbot::weapon_data[i], "legit_delay_after_kill", config.legitbot_for_weapon[i].legit_delay_after_kill, (float)atof);
		load_value(legitbot::weapon_data[i], "legit_delay_shot", config.legitbot_for_weapon[i].legit_delay_shot, (float)atof);
		load_value(legitbot::weapon_data[i], "legit_mindamage", config.legitbot_for_weapon[i].legit_mindamage, (float)atof);
	}

	load_value("visuals", "esp_enable", config.esp_enable, !strcmp, "true");
	load_value("visuals", "esp_enemy_only", config.esp_enemy_only, !strcmp, "true");
	load_value("visuals", "esp_invisible", config.esp_invisible, !strcmp, "true");
	load_value("visuals", "esp_box", config.esp_box, !strcmp, "true");
	load_value("visuals", "esp_box_filled", config.esp_box_filled, !strcmp, "true");
	load_value("visuals", "esp_box_outline", config.esp_box_outline, !strcmp, "true");
	load_value("visuals", "esp_box_filled_fade", config.esp_box_filled_fade, !strcmp, "true");
	load_value("visuals", "esp_box_filled_alpha",config.esp_box_filled_alpha, atoi);
	load_value("visuals", "esp_box_rounding", config.esp_box_rounding, atoi);
	load_value("visuals", "esp_box_type", config.esp_box_type, atoi);
	load_value("visuals", "esp_health", config.esp_health, !strcmp, "true");
	load_value("visuals", "esp_health_fade", config.esp_health_fade, !strcmp, "true");
	load_value("visuals", "esp_health_background", config.esp_health_background, !strcmp, "true");
	load_value("visuals", "esp_health_pos", config.esp_health_pos, atoi);
	load_value("visuals", "esp_armor", config.esp_armor, !strcmp, "true");
	load_value("visuals", "esp_armor_fade", config.esp_armor_fade, !strcmp, "true");
	load_value("visuals", "esp_armor_background", config.esp_armor_background, !strcmp, "true");
	load_value("visuals", "esp_armor_pos", config.esp_armor_pos, atoi);
	load_value("visuals", "esp_sound_enable", config.esp_sound_enable, !strcmp, "true");
	load_value("visuals", "esp_sound_size", config.esp_sound_size, (float)atof);
	load_value("visuals", "esp_name", config.esp_name, !strcmp, "true");
	load_value("visuals", "esp_name_pos", config.esp_name_pos, atoi);
	load_value("visuals", "esp_weapon", config.esp_weapon, !strcmp, "true");
	load_value("visuals", "esp_weapon_icon", config.esp_weapon_icon, !strcmp, "true");
	load_value("visuals", "esp_weapon_pos", config.esp_weapon_pos, atoi);
	load_value("visuals", "esp_line", config.esp_line, !strcmp, "true");
	load_value("visuals", "esp_danger_zone_enable", config.esp_danger_zone_enable, !strcmp, "true");
	load_value("visuals", "esp_danger_zone_box", config.esp_danger_zone_box, !strcmp, "true");
	load_value("visuals", "esp_danger_zone_distance", config.esp_danger_zone_distance, (float)atof);
	for (int i = 0; i < 18; i++) {
		load_value("visuals", std::string("esp_danger_zone_" + std::to_string(i)).c_str(), config.esp_danger_zone[i], !strcmp, "true");
	}
	load_value("visuals", "esp_world_weapon_enable", config.esp_world_weapon_enable, !strcmp, "true");
	load_value("visuals", "esp_world_weapon_weapons", config.esp_world_weapon_weapons, !strcmp, "true");
	load_value("visuals", "esp_world_weapon_planted_bomb", config.esp_world_weapon_planted_bomb, !strcmp, "true");
	load_value("visuals", "esp_world_weapon_box", config.esp_world_weapon_box, !strcmp, "true");
	load_value("visuals", "chams_enable", config.chams_enable, !strcmp, "true");
	load_value("visuals", "chams_enemy_only", config.chams_enemy_only, !strcmp, "true");
	load_value("visuals", "chams_invisible", config.chams_invisible, !strcmp, "true");
	load_value("visuals", "chams_wireframe", config.chams_wireframe, !strcmp, "true");
	load_value("visuals", "chams_pearlescent_value", config.chams_pearlescent_value, (float)atof);
	load_value("visuals", "chams_brightness", config.chams_brightness, (float)atof);
	load_value("visuals", "chams_type", config.chams_type, atoi);
	load_value("visuals", "chams_more", config.chams_more, !strcmp, "true");
	load_value("visuals", "chams_more_type", config.chams_more_type, atoi);
	load_value("visuals", "chams_arms", config.chams_arms, !strcmp, "true");
	load_value("visuals", "chams_arms_wireframe", config.chams_arms_wireframe, !strcmp, "true");
	load_value("visuals", "chams_arms_pearlescent_value", config.chams_arms_pearlescent_value, (float)atof);
	load_value("visuals", "chams_arms_type", config.chams_arms_type, atoi);
	load_value("visuals", "chams_arms_more_type", config.chams_arms_more_type, atoi);
	load_value("visuals", "chams_weapon", config.chams_weapon, !strcmp, "true");
	load_value("visuals", "chams_weapon_wireframe", config.chams_weapon_wireframe, !strcmp, "true");
	load_value("visuals", "chams_weapon_pearlescent_value", config.chams_weapon_pearlescent_value, (float)atof);
	load_value("visuals", "chams_weapon_type", config.chams_weapon_type, atoi);
	load_value("visuals", "chams_weapon_more_type", config.chams_weapon_more_type, atoi);
	load_value("visuals", "glow_enable", config.glow_enable, !strcmp, "true");
	load_value("visuals", "glow_enemy_only", config.glow_enemy_only, !strcmp, "true");
	load_value("visuals", "glow_invisible", config.glow_invisible, !strcmp, "true");
	load_value("visuals", "glow_style", config.glow_style, atoi);
	load_value("visuals", "glow_world_enable", config.glow_world_enable, !strcmp, "true");
	load_value("visuals", "glow_defuses", config.glow_defuses, !strcmp, "true");
	load_value("visuals", "glow_chicken", config.glow_chicken, !strcmp, "true");
	load_value("visuals", "glow_bomb", config.glow_bomb, !strcmp, "true");
	load_value("visuals", "glow_weapons", config.glow_weapons, !strcmp, "true");
	load_value("visuals", "visuals_dynamic_lights", config.visuals_dynamic_lights, !strcmp, "true");
	load_value("visuals", "visuals_spread_crosshair", config.visuals_spread_crosshair, !strcmp, "true");
	load_value("visuals", "visuals_taser_range", config.visuals_taser_range, !strcmp, "true");
	load_value("visuals", "visuals_draw_fov", config.visuals_draw_fov, !strcmp, "true");
	load_value("visuals", "visuals_draw_psilent_fov", config.visuals_draw_psilent_fov, !strcmp, "true");
	load_value("visuals", "visuals_grenade_prediction", config.visuals_grenade_prediction, !strcmp, "true");
	load_value("visuals", "visuals_hit_effect", config.visuals_hit_effect, !strcmp, "true");
	load_value("visuals", "visuals_bullet_tracers", config.visuals_bullet_tracers, !strcmp, "true");
	load_value("visuals", "visuals_bullet_tracers_life", config.visuals_bullet_tracers_life, (float)atof);
	load_value("visuals", "visuals_bullet_tracers_width", config.visuals_bullet_tracers_width, (float)atof);
	load_value("visuals", "visuals_damage_indicator", config.visuals_damage_indicator, !strcmp, "true");
	load_value("visuals", "visuals_damage_indicator_speed", config.visuals_damage_indicator_speed, (float)atof);
	load_value("visuals", "visuals_hit_marker", config.visuals_hit_marker, !strcmp, "true");
	load_value("visuals", "visuals_hit_sound", config.visuals_hit_sound, atoi);
	load_value("visuals", "visuals_sniper_crosshair", config.visuals_sniper_crosshair, !strcmp, "true");
	load_value("visuals", "visuals_hit_hitbox", config.visuals_hit_hitbox, !strcmp, "true");
	load_value("visuals", "visuals_enemy_circle", config.visuals_enemy_circle, !strcmp, "true");
	load_value("visuals", "visuals_bomb_timer", config.visuals_bomb_timer, !strcmp, "true");

	load_value("radar", "radar_enable", config.radar_enable, !strcmp, "true");
	load_value("radar", "radar_enemy_only", config.radar_enemy_only, !strcmp, "true");
	load_value("radar", "radar_invisible", config.radar_invisible, !strcmp, "true");
	load_value("radar", "radar_texture", config.radar_texture, !strcmp, "true");
	load_value("radar", "radar_grenades", config.radar_grenades, !strcmp, "true");
	load_value("radar", "radar_range", config.radar_range, atoi);
	load_value("radar", "radar_size", config.radar_size, atoi);

	for (int i = 0; i < 3; i++) {
		load_value("colors", std::string("color_esp_ct_invisible_" + std::to_string(i)).c_str(), config.color_esp_ct_invisible[i], (float)atof);
		load_value("colors", std::string("color_esp_ct_visible_" + std::to_string(i)).c_str(), config.color_esp_ct_visible[i], (float)atof);
		load_value("colors", std::string("color_esp_t_invisible_" + std::to_string(i)).c_str(), config.color_esp_t_invisible[i], (float)atof);
		load_value("colors", std::string("color_esp_t_visible_" + std::to_string(i)).c_str(), config.color_esp_t_visible[i], (float)atof);
		load_value("colors", std::string("color_esp_name_" + std::to_string(i)).c_str(), config.color_esp_name[i], (float)atof);
		load_value("colors", std::string("color_esp_weapon_" + std::to_string(i)).c_str(), config.color_esp_weapon[i], (float)atof);
		load_value("colors", std::string("color_esp_line_" + std::to_string(i)).c_str(), config.color_esp_line[i], (float)atof);
		load_value("colors", std::string("color_esp_sound_" + std::to_string(i)).c_str(), config.color_esp_sound[i], (float)atof);
		load_value("colors", std::string("color_chams_ct_invisible_" + std::to_string(i)).c_str(), config.color_chams_ct_invisible[i], (float)atof);
		load_value("colors", std::string("color_chams_ct_visible_" + std::to_string(i)).c_str(), config.color_chams_ct_visible[i], (float)atof);
		load_value("colors", std::string("color_chams_t_invisible_" + std::to_string(i)).c_str(), config.color_chams_t_invisible[i], (float)atof);
		load_value("colors", std::string("color_chams_t_visible_" + std::to_string(i)).c_str(), config.color_chams_t_visible[i], (float)atof);
		load_value("colors", std::string("color_chams_more_ct_" + std::to_string(i)).c_str(), config.color_chams_more_ct[i], (float)atof);
		load_value("colors", std::string("color_chams_more_t_" + std::to_string(i)).c_str(), config.color_chams_more_t[i], (float)atof);
		load_value("colors", std::string("color_chams_arms_" + std::to_string(i)).c_str(), config.color_chams_arms[i], (float)atof);
		load_value("colors", std::string("color_chams_weapon_" + std::to_string(i)).c_str(), config.color_chams_weapon[i], (float)atof);
		load_value("colors", std::string("color_chams_arms_more_" + std::to_string(i)).c_str(), config.color_chams_arms_more[i], (float)atof);
		load_value("colors", std::string("color_chams_weapon_more_" + std::to_string(i)).c_str(), config.color_chams_weapon_more[i], (float)atof);
		load_value("colors", std::string("color_glow_ct_invisible_" + std::to_string(i)).c_str(), config.color_glow_ct_invisible[i], (float)atof);
		load_value("colors", std::string("color_glow_ct_visible_" + std::to_string(i)).c_str(), config.color_glow_ct_visible[i], (float)atof);
		load_value("colors", std::string("color_glow_t_invisible_" + std::to_string(i)).c_str(), config.color_glow_t_invisible[i], (float)atof);
		load_value("colors", std::string("color_glow_t_visible_" + std::to_string(i)).c_str(), config.color_glow_t_visible[i], (float)atof);
		load_value("colors", std::string("color_glow_defuses_" + std::to_string(i)).c_str(), config.color_glow_defuses[i], (float)atof);
		load_value("colors", std::string("color_glow_chicken_" + std::to_string(i)).c_str(), config.color_glow_chicken[i], (float)atof);
		load_value("colors", std::string("color_glow_bomb_" + std::to_string(i)).c_str(), config.color_glow_bomb[i], (float)atof);
		load_value("colors", std::string("color_glow_weapons_" + std::to_string(i)).c_str(), config.color_glow_weapons[i], (float)atof);
		load_value("colors", std::string("color_visuals_dynamic_lights_" + std::to_string(i)).c_str(), config.color_visuals_dynamic_lights[i], (float)atof);
		load_value("colors", std::string("color_visuals_spread_crosshair_" + std::to_string(i)).c_str(), config.color_visuals_spread_crosshair[i], (float)atof);
		load_value("colors", std::string("color_visuals_grenade_prediction_" + std::to_string(i)).c_str(), config.color_visuals_grenade_prediction[i], (float)atof);
		load_value("colors", std::string("color_fog_" + std::to_string(i)).c_str(), config.color_fog[i], (float)atof);
		load_value("colors", std::string("color_radar_ct_invisible_" + std::to_string(i)).c_str(), config.color_radar_ct_invisible[i], (float)atof);
		load_value("colors", std::string("color_radar_ct_visible_" + std::to_string(i)).c_str(), config.color_radar_ct_visible[i], (float)atof);
		load_value("colors", std::string("color_radar_t_invisible_" + std::to_string(i)).c_str(), config.color_radar_t_invisible[i], (float)atof);
		load_value("colors", std::string("color_radar_t_visible_" + std::to_string(i)).c_str(), config.color_radar_t_visible[i], (float)atof);
		load_value("colors", std::string("color_radar_grenades_" + std::to_string(i)).c_str(), config.color_radar_grenades[i], (float)atof);
		load_value("colors", std::string("color_visuals_bullet_tracers_" + std::to_string(i)).c_str(), config.color_visuals_bullet_tracers[i], (float)atof);
		load_value("colors", std::string("color_visuals_damage_indicator_" + std::to_string(i)).c_str(), config.color_visuals_damage_indicator[i], (float)atof);
		load_value("colors", std::string("color_esp_danger_zone_" + std::to_string(i)).c_str(), config.color_esp_danger_zone[i], (float)atof);
		load_value("colors", std::string("color_esp_world_weapon_" + std::to_string(i)).c_str(), config.color_esp_world_weapon[i], (float)atof);
		load_value("colors", std::string("color_visuals_hit_hitbox_" + std::to_string(i)).c_str(), config.color_visuals_hit_hitbox[i], (float)atof);
		load_value("colors", std::string("color_misc_static_props_" + std::to_string(i)).c_str(), config.color_misc_static_props[i], (float)atof);
		load_value("colors", std::string("color_misc_world_" + std::to_string(i)).c_str(), config.color_misc_world[i], (float)atof);
	}

	load_value("misc", "misc_bhop", config.misc_bhop, !strcmp, "true");
	load_value("misc", "misc_auto_strafe", config.misc_auto_strafe, !strcmp, "true");
	load_value("misc", "misc_fast_ladder", config.misc_fast_ladder, !strcmp, "true");
	load_value("misc", "misc_mini_jump", config.misc_mini_jump, !strcmp, "true");
	load_value("misc", "misc_rank_reveal", config.misc_rank_reveal, !strcmp, "true");
	load_value("misc", "misc_money_reveal", config.misc_money_reveal, !strcmp, "true");
	load_value("misc", "misc_aspect_ratio", config.misc_aspect_ratio, !strcmp, "true");
	load_value("misc", "misc_aspect_ratio_value", config.misc_aspect_ratio_value, (float)atof);
	load_value("misc", "misc_no_scope_border", config.misc_no_scope_border, !strcmp, "true");
	load_value("misc", "misc_no_flash_effect", config.misc_no_flash_effect, !strcmp, "true");
	load_value("misc", "misc_no_smoke", config.misc_no_smoke, !strcmp, "true");
	load_value("misc", "misc_no_visual_recoil", config.misc_no_visual_recoil, !strcmp, "true");
	load_value("misc", "misc_fov_changer", config.misc_fov_changer, !strcmp, "true");
	load_value("misc", "misc_fov_viewmodel", config.misc_fov_viewmodel, (float)atof);
	load_value("misc", "misc_fov_offset", config.misc_fov_offset, !strcmp, "true");
	load_value("misc", "misc_fov_viewmodel_x", config.misc_fov_viewmodel_x, (float)atof);
	load_value("misc", "misc_fov_viewmodel_y", config.misc_fov_viewmodel_y, (float)atof);
	load_value("misc", "misc_fov_viewmodel_z", config.misc_fov_viewmodel_z, (float)atof);
	load_value("misc", "misc_fov_override", config.misc_fov_override, (float)atof);
	load_value("misc", "misc_night_mode_enable", config.misc_night_mode_enable, !strcmp, "true");
	load_value("misc", "misc_ragdoll", config.misc_ragdoll, !strcmp, "true");
	load_value("misc", "misc_ragdoll_value", config.misc_ragdoll_value, atoi);
	load_value("misc", "misc_auto_defuse", config.misc_auto_defuse, !strcmp, "true");
	load_value("misc", "misc_overwatch_reveal", config.misc_overwatch_reveal, !strcmp, "true");
	load_value("misc", "misc_asus_walls_enable", config.misc_asus_walls_enable, !strcmp, "true");
	load_value("misc", "misc_asus_walls_value", config.misc_asus_walls_value, (float)atof);
	load_value("misc", "misc_edge_jump", config.misc_edge_jump, !strcmp, "true");
	load_value("misc", "misc_night_mode_value", config.misc_night_mode_value, (float)atof)
	load_value("misc", "misc_bloom_effect_value", config.misc_bloom_effect_value, (float)atof);
	load_value("misc", "misc_model_ambient_value", config.misc_model_ambient_value, (float)atof);
	load_value("misc", "misc_auto_accept", config.misc_auto_accept, !strcmp, "true");
	load_value("misc", "misc_inventory_always", config.misc_inventory_always, !strcmp, "true");
	load_value("misc", "misc_fog_enable", config.misc_fog_enable, !strcmp, "true");
	load_value("misc", "misc_fog_blend", config.misc_fog_blend, !strcmp, "true");
	load_value("misc", "misc_fog_start", config.misc_fog_start, (float)atof);
	load_value("misc", "misc_fog_end", config.misc_fog_end, (float)atof);
	load_value("misc", "misc_fog_density", config.misc_fog_density, (float)atof);
	load_value("misc", "misc_radar", config.misc_radar, !strcmp, "true");
	load_value("misc", "misc_clan_tag", config.misc_clan_tag, !strcmp, "true");
	load_value("misc", "misc_clan_tag_type", config.misc_clan_tag_type, atoi);
	load_string("misc", "misc_clan_tag_name", config.misc_clan_tag_name);
	load_value("misc", "misc_name_stealer", config.misc_name_stealer, !strcmp, "true");
	load_value("misc", "misc_knife_bot", config.misc_knife_bot, !strcmp, "true");
	load_value("misc", "misc_spectators", config.misc_spectators, !strcmp, "true");
	//load_value("misc", "misc_russian", config.misc_russian, !strcmp, "true");
	load_value("misc", "misc_fake_latency", config.misc_fake_latency, !strcmp, "true");
	load_value("misc", "misc_fake_latency_value", config.misc_fake_latency_value, atoi);
	load_value("misc", "misc_third_person_value", config.misc_third_person_value, atoi);
	load_value("misc", "misc_third_person_key", config.misc_third_person_key, atoi);
	load_value("misc", "misc_third_person_key_mode", config.misc_third_person_key_mode, atoi);
	load_value("misc", "misc_disable_post_processing", config.misc_disable_post_processing, !strcmp, "true");
	load_value("misc", "misc_thunder_enable", config.misc_thunder_enable, !strcmp, "true");
	load_value("misc", "misc_world_custom_color_enable", config.misc_world_custom_color_enable, !strcmp, "true");

	load_value("profile", "profile_friendly", config.profile_friendly, atoi);
	load_value("profile", "profile_leader", config.profile_leader, atoi);
	load_value("profile", "profile_player_ban", config.profile_player_ban, atoi);
	load_value("profile", "profile_player_ban_time", config.profile_player_ban_time, atoi);
	load_value("profile", "profile_player_level", config.profile_player_level, atoi);
	load_value("profile", "profile_player_level_xp", config.profile_player_level_xp, atoi);
	load_value("profile", "profile_rank_id", config.profile_rank_id, atoi);
	load_value("profile", "profile_rank_id_dz",config.profile_rank_id_dz, atoi);
	load_value("profile", "profile_rank_id_wingmans", config.profile_rank_id_wingmans, atoi);
	load_value("profile", "profile_teaching", config.profile_teaching, atoi);
	load_value("profile", "profile_wins", config.profile_wins, atoi);
	load_value("profile", "profile_wins_dz", config.profile_wins_dz, atoi);
	load_value("profile", "profile_wins_wingmans", config.profile_wins_wingmans, atoi);

	for (const auto & item : config.inventory_item_list)
		g_player_inventory->RemoveItem(g_player_inventory->GetInventoryItemByItemID(item.first)->GetSOCData());

	config.inventory_item_list.clear();

	auto items_count = config.inventory_item_list.size();

	load_value("inventory", "count", items_count, atoi);

	for (size_t i = 0; i < items_count; i++) {
		static int definition_index;
		static int paint_kit;
		static int seed;
		static int stattrak;
		static int in_use_ct;
		static int in_use_t;
		static int in_use_medal;
		static int in_use_music;
		static float wear;
		static std::array< inventory_item::sticker_kit, 5> stickers;

		const auto slot_name = "inventory_" + std::to_string(i);
		auto item = g_player_inventory->CreateEconItem();

		load_value("inventory", std::string(slot_name + "_definition_index").c_str(), definition_index, atoi);
		load_value("inventory", std::string(slot_name + "_paint_kit").c_str(), paint_kit, atoi);
		load_value("inventory", std::string(slot_name + "_wear").c_str(), wear, (float)atof);
		load_value("inventory", std::string(slot_name + "_seed").c_str(), seed, atoi);
		load_value("inventory", std::string(slot_name + "_stattrak").c_str(), stattrak, atoi);
		load_value("inventory", std::string(slot_name + "_in_use_t").c_str(), in_use_t, atoi);
		load_value("inventory", std::string(slot_name + "_in_use_ct").c_str(), in_use_ct, atoi);
		load_value("inventory", std::string(slot_name + "_in_use_medal").c_str(), in_use_medal, atoi);
		load_value("inventory", std::string(slot_name + "_in_use_music").c_str(), in_use_music, atoi);

		*item->GetAccountID() = (uint32_t)g_player_inventory->get_owner_id().id;
		*item->GetDefinitionIndex() = definition_index;
		*item->GetItemID() = g_player_inventory->GetHighestItemID() + unique_id++;
		*item->GetInventory() = 1;
		if (get_hash(weaponids.m_list[definition_index]->get_item_type_name()) != utils::get_const(XORSTR("#CSGO_Type_MusicKit")))
			*item->GetFlags() = 0;
		*item->GetOriginalID() = 0;

		for (std::size_t j = 0; j < stickers.size(); ++j) {

			const auto sticker_slot_name = slot_name + "_sticker_slot_" + std::to_string(j);

			load_value("inventory", std::string(sticker_slot_name + "_kit").c_str(), stickers[j].kit, atoi);
			load_value("inventory", std::string(sticker_slot_name + "_wear").c_str(), stickers[j].wear, (float)atof);
			load_value("inventory", std::string(sticker_slot_name + "_rotation").c_str(), stickers[j].rotation, (float)atof);
			load_value("inventory", std::string(sticker_slot_name + "_scale").c_str(), stickers[j].scale, (float)atof);

			const auto& sticker = stickers[j];

			if (sticker.kit == 0)
				continue;

			item->AddSticker(j, sticker.kit, sticker.wear, sticker.scale, sticker.rotation);
		}

		if (get_hash(weaponids.m_list[definition_index]->get_item_type_name()) != utils::get_const(XORSTR("#CSGO_Type_MusicKit"))) {
			item->SetPaintKit(paint_kit);
			item->SetPaintSeed(seed);
			item->SetPaintWear(wear);
		}
		item->SetOrigin(8);
		if (paint_kit) {
			if (get_hash(weaponids.m_list[definition_index]->get_item_type_name()) == utils::get_const(XORSTR("#CSGO_Type_MusicKit")))
				item->SetRarity(3);
			else
				item->SetRarity(changer::get_rarity(weaponids.m_list[definition_index]->get_rarity(), config.inventory_skin_info[paint_kit]->get_rarity_value()));
		}
		else if (get_hash(weaponids.m_list[definition_index]->get_item_type_name()) == utils::get_const(XORSTR("#CSGO_Type_Collectible")) || get_hash(weaponids.m_list[definition_index]->get_item_type_name()) == utils::get_const(XORSTR("#Type_CustomPlayer")) ||
			get_hash(weaponids.m_list[definition_index]->get_item_type_name()) == utils::get_const(XORSTR("#CSGO_Type_WeaponCase")) || get_hash(weaponids.m_list[definition_index]->get_item_type_name()) == utils::get_const(XORSTR("#CSGO_Tool_WeaponCase_KeyTag")))
			item->SetRarity(changer::get_rarity(weaponids.m_list[definition_index]->get_rarity(), 1));
		else
			item->SetRarity(6);

		item->SetLevel(1);
		item->SetInUse(false);

		if (stattrak > 0)
			item->SetStatTrak(stattrak);
	
		if (get_hash(weaponids.m_list[*item->GetDefinitionIndex()]->get_item_type_name()) == utils::get_const(XORSTR("#CSGO_Type_Knife")) || get_hash(weaponids.m_list[*item->GetDefinitionIndex()]->get_item_type_name()) == utils::get_const(XORSTR("#Type_Hands")))
			item->SetQuality(3);

		if (get_hash(weaponids.m_list[definition_index]->get_item_type_name()) == utils::get_const(XORSTR("#CSGO_Type_MusicKit")))
			item->SetAttributeValue<int>(166, paint_kit);

		g_player_inventory->get_item_base_type_cache()->add_object(item);
		g_player_inventory->so_created(g_player_inventory->get_owner_id(), (c_shared_object*)item, 4);

		const auto item_view = g_player_inventory->GetInventoryItemByItemID(*item->GetItemID());
		item_view->ClearInventoryImageRgba();

		config.inventory_item_list.insert({ *item->GetItemID(), {definition_index, paint_kit, seed, stattrak, wear, {stickers[0], stickers[1], stickers[2], stickers[3], stickers[4]} } });
		if (in_use_ct)
			g_inventory_manager->EquipItemInLoadout(3, weaponids.m_list[definition_index]->get_loadout_slot(), *item->GetItemID());
		if (in_use_t)
			g_inventory_manager->EquipItemInLoadout(2, weaponids.m_list[definition_index]->get_loadout_slot(), *item->GetItemID());
		if (in_use_medal)
			g_inventory_manager->EquipItemInLoadout(0, 55, *item->GetItemID());
		if (in_use_music)
			g_inventory_manager->EquipItemInLoadout(0, 54, *item->GetItemID());
	}

	load_value("settings", "settings_obs_spoof", config.settings_obs_spoof, !strcmp, "true");
}

int config_manager::upload_share_settings(std::string file) {
	std::string code = utils::random_string();
	std::string path;

	path.append("configs");
	path.append("/");
	path.append(code);
	path.append(".ini");

	HINTERNET internet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	HINTERNET ftp_session = InternetConnect(internet, XORSTR("164.132.77.139"), INTERNET_DEFAULT_FTP_PORT, XORSTR("id11291_cloud"), XORSTR("cloudshark992918zzxc"), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	FtpPutFileA(ftp_session, file.c_str(), path.c_str(), FTP_TRANSFER_TYPE_BINARY, 0);
	InternetCloseHandle(ftp_session);
	InternetCloseHandle(internet);

	utils::to_clipboard(0, code.c_str());

	return NULL;
}

int config_manager::download_share_settings(std::string file) {
	std::string request;

	request.append("configs");
	request.append("/");
	request.append(file);
	request.append(".ini");

	std::string path;
	path.append("C:/shark-hack/configs/");
	path.append(file);
	path.append(".ini");

	HINTERNET internet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	HINTERNET ftp_session = InternetConnect(internet, XORSTR("164.132.77.139"), INTERNET_DEFAULT_FTP_PORT, XORSTR("id11291_cloud"), XORSTR("cloudshark992918zzxc"), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	FtpGetFileA(ftp_session, request.c_str(), path.c_str(), NULL, NULL, NULL, NULL);
	InternetCloseHandle(ftp_session);
	InternetCloseHandle(internet);

	return NULL;
}
