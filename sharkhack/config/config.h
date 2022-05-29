#pragma once

#include <array>
#include <unordered_map>
#include <set>
#include <map>

struct inventory_item {
	int definition_index = 0;
	int paint_kit = 0;
	int seed = 0;
	int stattrak = 0;
	float wear = 0.f;

	struct sticker_kit {
		int kit = 0;
		float wear = 0.f;
		float scale = 1.f;
		float rotation = 0.f;
	};

	std::array<sticker_kit, 5> stickers;
};

struct legitbot_s {
	bool legit_nearest = false;
	bool legit_psilent_enable = false;
	bool legit_teamattack = false;
	bool legit_ignoresmoke = false;
	bool legit_ignoreflash = false;
	bool legit_ignorejump = false;
	bool legit_aimlock = false;
	bool legit_autofire = false;
	bool legit_dynamic_fov = false;
	bool legit_autowall = false;
	bool legit_fov_after_enable = false;
	bool legit_smooth_after_enable = false;

	int legit_hitbox = 0;
	int legit_rcs_x = 100;
	int legit_rcs_y = 100;
	int legit_psilent_type = 0;

	float legit_fov = 6.f;
	float legit_smooth = 8.f;
	float legit_fov_after = 7.f;
	float legit_smooth_after = 10.f;
	float legit_psilent_fov = 0.f;
	float legit_delay_after_kill = 0.f;
	float legit_delay_shot = 0.f;
	float legit_mindamage = 0.f;
};
struct C_RageSettings
{
	int32_t m_iFOV = 180;
	int32_t m_iHitChance = 0;
	int32_t m_iDoubleTapHitChance = 0;

	int32_t m_iMinDamage = 0;
	int32_t m_iMinDamageOverride = 0;
	int32_t m_iAccuracyBoost = 0;

	int32_t m_iHeadScale = 50;
	int32_t m_iBodyScale = 50;

	bool m_bEnabled = false;
	bool m_bLethalSafety = false;

	bool m_AutoStopOptions[ 2 ] = { false, false }; // Force Accuracy, Early
	bool m_DoubleTapOptions[ 2 ] = { false, false }; // Move between shots, stop
	bool m_Hitboxes[ 6 ] = { true, true, true, true, true, true }; // head, chest, arms, pelvis, stomach, legs
	bool m_SafeHitboxes[ 6 ] = { }; // head, chest, arms, pelvis, stomach, legs
	bool m_Multipoints[ 6 ] = { }; // head, chest, arms, pelvis, stomach, legs

	bool m_bAutoStop = false;
	bool m_bAutoScope = false;

	bool m_bPreferSafe = false;
	bool m_bPreferBody = false;
};
#define SETTING( type, name, value ) type name = value
struct config_t 
{

	bool legit_enable = false;
	bool legit_autoselectweapon = true;
	bool legit_resize_fov = false;
	bool legit_onlyscope = false;
	bool legit_desync = false;
	int legit_desync_key = 0;
	bool legit_backtrack = false;
	int legit_backtrack_time = 0;
	bool legit_backtrack_chams_enable = false;
	int legit_backtrack_chams = 0;

	std::array<legitbot_s, 34> legitbot_for_weapon;

	bool esp_enable = false;
	bool esp_enemy_only = false;
	bool esp_invisible = false;

	bool esp_box = false;
	bool esp_box_filled = false;
	bool esp_box_outline = true;
	bool esp_box_filled_fade = false;
	int esp_box_filled_alpha = 130;
	int esp_box_rounding = 0;
	int esp_box_type = 0;

	float color_esp_ct_visible[3] = { 0.f, 1.f, 0.f };
	float color_esp_ct_invisible[3] = { 1.f, 0.f, 0.f };
	float color_esp_t_visible[3] = { 0.f, 1.f, 0.f };
	float color_esp_t_invisible[3] = { 1.f, 0.f, 0.f };

	// inverter conditions
	// 0 - Always
	// 1 - Air
	// 2 - Move
	std::array < bool, 3 > m_aInverterConditions = { };

	// fakelag triggers
	// 0 - Move
	// 1 - Air
	// 2 - Peek
	std::array < bool, 3 > m_aFakelagTriggers = { };

	// rage settings
	// 0 - Autosniper
	// 1 - Scout
	// 2 - AWP
	// 3 - Deagle
	// 4 - Revolver
	// 5 - Pistols
	// 6 - Rifles
	std::array < C_RageSettings, 7 > m_aRageSettings = { };

	SETTING( int, m_iLeftFakeLimit, 0 );
	SETTING( int, m_iLagLimit, 1 );
	SETTING( int, m_iTriggerLimit, 1 );
	SETTING( int, m_iRightFakeLimit, 0 );
	SETTING( int, m_iJitterAmount, 0 );
	SETTING( int, m_iYawAddOffset, 0 );
	SETTING( int, m_iPitchMode, 1 );
	SETTING( int, m_iLegMovement, 0 );
	SETTING( bool, m_bAutoDirection, false );
	SETTING( bool, m_bJitterMove, false );
	SETTING( bool, m_bSlowWalk, false );
	SETTING(bool, m_bFakeLagEnabled, false);
	SETTING(bool, m_bAntiAim, false);
	SETTING(bool, m_bAntiAimAtTargets, false);

	int hide_shots_key_mode = 0;
	int hide_shots_key = 0;
	int double_tap_key_mode = 0;
	int double_tap_key = 0;
	int safe_point_key_mode = 0;
	int safe_point_key = 0;
	int manual_back_key = 0;
	int manual_left_key = 0;
	int manual_right_key = 0;
	int inverter_key = 0;
	int inverter_key_mode = 0;

	bool esp_health = false;
	bool esp_health_background = true;
	bool esp_health_fade = false;
	int esp_health_pos = 0;

	bool esp_armor = false;
	bool esp_armor_background = true;
	bool esp_armor_fade = false;
	int esp_armor_pos = 2;

	bool esp_name = false;
	int esp_name_pos = 0;

	float color_esp_name[3] = { 1.f, 1.f, 1.f };

	bool esp_weapon = false;
	bool esp_weapon_icon = false;
	int esp_weapon_pos = 3;

	float color_esp_weapon[3] = { 1.f, 1.f, 1.f };

	bool esp_sound_enable = false;
	float esp_sound_size = 12.f;

	float color_esp_sound[3] = { 1.f, 1.f, 1.f };

	bool esp_line = false;
	float color_esp_line[3] = { 1.f, 1.f, 1.f };

	bool chams_enable = false;
	bool chams_enemy_only = false;
	bool chams_invisible = false;
	bool chams_wireframe = false;
	int chams_type = 0;
	bool chams_more = false;
	int chams_more_type = 0;
	float chams_pearlescent_value = 5.f;
	float chams_brightness = 50.f;

	float color_chams_ct_visible[3] = { 0.f, 1.f, 0.f };
	float color_chams_ct_invisible[3] = { 1.f, 0.f, 0.f };
	float color_chams_t_visible[3] = { 0.f, 1.f, 0.f };
	float color_chams_t_invisible[3] = { 1.f, 0.f, 0.f };
	float color_chams_more_ct[3] = { 0.f, 0.3f, 1.f };
	float color_chams_more_t[3] = { 1.f, 0.3f, 0.f };

	bool chams_arms = false;
	bool chams_arms_wireframe = false;
	int chams_arms_type = 0;
	int chams_arms_more_type = 0;
	float chams_arms_pearlescent_value = 5.f;

	float color_chams_arms[3] = { 1.f, 1.f, 0.f };
	float color_chams_arms_more[3] = { 1.f, 1.f, 0.f };

	bool chams_weapon = false;
	bool chams_weapon_wireframe = false;
	int chams_weapon_type = 0;
	int chams_weapon_more_type = 0;
	float chams_weapon_pearlescent_value = 5.f;

	float color_chams_weapon[3] = { 1.f, 1.f, 0.f };
	float color_chams_weapon_more[3] = { 1.f, 1.f, 0.f };

	bool glow_enable = false;
	bool glow_enemy_only = false;
	bool glow_invisible = false;
	int glow_style = 0;

	float color_glow_ct_visible[3] = { 0.f, 0.f, 1.f };
	float color_glow_ct_invisible[3] = { 1.f, 0.f, 0.f };
	float color_glow_t_visible[3] = { 0.f, 1.f, 0.f };
	float color_glow_t_invisible[3] = { 1.f, 0.f, 0.f };

	bool glow_world_enable = false;
	bool glow_defuses = false;
	bool glow_chicken = false;
	bool glow_bomb = false;
	bool glow_planted_bomb = false;
	bool glow_weapons = false;

	float color_glow_defuses[3] = { 0.f, 0.f, 1.f };
	float color_glow_chicken[3] = { 1.f, 0.f, 0.f };
	float color_glow_bomb[3] = { 0.f, 1.f, 0.f };
	float color_glow_planted_bomb[3] = { 1.f, 0.f, 0.f };
	float color_glow_weapons[3] = { 1.f, 1.f, 0.f };

	bool visuals_dynamic_lights = false;
	bool visuals_spread_crosshair = false;
	bool visuals_taser_range = false;
	bool visuals_draw_fov = false;
	bool visuals_draw_psilent_fov = false;
	bool visuals_grenade_prediction = false;
	bool visuals_hit_effect = false;
	bool visuals_damage_indicator = false;
	float visuals_damage_indicator_speed = 1.f;
	bool visuals_hit_marker = false;
	int visuals_hit_sound = 0;
	bool visuals_sniper_crosshair = false;
	bool visuals_bullet_tracers = false;
	float visuals_bullet_tracers_life = 3.f;
	float visuals_bullet_tracers_width = 2.f;
	bool visuals_hit_hitbox = false;
	bool visuals_enemy_circle = false;
	bool visuals_bomb_timer = false;

	float color_visuals_dynamic_lights[3] = { 0.f, 1.f, 0.f };
	float color_visuals_spread_crosshair[3] = { 1.f, 1.f, 0.f };
	float color_visuals_grenade_prediction[3] = { 1.f, 0.f, 0.f };
	float color_visuals_bullet_tracers[3] = { 0.f, 1.f, 0.f };
	float color_visuals_damage_indicator[3] = { 0.f, 1.f, 0.f };
	float color_visuals_hit_hitbox[3] = { 1.f, 0.f, 0.f };

	bool radar_enable = false;
	bool radar_enemy_only = false;
	bool radar_invisible = false;
	bool radar_texture = false;
	bool radar_grenades = false;
	int radar_range = 1890;
	int radar_size = 250;

	float color_radar_ct_visible[3] = { 0.f, 1.f, 0.f };
	float color_radar_ct_invisible[3] = { 1.f, 0.f, 0.f };
	float color_radar_t_visible[3] = { 0.f, 1.f, 0.f };
	float color_radar_t_invisible[3] = { 1.f, 0.f, 0.f };
	float color_radar_grenades[3] = { 1.f, 1.f, 1.f };

	bool esp_world_weapon_enable = false;
	bool esp_world_weapon_box = false;
	bool esp_world_weapon_weapons = false;
	bool esp_world_weapon_planted_bomb = false;
	float color_esp_world_weapon[3] = { 1.f, 1.f, 1.f };

	bool esp_danger_zone_enable = false;
	bool esp_danger_zone[18];
	bool esp_danger_zone_box = false;
	float esp_danger_zone_distance = 0.f;
	float color_esp_danger_zone[3] = { 1.f, 1.f, 1.f };

	bool misc_bhop = false;
	bool misc_auto_strafe = false;
	bool misc_fast_ladder = false;
	bool misc_mini_jump = false;
	bool misc_rank_reveal = false;
	bool misc_money_reveal = false;
	bool misc_aspect_ratio = false;
	float misc_aspect_ratio_value = 1.f;
	bool misc_no_scope_border = false;
	bool misc_no_flash_effect = false;
	bool misc_no_smoke = false;
	bool misc_no_visual_recoil = false;
	bool misc_fov_changer = false;
	float misc_fov_viewmodel = 90.f;
	bool misc_fov_offset = false;
	float misc_fov_viewmodel_x = 1.f;
	float misc_fov_viewmodel_y = 1.f;
	float misc_fov_viewmodel_z = 1.f;
	float misc_fov_override = 90.f;
	bool misc_night_mode_enable = false;
	float misc_night_mode_value = 0.75f;
	bool misc_ragdoll = false;
	int misc_ragdoll_value = 0;
	bool misc_auto_defuse = false;
	bool misc_asus_walls_enable = false;
	float misc_asus_walls_value = 1.f;
	bool misc_edge_jump = false;
	float misc_bloom_effect_value = 0.f;
	float misc_model_ambient_value = 0.f;
	bool misc_auto_accept = false;
	bool misc_inventory_always = false;
	bool misc_fog_enable = false;
	bool misc_fog_blend = false;
	float misc_fog_start = 0.f;
	float misc_fog_end = 0.f;
	float misc_fog_density = 1.f;
	float color_fog[3] = { 1.f, 0.f, 0.f };
	bool misc_radar = false;
	bool misc_clan_tag = false;
	int misc_clan_tag_type = 0;
	std::string misc_clan_tag_name = "";
	bool misc_name_stealer = false;
	bool misc_knife_bot = false;
	bool misc_spectators = false;
	//bool misc_russian = false;
	bool misc_fake_latency = false;
	int misc_fake_latency_value = 1;
	int misc_third_person_value = 90;
	int misc_third_person_key = 0;
	int misc_third_person_key_mode = 0;
	bool misc_overwatch_reveal = false;
	bool misc_disable_post_processing = false;
	bool misc_thunder_enable = false;
	bool misc_world_custom_color_enable = false;

	float color_misc_world[3] = { 1.f, 0.f, 0.f };
	float color_misc_static_props[3] = { 1.f, 0.f, 0.f };

	int profile_rank_id = 0;
	int profile_wins = 0;
	int profile_rank_id_wingmans = 0;
	int profile_wins_wingmans = 0;
	int profile_rank_id_dz = 0;
	int profile_wins_dz = 0;
	int profile_player_level = 0;
	int profile_player_level_xp = 0;
	int profile_friendly = 0;
	int profile_teaching = 0;
	int profile_leader = 0;
	int profile_player_ban = 0;
	int profile_player_ban_time = 0;

	bool m_bFakeDuck = false;
	bool lobby_popups_spam = false;
	bool lobby_error_spam = false;
	int32_t m_bOverrideDamage_key = 0;
	int32_t m_bOverrideDamage_key_mode = 0;
	bool lobby_invitations = false;

	std::unordered_map<uint32_t, std::vector<CEconPaintKitDefinition*>> inventory_weapon_skins;
	std::unordered_map<uint32_t, CEconPaintKitDefinition*> inventory_skin_info;
	std::unordered_map<uint64_t, inventory_item> inventory_item_list;
	std::unordered_map<uint32_t, std::string> inventory_sticker_info;
	std::unordered_map<uint32_t, std::string> inventory_sticker_image;
	std::unordered_map<uint32_t, CEconMusicDefinition*> inventory_music_kits_info;

	bool settings_obs_spoof = false;
};

inline config_t config;
