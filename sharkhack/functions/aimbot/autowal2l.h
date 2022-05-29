#pragma once

#include "../../utils/entity.h"
#include "../../utils/math.h"
#include "../../utils/render.h"
#include "../../utils/utils.h"

#define DAMAGE_NO		0
#define DAMAGE_EVENTS_ONLY	1	
#define DAMAGE_YES		2
#define DAMAGE_AIM		3

enum TextureCharacters {
	CHAR_TEX_ANTLION = 'A',
	CHAR_TEX_BLOODYFLESH = 'B',
	CHAR_TEX_CONCRETE = 'C',
	CHAR_TEX_DIRT = 'D',
	CHAR_TEX_EGGSHELL = 'E',
	CHAR_TEX_FLESH = 'F',
	CHAR_TEX_GRATE = 'G',
	CHAR_TEX_ALIENFLESH = 'H',
	CHAR_TEX_CLIP = 'I',
	CHAR_TEX_PLASTIC = 'L',
	CHAR_TEX_METAL = 'M',
	CHAR_TEX_SAND = 'N',
	CHAR_TEX_FOLIAGE = 'O',
	CHAR_TEX_COMPUTER = 'P',
	CHAR_TEX_SLOSH = 'S',
	CHAR_TEX_TILE = 'T',
	CHAR_TEX_CARDBOARD = 'U',
	CHAR_TEX_VENT = 'V',
	CHAR_TEX_WOOD = 'W',
	CHAR_TEX_GLASS = 'Y',
	CHAR_TEX_WARPSHIELD = 'Z',
};

#define CHAR_TEX_STEAM_PIPE		11

namespace autowall {
	void trace_line(Vector & abs_start, Vector & abs_end, unsigned int mask, C_BasePlayer * ignore, CGameTrace * ptr);
	void scale_damage(C_BasePlayer * local_player, CGameTrace & enter_trace, CCSWeaponInfo * weapon_data, float & current_damage);
	bool trace_to_exit(C_BasePlayer * local_player, trace_t & enter_trace, trace_t & exit_trace, const Vector start_position, const Vector direction, const bool is_local);
	bool handle_bullet_penetration(C_BasePlayer * local_player, CCSWeaponInfo * weapon_data, trace_t & enter_trace, Vector & eye_position, Vector direction, int & possible_hits_remaining, float & current_damage, float penetration_power, float ff_damage_bullet_penetration, bool pskip = false);
	void fix_trace_ray(Vector end, Vector start, trace_t * oldtrace, C_BasePlayer * entity);
	void clip_trace_to_players(const Vector & vec_abs_start, const Vector & vec_abs_end, uint32_t mask, ITraceFilter * filter, trace_t * tr);
	bool fire_bullet(C_BasePlayer * local_player, Vector eyepos, C_BaseCombatWeapon * weapon, Vector & direction, float & current_damage, C_BasePlayer * entity);
	float can_hit(C_BasePlayer * local_player, Vector & point, C_BasePlayer * entity);
};
