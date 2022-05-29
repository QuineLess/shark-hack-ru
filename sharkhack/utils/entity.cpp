#include "entity.h"
#include "math.h"

void C_BaseEntity::set_abs_origin(Vector origin) {
	static auto fn = reinterpret_cast<void(__thiscall *)(void *, const Vector &)>(memory.set_abs_origin);
	return fn(this, origin);
}

void C_BaseEntity::set_abs_angles(QAngle angles) {
	static auto fn = reinterpret_cast<void(__thiscall *)(void*, const QAngle&)>(memory.set_abs_angles);
	fn(this, angles);
}

bool C_BasePlayer::is_teammate(C_BasePlayer* local_player) {

	if (!local_player)
		return false;
	
	if (utils::is_danger_zone())
		return (local_player->get_survival_team() == -1) ? false : (local_player->get_survival_team() == this->get_survival_team());
	else
		return local_player->get_team() == this->get_team();
}

bool C_BasePlayer::is_alive() {
	return get_life_state() == life_state::life_alive;
}

player_info_t C_BasePlayer::get_player_info() {
	player_info_t info;
	g_engine->GetPlayerInfo(EntIndex(), info);
	return info;
}

Vector C_BasePlayer::get_eye_position() {
	if (!this)
		return Vector(0, 0, 0);

	return get_origin() + get_view_origin();
}

Vector C_BasePlayer::get_bone_position(int hitbox_id) {
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];
	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
		auto studio_model = g_mdl_info->get_studio_model(GetModel());
		if (studio_model) {
			auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
			if (hitbox) {
				auto
					min = Vector{},
					max = Vector{};

				math::vector_transform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
				math::vector_transform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

				return (min + max) / 2.0f;
			}
		}
	}
	return Vector{};
}

bool C_BasePlayer::is_visible(C_BasePlayer* player, int bone) {

	if (!this || !player)
		return false;

	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	auto endpos = player->get_bone_position(bone);

	ray.Init(get_eye_position(), endpos);
	g_engine_trace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

bool C_BasePlayer::is_visible_with_smoke_check(C_BasePlayer* player, int bone) {

	if (!this || !player)
		return false;

	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	auto endpos = player->get_bone_position(bone);

	if (utils::line_goes_through_smoke(get_eye_position(), endpos))
		return false;

	ray.Init(get_eye_position(), endpos);
	g_engine_trace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

bool C_BasePlayer::is_breakable() {
	if (!this)
		return false;

	static auto is_breakable_fn = reinterpret_cast<bool(__thiscall*)(IClientEntity*)>(memory.is_breakable);
	const auto result = is_breakable_fn(this);

	if (!result && GetClientClass() != nullptr &&
		(GetClientClass()->m_ClassID == classids::get(XORSTR("CBaseDoor")) ||
			GetClientClass()->m_ClassID == classids::get(XORSTR("CBreakableSurface")) ||
			(GetClientClass()->m_ClassID == classids::get(XORSTR("CBaseEntity")) && GetCollideable() != nullptr && GetCollideable()->GetSolid() == solid_bsp)))
		return true;

	return result;
}


bool C_BasePlayer::is_visible(C_BasePlayer* player, const Vector& pos) {
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	ray.Init(get_eye_position(), pos);
	g_engine_trace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

CUtlVector< matrix3x4_t >& C_BasePlayer::get_cached_bone_data() {
	return *(CUtlVector< matrix3x4_t >*)((uintptr_t)this + 0x2914);
}

matrix3x4_t* C_BasePlayer::bone_cache() {
	return get_p_static_offset(matrix3x4_t*, GetClientRenderable(), 0x290C);
}

matrix3x4_t C_BasePlayer::get_bone_matrix(int bone) {
	matrix3x4_t matrix;

	auto offset = *reinterpret_cast<uintptr_t*>(uintptr_t(this) + 0x26A8);
	if (offset)
		matrix = *reinterpret_cast<matrix3x4_t*>(offset + 0x30 * bone);

	return matrix;
}

bool C_BasePlayer::get_update(C_BasePlayer * local_player) {
	return !this || this == local_player || get_health() <= 0 || IsDormant() || !is_alive() || get_gun_game_immunity() || (get_flags() & entity_flags::fl_frozen) || GetClientClass()->m_ClassID != classids::get(XORSTR("CCSPlayer"));
}

bool C_BasePlayer::is_flashed() {
	return get_flash_duration() > 0;
}

float C_BasePlayer::get_spawn_time() {
	return *(float*)((uintptr_t)this + 0x103C0);
}

float C_PlantedC4::get_bomb_timer() {
	float get_value = this->get_c4_blow() - g_globals->curtime;
	return (get_value < 0) ? 0.f : get_value;
}

bool C_BaseCombatWeapon::is_empty() {
	return get_ammo() == 0;
}

bool C_BaseCombatWeapon::is_reloading() {
	if (!this)
		return false;

	auto type = get_weapon_type();

	if (type == WT_Invalid || type == WT_Misc || type == WT_Grenade || type == WT_Knife)
		return false;

	if (!is_empty())
		return false;

	return true;
}

int C_BaseCombatWeapon::get_weapon_type() {

	if (!this)
		return WT_Invalid;

	switch (this->get_definition_index()) {
	case WEAPON_DEAGLE:
	case WEAPON_P250:
	case WEAPON_USP_SILENCER:
	case WEAPON_HKP2000:
	case WEAPON_GLOCK:
	case WEAPON_FIVESEVEN:
	case WEAPON_TEC9:
	case WEAPON_ELITE:
	case WEAPON_REVOLVER:
	case WEAPON_CZ75A:
		return WT_Pistol;
		break;
	case WEAPON_MP9:
	case WEAPON_MP7:
	case WEAPON_MP5:
	case WEAPON_UMP45:
	case WEAPON_BIZON:
	case WEAPON_P90:
	case WEAPON_MAC10:
		return WT_Submg;
		break;
	case WEAPON_KNIFE_BAYONET:
	case WEAPON_KNIFE_CSS:
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
	case WEAPON_KNIFE_BUTTERFLY:
	case WEAPON_KNIFE:
	case WEAPON_KNIFE_FALCHION:
	case WEAPON_KNIFE_FLIP:
	case WEAPON_KNIFE_GUT:
	case WEAPON_KNIFE_KARAMBIT:
	case WEAPON_KNIFE_M9_BAYONET:
	case WEAPON_KNIFE_PUSH:
	case WEAPON_KNIFE_TACTICAL:
	case WEAPON_KNIFE_T:
	case WEAPON_KNIFE_URSUS:
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
	case WEAPON_KNIFE_STILETTO:
	case WEAPON_KNIFE_WIDOWMAKER:
	case WEAPON_KNIFE_SKELETON:
	case WEAPON_KNIFE_OUTDOOR:
	case WEAPON_KNIFE_CORD:
	case WEAPON_KNIFE_CANIS:
		return WT_Knife;
		break;
	case WEAPON_SAWEDOFF:
	case WEAPON_XM1014:
	case WEAPON_MAG7:
	case WEAPON_NOVA:
		return WT_Shotgun;
	case WEAPON_M249:
	case WEAPON_NEGEV:
		return WT_Machinegun;
	case WEAPON_TASER:
	case WEAPON_C4:
		return WT_Misc;
		break;
	case WEAPON_HEGRENADE:
	case WEAPON_FLASHBANG:
	case WEAPON_DECOY:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_INCGRENADE:
	case WEAPON_MOLOTOV:
		return WT_Grenade;
		break;
	case WEAPON_AK47:
	case WEAPON_M4A1:
	case WEAPON_M4A1_SILENCER:
	case WEAPON_GALILAR:
	case WEAPON_FAMAS:
	case WEAPON_AUG:
	case WEAPON_SG556:
		return WT_Rifle;
		break;
	case WEAPON_SCAR20:
	case WEAPON_G3SG1:
	case WEAPON_SSG08:
	case WEAPON_AWP:
		return WT_Sniper;
		break;
	default:
		return WT_Knife;
	}
	return WT_Invalid;
}

bool C_BaseCombatWeapon::get_gun() {

	if (!this)
		return false;

	switch (this->get_definition_index()) {
	case WEAPON_DEAGLE:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
	case WEAPON_AK47:
	case WEAPON_AUG:
	case WEAPON_AWP:
	case WEAPON_FAMAS:
	case WEAPON_G3SG1:
	case WEAPON_GALILAR:
	case WEAPON_M249:
	case WEAPON_M4A1:
	case WEAPON_MAC10:
	case WEAPON_P90:
	case WEAPON_MP5:
	case WEAPON_UMP45:
	case WEAPON_XM1014:
	case WEAPON_BIZON:
	case WEAPON_MAG7:
	case WEAPON_NEGEV:
	case WEAPON_SAWEDOFF:
	case WEAPON_TEC9:
	case WEAPON_HKP2000:
	case WEAPON_MP7:
	case WEAPON_MP9:
	case WEAPON_NOVA:
	case WEAPON_P250:
	case WEAPON_SCAR20:
	case WEAPON_SG556:
	case WEAPON_SSG08:
		return true;
	case WEAPON_TASER:
	case WEAPON_KNIFE:
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_DECOY:
	case WEAPON_INCGRENADE:
	case WEAPON_C4:
	case WEAPON_KNIFE_T:
	case WEAPON_KNIFE_BAYONET:
	case WEAPON_KNIFE_CSS:
	case WEAPON_KNIFE_FLIP:
	case WEAPON_KNIFE_GUT:
	case WEAPON_KNIFE_KARAMBIT:
	case WEAPON_KNIFE_M9_BAYONET:
	case WEAPON_KNIFE_TACTICAL:
	case WEAPON_KNIFE_FALCHION:
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
	case WEAPON_KNIFE_BUTTERFLY:
	case WEAPON_KNIFE_PUSH:
	case WEAPON_KNIFE_CORD:
	case WEAPON_KNIFE_CANIS:
	case WEAPON_KNIFE_URSUS:
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
	case WEAPON_KNIFE_OUTDOOR:
	case WEAPON_KNIFE_STILETTO:
	case WEAPON_KNIFE_WIDOWMAKER:
	case WEAPON_KNIFE_SKELETON:
	case WEAPON_SNOWBALL:
	case WEAPON_KNIFE_GHOST:
	case WEAPON_SPANNER:
	case WEAPON_HAMMER:
	case WEAPON_AXE:
	case WEAPON_MELEE:
	case WEAPON_TABLET:
	case WEAPON_FISTS:
	case WEAPON_BREACHCHARGE:
	case WEAPON_TAGRENADE:
	case WEAPON_HEALTHSHOT:
		return false;
	case WEAPON_M4A1_SILENCER:
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_REVOLVER:
		return true;
	default:
		return false;
	}
}

CEconItemView* C_BaseCombatWeapon::get_econ_item_view() {
	static auto fn = reinterpret_cast<CEconItemView * (__thiscall*)(void*)>(memory.get_econ_item_view);
	return fn(this);
}

void C_BaseCombatWeapon::equip_wearable(C_BaseEntity* entity) {
	static auto fn_equip_wearable = reinterpret_cast<int32_t(__thiscall*)(void*, void*)>(memory.equip_wearable);
	fn_equip_wearable(this, entity);
}

void C_BaseCombatWeapon::initialize_attibutes() {
	static auto fn_initialize_attibutes = reinterpret_cast<int32_t(__thiscall*)(void*)>(memory.initialize_attibutes);
	fn_initialize_attibutes(this);
}

CCSWeaponInfo* C_BaseCombatWeapon::get_weapon_data() {
	return g_weapon_system->GetWpnData(this->get_definition_index());
}

const char* C_BaseCombatWeapon::get_weapon() {

	if (!this)
		return "";

	switch (get_definition_index()) {
	case WEAPON_DEAGLE:
		return "Desert Eagle";
		break;
	case WEAPON_ELITE:
		return "Dual Berettas";
		break;
	case WEAPON_FIVESEVEN:
		return "Five-SeveN";
		break;
	case WEAPON_GLOCK:
		return "Glock-18";
		break;
	case WEAPON_AK47:
		return "AK-47";
		break;
	case WEAPON_AUG:
		return "AUG";
		break;
	case WEAPON_AWP:
		return "AWP";
		break;
	case WEAPON_FAMAS:
		return "FAMAS";
		break;
	case WEAPON_G3SG1:
		return "G3SG1";
		break;
	case WEAPON_GALILAR:
		return "Galil AR";
		break;
	case WEAPON_M249:
		return "M249";
		break;
	case WEAPON_M4A1:
		return "M4A4";
		break;
	case WEAPON_MAC10:
		return "MAC-10";
		break;
	case WEAPON_P90:
		return "P90";
		break;
	case WEAPON_MP5:
		return "MP5-SD";
		break;
	case WEAPON_UMP45:
		return "UMP-45";
		break;
	case WEAPON_XM1014:
		return "XM1014";
		break;
	case WEAPON_BIZON:
		return "PP-Bizon";
		break;
	case WEAPON_MAG7:
		return "MAG-7";
		break;
	case WEAPON_NEGEV:
		return "Negev";
		break;
	case WEAPON_SAWEDOFF:
		return "Sawed-Off";
		break;
	case WEAPON_TEC9:
		return "Tec-9";
		break;
	case WEAPON_TASER:
		return "ZEUS";
		break;
	case WEAPON_HKP2000:
		return "P2000";
		break;
	case WEAPON_MP7:
		return "MP7";
		break;
	case WEAPON_MP9:
		return "MP9";
		break;
	case WEAPON_NOVA:
		return "Nova";
		break;
	case WEAPON_P250:
		return "P250";
		break;
	case WEAPON_SCAR20:
		return "SCAR-20";
		break;
	case WEAPON_SG556:
		return "SG 553";
		break;
	case WEAPON_SSG08:
		return "SSG 08";
		break;
	case WEAPON_KNIFEGG:
		return "Golden Knife";
		break;
	case WEAPON_KNIFE:
		return "Knife";
		break;
	case WEAPON_FLASHBANG:
		return "Flashbang";
		break;
	case WEAPON_HEGRENADE:
		return "Hegrenade";
		break;
	case WEAPON_SMOKEGRENADE:
		return "Smoke";
		break;
	case WEAPON_MOLOTOV:
		return "Molotov";
		break;
	case WEAPON_DECOY:
		return "Decoy";
		break;
	case WEAPON_INCGRENADE:
		return "Inc Grenade";
		break;
	case WEAPON_C4:
		return "C4";
		break;
	case WEAPON_KNIFE_T:
		return "Knife";
		break;
	case WEAPON_M4A1_SILENCER:
		return "M4A1-S";
		break;
	case WEAPON_USP_SILENCER:
		return "USP-S";
		break;
	case WEAPON_CZ75A:
		return "CZ75-Auto";
		break;
	case WEAPON_REVOLVER:
		return "Revolver R8";
		break;
	case WEAPON_KNIFE_BAYONET:
		return "Bayonet";
		break;
	case WEAPON_KNIFE_CSS:
		return "CSS";
		break;
	case WEAPON_KNIFE_FLIP:
		return "Flip";
		break;
	case WEAPON_KNIFE_GUT:
		return "Gut";
		break;
	case WEAPON_KNIFE_KARAMBIT:
		return "Karambit";
		break;
	case WEAPON_KNIFE_M9_BAYONET:
		return "M9 Bayonet";
		break;
	case WEAPON_KNIFE_TACTICAL:
		return "Tactical";
		break;
	case WEAPON_KNIFE_FALCHION:
		return "Falchion";
		break;
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
		return "Bowie";
		break;
	case WEAPON_KNIFE_BUTTERFLY:
		return "Butterfly";
		break;
	case WEAPON_KNIFE_PUSH:
		return "Daggers";
		break;
	case WEAPON_KNIFE_URSUS:
		return "Ursus";
		break;
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
		return "Jack Knife";
		break;
	case WEAPON_KNIFE_STILETTO:
		return "Stiletto";
		break;
	case WEAPON_KNIFE_WIDOWMAKER:
		return "Widowmaker";
		break;
	case WEAPON_KNIFE_SKELETON:
		return "Skeleton Knife";
		break;
	case WEAPON_KNIFE_OUTDOOR:
		return "Outdoor Knife";
		break;
	case WEAPON_KNIFE_CORD:
		return "Cord Knife";
		break;
	case WEAPON_KNIFE_CANIS:
		return "Canis Knife";
		break;
	}
	return "";
}

const char* C_BaseCombatWeapon::get_weapon_icon() {

	if (!this)
		return "";

	switch (get_definition_index()) {
	case WEAPON_DEAGLE:
		return "a";
		break;
	case WEAPON_ELITE:
		return "b";
		break;
	case WEAPON_FIVESEVEN:
		return "c";
		break;
	case WEAPON_GLOCK:
		return "d";
		break;
	case WEAPON_AK47:
		return "e";
		break;
	case WEAPON_AUG:
		return "f";
		break;
	case WEAPON_AWP:
		return "g";
		break;
	case WEAPON_FAMAS:
		return "h";
		break;
	case WEAPON_G3SG1:
		return "j";
		break;
	case WEAPON_GALILAR:
		return "k";
		break;
	case WEAPON_M249:
		return "i";
		break;
	case WEAPON_M4A1:
		return "l";
		break;
	case WEAPON_MAC10:
		return "n";
		break;
	case WEAPON_P90:
		return "C";
		break;
	case WEAPON_MP5:
		return "p";
		break;
	case WEAPON_UMP45:
		return "q";
		break;
	case WEAPON_XM1014:
		return "r";
		break;
	case WEAPON_BIZON:
		return "s";
		break;
	case WEAPON_MAG7:
		return "t";
		break;
	case WEAPON_NEGEV:
		return "u";
		break;
	case WEAPON_SAWEDOFF:
		return "v";
		break;
	case WEAPON_TEC9:
		return "w";
		break;
	case WEAPON_TASER:
		return "x";
		break;
	case WEAPON_HKP2000:
		return "o";
		break;
	case WEAPON_MP7:
		return "z";
		break;
	case WEAPON_MP9:
		return "A";
		break;
	case WEAPON_NOVA:
		return "B";
		break;
	case WEAPON_P250:
		return "y";
		break;
	case WEAPON_SCAR20:
		return "D";
		break;
	case WEAPON_SG556:
		return "E";
		break;
	case WEAPON_SSG08:
		return "F";
		break;
	case WEAPON_KNIFE_WIDOWMAKER:
	case WEAPON_KNIFE_T:
	case WEAPON_KNIFE_BAYONET:
	case WEAPON_KNIFE_FLIP:
	case WEAPON_KNIFE_GUT:
	case WEAPON_KNIFE_KARAMBIT:
	case WEAPON_KNIFE_M9_BAYONET:
	case WEAPON_KNIFE_TACTICAL:
	case WEAPON_KNIFE_FALCHION:
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
	case WEAPON_KNIFE_BUTTERFLY:
	case WEAPON_KNIFE_PUSH:
	case WEAPON_KNIFE_URSUS:
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
	case WEAPON_KNIFE_STILETTO:
	case WEAPON_KNIFEGG:
	case WEAPON_KNIFE:
	case WEAPON_KNIFE_CORD:
	case WEAPON_KNIFE_CANIS:
	case WEAPON_KNIFE_CSS:
	case WEAPON_KNIFE_OUTDOOR:
	case WEAPON_KNIFE_SKELETON:
		return "G";
		break;
	case WEAPON_FLASHBANG:
		return "L";
		break;
	case WEAPON_HEGRENADE:
		return "I";
		break;
	case WEAPON_SMOKEGRENADE:
		return "J";
		break;
	case WEAPON_MOLOTOV:
		return "K";
		break;
	case WEAPON_DECOY:
		return "H";
		break;
	case WEAPON_INCGRENADE:
		return "M";
		break;
	case WEAPON_C4:
		return "N";
		break;
	case WEAPON_M4A1_SILENCER:
		return "m";
		break;
	case WEAPON_USP_SILENCER:
		return "P";
		break;
	case WEAPON_CZ75A:
		return "Q";
		break;
	case WEAPON_REVOLVER:
		return "R";
		break;
	}
	return "";
}