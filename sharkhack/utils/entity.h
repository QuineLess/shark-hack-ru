#pragma once
#include "../sdk/interfaces.h"
#include "../sdk/interfaces/ehandle.h"

#include "netvars.h"
#include "memory.h"
#include "xor_str.h"
#include "DataMap.hpp"

inline uint32_t FindInDataMap( C_DataMap* pDataMap, const std::string& szPropName )
{
	while ( pDataMap )
	{
		for ( int i = 0; i < pDataMap->m_iDataNumFields; i++ )
		{
			if ( !pDataMap->m_pDataDescription[ i ].fieldName )
				continue;

			if ( pDataMap->m_pDataDescription[ i ].fieldName == szPropName )
				return pDataMap->m_pDataDescription[ i ].fieldOffset[ TD_OFFSET_NORMAL ];

			if ( pDataMap->m_pDataDescription[ i ].fieldType != FIELD_EMBEDDED )
				continue;

			if ( !pDataMap->m_pDataDescription[ i ].td )
				continue;

			uint32_t iOffset = FindInDataMap( pDataMap->m_pDataDescription[ i ].td, szPropName );
			if ( !iOffset )
				continue;

			return iOffset;
		}

		pDataMap = pDataMap->m_pBaseMap;
	}

	return 0;
}
#define DATAMAP( type, name ) \
__forceinline type& name( ) \
{\
	static uint32_t g_Datamap_##name = 0;\
    if ( !g_Datamap_##name )\
        g_Datamap_##name = FindInDataMap( this->GetPredDescMap( ), #name );\
    return *( type* )( ( uintptr_t )( this ) + g_Datamap_##name );\
}\

#define get_netvar(type, name, table, netvar, offset)                           \
    type& name() const {                                          \
        static int _##name = netvars::get_offset(table, netvar) + offset;     \
        return *(type*)((DWORD)this + _##name);                 \
    }

#define get_p_netvar(type, name, table, netvar, offset)                           \
    type* name() const {                                          \
        static int _##name = netvars::get_offset(table, netvar) + offset;     \
        return (type*)((DWORD)this + _##name);                 \
    }\

#define CUSTOM_OFFSET( name, type, prop, offset ) \
__forceinline type& name( ) \
{\
    return *( type* )( ( uintptr_t )( this ) + offset ); \
}\

enum life_state : unsigned char {
	life_alive = 0,
	life_dying = 1,
	life_dead = 2,
	max_life_state
};

enum entity_flags {
	fl_on_ground = ( 1 << 0 ),
	fl_ducking = ( 1 << 1 ),
	fl_water_jump = ( 1 << 2 ),
	fl_on_train = ( 1 << 3 ),
	fl_in_rain = ( 1 << 4 ),
	fl_frozen = ( 1 << 5 ),
	fl_at_controls = ( 1 << 6 ),
	fl_client = ( 1 << 7 ),
	fl_fake_client = ( 1 << 8 ),
	max_entity_flags
};

enum move_type {
	move_type_none = 0,
	move_type_isometric,
	move_type_walk,
	move_type_step,
	move_type_fly,
	move_type_fly_gravity,
	move_type_vphysics,
	move_type_push,
	move_type_noclip,
	move_type_ladder,
	move_type_ob_server,
	move_type_custom,
	move_type_last = move_type_custom,
	move_type_max_bits = 4
};

enum func_index {
	get_abs_origin_ = 10,
	get_abs_angles_ = 11,
	is_weapon_ = 166,
	set_model_index_ = 75,
	update_client_side_animation_ = 224,
	send_view_model_matcing_sequence_ = 246,
	draw_crosshair_ = 403,
	get_spread_ = 453,
	get_inaccuracy_ = 483,
	update_accuracy_penalty_ = 484,
};

enum WeaponType : int {
	WT_Invalid,
	WT_Grenade,
	WT_Knife,
	WT_Misc,
	WT_Pistol,
	WT_Submg,
	WT_Rifle,
	WT_Sniper,
	WT_Shotgun,
	WT_Machinegun,
	WT_Max
};

enum item_definition_index {
	WEAPON_NONE = 0,
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_MP5 = 23,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SHIELD = 37,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFEGG = 41,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_HEALTHSHOT = 57,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_TAGRENADE = 68,
	WEAPON_FISTS = 69,
	WEAPON_BREACHCHARGE = 70,
	WEAPON_TABLET = 72,
	WEAPON_MELEE = 74,
	WEAPON_AXE = 75,
	WEAPON_HAMMER = 76,
	WEAPON_SPANNER = 78,
	WEAPON_KNIFE_GHOST = 80,
	WEAPON_FIREBOMB = 81,
	WEAPON_DIVERSION = 82,
	WEAPON_FRAG_GRENADE = 83,
	WEAPON_SNOWBALL = 84,
	WEAPON_BUMPMINE = 85,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_CSS = 503,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516,
	WEAPON_KNIFE_CORD = 517,
	WEAPON_KNIFE_CANIS = 518,
	WEAPON_KNIFE_URSUS = 519,
	WEAPON_KNIFE_GYPSY_JACKKNIFE = 520,
	WEAPON_KNIFE_OUTDOOR = 521,
	WEAPON_KNIFE_STILETTO = 522,
	WEAPON_KNIFE_WIDOWMAKER = 523,
	WEAPON_KNIFE_SKELETON = 525,
	WEAPON_GLOVES_BLOODHOUND = 5027,
	WEAPON_GLOVES_T_SIDE = 5028,
	WEAPON_GLOVES_CT_SIDE = 5029,
	WEAPON_GLOVES_SPORT = 5030,
	WEAPON_GLOVES_DRIVER = 5031,
	WEAPON_GLOVES_WARPS = 5032,
	WEAPON_GLOVES_MOTO = 5033,
	WEAPON_GLOVES_SPEC = 5034,
	WEAPON_GLOVES_HYDRA = 5035,
	MAX_ITEMDEFINITIONINDEX
};
class C_BoneAccessor
{
public:
	matrix3x4_t* GetBoneArrayForWrite( )
	{
		return m_aBoneArray;
	}

	void SetBoneArrayForWrite( matrix3x4_t* bone_array )
	{
		m_aBoneArray = bone_array;
	}

	int GetReadableBones( )
	{
		return m_ReadableBones;
	}

	void SetReadableBones( int flags )
	{
		m_ReadableBones = flags;
	}

	int GetWritableBones( )
	{
		return m_WritableBones;
	}

	void SetWritableBones( int flags )
	{
		m_WritableBones = flags;
	}

	alignas( 16 ) matrix3x4_t* m_aBoneArray;
	int m_ReadableBones;
	int m_WritableBones;
};
template<typename FuncType>
__forceinline static FuncType CallVFunct123ion( void* ppClass, int index )
{
	int* pVTable = *( int** ) ppClass;
	int dwAddress = pVTable[ index ];
	return ( FuncType ) ( dwAddress );
}
class C_BaseEntity : public IClientEntity {
public:
	get_virtual_fn( 17, GetPredDescMap( ), C_DataMap* ( __thiscall* )( void* ) );
	get_netvar( int, get_team, XORSTR( "DT_BaseEntity" ), XORSTR( "m_iTeamNum" ), NULL );
	get_netvar( CBaseHandle, get_owner_entity, XORSTR( "DT_BaseEntity" ), XORSTR( "m_hOwnerEntity" ), NULL );
	get_netvar( const matrix3x4_t, get_coordinate_frame, XORSTR( "DT_BaseEntity" ), XORSTR( "m_CollisionGroup" ), -0x30 );
	get_netvar( move_type, get_move_type, XORSTR( "DT_BaseEntity" ), XORSTR( "m_nRenderMode" ), 0x1 );
	get_netvar( Vector, get_origin, XORSTR( "DT_BaseEntity" ), XORSTR( "m_vecOrigin" ), NULL );
	get_netvar( Vector, get_angles, XORSTR( "DT_BaseEntity" ), XORSTR( "m_vecAngles" ), NULL );
	get_netvar( bool, get_spotted, XORSTR( "DT_BaseEntity" ), XORSTR( "m_bSpotted" ), NULL );
	get_netvar( int, get_model_index, XORSTR( "DT_BaseEntity" ), XORSTR( "m_nModelIndex" ), NULL );
	get_netvar( float, get_simulation_time, XORSTR( "DT_BaseEntity" ), XORSTR( "m_flSimulationTime" ), NULL );
	get_netvar( float, m_flSimulationTime, XORSTR( "DT_BaseEntity" ), XORSTR( "m_flSimulationTime" ), NULL );
	get_netvar( float, m_flOldSimulationTime, XORSTR( "DT_BaseEntity" ), XORSTR( "m_flSimulationTime" ), 4 );
	get_netvar( int, get_sentry_health, XORSTR( "DT_Dronegun" ), XORSTR( "m_iHealth" ), NULL )
	get_netvar( Vector, m_vecViewOffset, XORSTR( "DT_BasePlayer" ), XORSTR( "m_vecViewOffset[0]" ), NULL );
	get_netvar( Vector, m_aimPunchAngleVel, XORSTR( "DT_BasePlayer" ), XORSTR( "m_aimPunchAngleVel" ), NULL );
	get_netvar( float, m_flThirdpersonRecoil, "DT_CSPlayer", "m_flThirdpersonRecoil", NULL );
	get_netvar( float, m_flLowerBodyYaw, "DT_CSPlayer", "m_flLowerBodyYawTarget", NULL );
	get_netvar( float, m_flDuckSpeed, "DT_BasePlayer", "m_flDuckSpeed", NULL );
	get_netvar( float, m_flDuckAmount, "DT_BasePlayer", "m_flDuckAmount", NULL );
	get_netvar( bool, m_bClientSideAnimation, "DT_BaseAnimating", "m_bClientSideAnimation", NULL );
	get_netvar( CHandle < C_BaseEntity >, m_hGroundEntity, "DT_BasePlayer", "m_hGroundEntity", NULL );

	inline bool is_breakable_entity( )
	{
		static auto pIsBreakableEntity = ( void* )( utils::find_signature( "client.dll", "55 8B EC 51 56 8B F1 85 F6 74 68" ) );

		const auto szObjectName = this->GetClientClass( )->m_pNetworkName;
		if ( szObjectName[ 0 ] == 'C' )
		{
			if ( szObjectName[ 7 ] == 't' || szObjectName[ 7 ] == 'b' )
				return true;
		}

		return ( ( bool( __thiscall* )( C_BaseEntity* ) )( pIsBreakableEntity ) )( this );
	}

	__forceinline void InvalidateBoneCache( )
	{
		static auto pModelBoneCounter = ( void* )( utils::find_signature( "client.dll", "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81" ) );
		int nModelBoneCounter = **( int** ) ( ( ( DWORD ) ( pModelBoneCounter ) ) + 10 );

		*( float* ) ( ( DWORD ) this + 0x2928 ) = -FLT_MAX;
		*( int* ) ( ( DWORD ) this + 0x2690 ) = nModelBoneCounter - 1;
	}

	CUSTOM_OFFSET( m_CachedBoneData, CUtlVector < matrix3x4_t >, FNV32( "CachedBoneData" ), 0x2914 );
	CUSTOM_OFFSET( GetBoneAccessor, C_BoneAccessor, FNV32( "BoneAccessor" ), 0x26A8 );
	CUSTOM_OFFSET( m_angVisualAngles, QAngle, FNV32( "VisualAngles" ), 0x31E8 );
	CUSTOM_OFFSET( m_flSpawnTime, float_t, FNV32( "SpawnTime" ), 0x103C0 );
	CUSTOM_OFFSET( GetMoveType, int32_t, FNV32( "MoveType" ), 0x25C );
	CUSTOM_OFFSET( m_nClientEffects, int32_t, FNV32( "ClientEffects" ), 0x68 );
	CUSTOM_OFFSET( m_nLastSkipFramecount, int32_t, FNV32( "LastSkipFramecount" ), 0xA68 );
	CUSTOM_OFFSET( m_nOcclusionFrame, int32_t, FNV32( "OcclusionFrame" ), 2608 );
	CUSTOM_OFFSET( m_nOcclusionMask, int32_t, FNV32( "OcclusionMask" ), 2600 );
	CUSTOM_OFFSET( m_pInverseKinematics, LPVOID, FNV32( "InverseKinematics" ), 9840 );
	CUSTOM_OFFSET( m_bJiggleBones, bool, FNV32( "JiggleBones" ), 0x2930 );
	CUSTOM_OFFSET( m_bMaintainSequenceTransition, bool, FNV32( "MaintainSequenceTransition" ), 0x9F0 );
	typedef void( __thiscall* GetShootPosition_t )( LPVOID, Vector* );

	Vector GetShootPosition( )
	{
		Vector vecShootPosition = Vector( 0, 0, 0 );
		if ( !this )
			return vecShootPosition;

		reinterpret_cast < GetShootPosition_t > ( get_virtual( this, 285 ) )( this, &vecShootPosition );
		return vecShootPosition;
	}

	CStudioHdr* GetStudioHdr( )
	{
		return *( CStudioHdr** ) ( ( DWORD ) ( this ) + 0x2950 );
	}

	DATAMAP( int, m_iEFlags );
	DATAMAP( float, m_flStamina );
	DATAMAP( int, m_fEffects );
	DATAMAP( Vector, m_vecAbsVelocity );
	DATAMAP( Vector, m_vecBaseVelocity );

	get_virtual_fn( func_index::set_model_index_, set_model_index( int index ), void( __thiscall* )( void*, int ), index );
	get_virtual_fn( func_index::is_weapon_, is_weapon( ), bool( __thiscall* )( void* ) );
	get_virtual_fn( 158, is_player( ), bool( __thiscall* )( void* ) );
	get_virtual_fn( func_index::get_abs_origin_, get_abs_origin( ), Vector& ( __thiscall* )( void* ) );
	
	Vector WorldSpaceCenter( )
	{
		Vector vecOrigin = get_origin( );

		Vector vecMins = this->GetCollideable( )->OBBMins( ) + vecOrigin;
		Vector vecMaxs = this->GetCollideable( )->OBBMaxs( ) + vecOrigin;

		Vector vecSize = vecMaxs - vecMins;
		vecSize /= 2.0f;
		vecSize += vecMins;
		return vecSize;
	}

	void set_abs_origin( Vector origin );
	void set_abs_angles( QAngle angles );
};

class C_BaseAttributableItem : public C_BaseEntity 
{
public:
	get_netvar( int, get_owner_xuid_low, XORSTR( "DT_BaseAttributableItem" ), XORSTR( "m_OriginalOwnerXuidLow" ), NULL );
	get_netvar( int, get_owner_xuid_high, XORSTR( "DT_BaseAttributableItem" ), XORSTR( "m_OriginalOwnerXuidHigh" ), NULL );
	get_netvar( int, get_item_id_high, XORSTR( "DT_BaseAttributableItem" ), XORSTR( "m_iItemIDHigh" ), NULL );
	get_netvar( int, get_item_id_low, XORSTR( "DT_BaseAttributableItem" ), XORSTR( "m_iItemIDLow" ), NULL );
	get_netvar( int, get_account_id, XORSTR( "DT_BaseAttributableItem" ), XORSTR( "m_iAccountID" ), NULL );
	get_netvar( bool, is_initialized, XORSTR( "DT_BaseAttributableItem" ), XORSTR( "m_bInitialized" ), NULL );
	get_netvar( short, get_definition_index, XORSTR( "DT_BaseAttributableItem" ), XORSTR( "m_iItemDefinitionIndex" ), NULL );
};

class C_BaseWeaponWorldModel : public C_BaseEntity {
public:
	get_netvar( int, get_model_index, XORSTR( "DT_BaseWeaponWorldModel" ), XORSTR( "m_nModelIndex" ), NULL );
};

class C_BaseCombatWeapon : public C_BaseAttributableItem {
public:
	get_netvar( float, m_flLastShotTime, XORSTR( "DT_WeaponCSBase" ), XORSTR( "m_fLastShotTime" ), NULL );
	get_netvar( int, get_ammo, XORSTR( "DT_BaseCombatWeapon" ), XORSTR( "m_iClip1" ), NULL );
	get_netvar( CBaseHandle, get_owner, XORSTR( "DT_BaseCombatWeapon" ), XORSTR( "m_hOwner" ), NULL );
	get_netvar( CBaseHandle, get_weapon_world_model, XORSTR( "DT_BaseCombatWeapon" ), XORSTR( "m_hWeaponWorldModel" ), NULL );
	get_netvar( float, get_next_primary_attack, XORSTR( "DT_BaseCombatWeapon" ), XORSTR( "m_flNextPrimaryAttack" ), NULL );
	get_netvar( float, get_next_secondary_attack, XORSTR( "DT_BaseCombatWeapon" ), XORSTR( "m_flNextSecondaryAttack" ), NULL );
	get_netvar( int, world_dropped_model_index, XORSTR( "DT_BaseCombatWeapon" ), XORSTR( "m_iWorldDroppedModelIndex" ), NULL );
	get_netvar( bool, get_pin_pulled, XORSTR( "DT_BaseCSGrenade" ), XORSTR( "m_bPinPulled" ), NULL );
	get_netvar( float, get_throw_time, XORSTR( "DT_BaseCSGrenade" ), XORSTR( "m_fThrowTime" ), NULL );
	get_netvar( int, get_zoom_level, XORSTR( "DT_WeaponCSBaseGun" ), XORSTR( "m_zoomLevel" ), NULL );
	get_netvar( float, get_post_pone_fire_ready_time, XORSTR( "DT_WeaponCSBase" ), XORSTR( "m_flPostponeFireReadyTime" ), NULL );
	get_netvar( float, m_flRecoilIndex, XORSTR( "DT_WeaponCSBase" ), XORSTR( "m_flRecoilIndex" ), 0 );
	get_netvar( float, m_fAccuracyPenalty, XORSTR( "DT_WeaponCSBase" ), XORSTR( "m_fAccuracyPenalty" ), 0 );
	
	bool is_empty( );
	bool is_reloading( );
	int get_weapon_type( );
	bool get_gun( );
	CEconItemView* get_econ_item_view( );
	void equip_wearable( C_BaseEntity* entity );
	void initialize_attibutes( );
	CCSWeaponInfo* get_weapon_data( );
	const char* get_weapon( );
	const char* get_weapon_icon( );

	DATAMAP( int, m_Activity );

	inline bool IsRevolver( )
	{
		return get_definition_index( ) == WEAPON_REVOLVER;
	}
	inline bool IsSMG( )
	{
		int ID = get_definition_index( );
		if ( ID == WEAPON_M249 || ID == WEAPON_NEGEV || ID == WEAPON_MP7 || ID == WEAPON_MP9 || ID == WEAPON_P90 || ID == WEAPON_UMP45 || ID == WEAPON_BIZON )
			return true;

		return false;
	}
	inline bool IsGrenade( )
	{
		return get_weapon_type( ) == WT_Grenade;
	}
	inline bool IsGun( )
	{
		switch ( get_weapon_type( ) )
		{
			case WT_Misc:
			return false;
			case WT_Grenade:
			return false;
			case WT_Knife:
			return false;
			default:
			return true;
		}
	}
	inline bool CanShift( )
	{
		switch ( ( int ) ( this->get_definition_index( ) ) )
		{
			case 43:
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:
			case 64:
			case 84:
			return false;
			break;
			default:
			break;
		}

		return this->IsGun( );
	}
	inline bool IsKnife( )
	{
		int idx = this->get_definition_index( );
		return idx == WEAPON_KNIFE || idx == WEAPON_KNIFE_BAYONET || idx == WEAPON_KNIFE_BUTTERFLY || idx == WEAPON_KNIFE_FALCHION
			|| idx == WEAPON_KNIFE_FLIP || idx == WEAPON_KNIFE_GUT || idx == WEAPON_KNIFE_KARAMBIT || idx == WEAPON_KNIFE_M9_BAYONET
			|| idx == WEAPON_KNIFE_PUSH || idx == WEAPON_KNIFE_SURVIVAL_BOWIE || idx == WEAPON_KNIFE_T || idx == WEAPON_KNIFE_TACTICAL
			|| idx == WEAPON_KNIFEGG || idx == WEAPON_KNIFE_GHOST || idx == WEAPON_KNIFE_GYPSY_JACKKNIFE || idx == WEAPON_KNIFE_STILETTO
			|| idx == WEAPON_KNIFE_URSUS || idx == WEAPON_KNIFE_WIDOWMAKER || idx == WEAPON_KNIFE_CSS || idx == WEAPON_KNIFE_CANIS
			|| idx == WEAPON_KNIFE_CORD || idx == WEAPON_KNIFE_OUTDOOR || idx == WEAPON_KNIFE_SKELETON;
	}
	inline bool IsRifle( )
	{
		switch ( get_weapon_type( ) )
		{
			case WT_Rifle:
			return true;
			case WT_Submg:
			return true;
			case WT_Shotgun:
			return true;
			case WT_Machinegun:
			return true;
			default:
			return false;
		}
	}
	inline bool IsSniper( )
	{
		switch (get_weapon_data()->m_iWeaponType)
		{
		case WEAPONTYPE_SNIPER_RIFLE:
			return true;
		default:
			return false;
		}
	}

	get_virtual_fn( func_index::draw_crosshair_, draw_crosshair( ), void( __thiscall* )( void* ) );
	get_virtual_fn( func_index::get_spread_, get_spread( ), float( __thiscall* )( void* ) );
	get_virtual_fn( func_index::update_accuracy_penalty_, update_accuracy_penalty( ), void( __thiscall* )( void* ) );
	get_virtual_fn( func_index::get_inaccuracy_, get_inaccuracy( ), float( __thiscall* )( void* ) );
};

enum C_CSGOAnimationActivity
{
	ACT_CSGO_DEFUSE = 958,
	ACT_CSGO_DEFUSE_WITH_KIT,
	ACT_CSGO_FLASHBANG_REACTION,
	ACT_CSGO_FIRE_PRIMARY,
	ACT_CSGO_FIRE_PRIMARY_OPT_1,
	ACT_CSGO_FIRE_PRIMARY_OPT_2,
	ACT_CSGO_FIRE_SECONDARY,
	ACT_CSGO_FIRE_SECONDARY_OPT_1,
	ACT_CSGO_FIRE_SECONDARY_OPT_2,
	ACT_CSGO_RELOAD,
	ACT_CSGO_RELOAD_START,
	ACT_CSGO_RELOAD_LOOP,
	ACT_CSGO_RELOAD_END,
	ACT_CSGO_OPERATE,
	ACT_CSGO_DEPLOY,
	ACT_CSGO_CATCH,
	ACT_CSGO_SILENCER_DETACH,
	ACT_CSGO_SILENCER_ATTACH,
	ACT_CSGO_TWITCH,
	ACT_CSGO_TWITCH_BUYZONE,
	ACT_CSGO_PLANT_BOMB,
	ACT_CSGO_IDLE_TURN_BALANCEADJUST,
	ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING,
	ACT_CSGO_ALIVE_LOOP,
	ACT_CSGO_FLINCH,
	ACT_CSGO_FLINCH_HEAD,
	ACT_CSGO_FLINCH_MOLOTOV,
	ACT_CSGO_JUMP,
	ACT_CSGO_FALL,
	ACT_CSGO_CLIMB_LADDER,
	ACT_CSGO_LAND_LIGHT,
	ACT_CSGO_LAND_HEAVY,
	ACT_CSGO_EXIT_LADDER_TOP,
	ACT_CSGO_EXIT_LADDER_BOTTOM,
	ACT_CSGO_PARACHUTE,
	ACT_CSGO_UIPLAYER_IDLE,
	ACT_CSGO_UIPLAYER_WALKUP,
	ACT_CSGO_UIPLAYER_CELEBRATE,
	ACT_CSGO_UIPLAYER_CONFIRM,
	ACT_CSGO_UIPLAYER_BUYMENU,
	ACT_CSGO_UIPLAYER_PATCH
};
class C_EnvTonemapController : public C_BaseEntity {
public:
	get_netvar( bool, get_use_custom_auto_exposure_min, XORSTR( "DT_EnvTonemapController" ), XORSTR( "m_bUseCustomAutoExposureMin" ), NULL );
	get_netvar( bool, get_use_custom_auto_exposure_max, XORSTR( "DT_EnvTonemapController" ), XORSTR( "m_bUseCustomAutoExposureMax" ), NULL );
	get_netvar( float, get_custom_auto_exposure_min, XORSTR( "DT_EnvTonemapController" ), XORSTR( "m_flCustomAutoExposureMin" ), NULL );
	get_netvar( float, get_custom_auto_exposure_max, XORSTR( "DT_EnvTonemapController" ), XORSTR( "m_flCustomAutoExposureMax" ), NULL );
	get_netvar( float, get_custom_bloom_scale, XORSTR( "DT_EnvTonemapController" ), XORSTR( "m_flCustomBloomScale" ), NULL );
};

class C_Inferno : public C_BaseEntity {
public:
	get_netvar( bool, get_fire_is_burning, XORSTR( "DT_Inferno" ), XORSTR( "m_bFireIsBurning" ), NULL );
	get_netvar( int, get_fire_delta_x, XORSTR( "DT_Inferno" ), XORSTR( "m_fireXDelta" ), NULL );
	get_netvar( int, get_fire_delta_y, XORSTR( "DT_Inferno" ), XORSTR( "m_fireYDelta" ), NULL );
	get_netvar( int, get_fire_delta_z, XORSTR( "DT_Inferno" ), XORSTR( "m_fireZDelta" ), NULL );
	get_netvar( int, get_fire_effect_begin, XORSTR( "DT_Inferno" ), XORSTR( "m_nFireEffectTickBegin" ), NULL );
};

class C_BaseViewModel : public C_BaseEntity 
{
public:
	get_netvar( int, m_iSequence, XORSTR( "DT_BaseViewModel" ), XORSTR( "m_nSequence" ), NULL );
	get_netvar( int, m_iAnimationParity, XORSTR( "DT_BaseViewModel" ), XORSTR( "m_nAnimationParity" ), NULL );
	
	get_netvar( CBaseHandle, get_weapon, XORSTR( "DT_BaseViewModel" ), XORSTR( "m_hWeapon" ), NULL );
	get_netvar( int, get_model_index, XORSTR( "DT_BaseViewModel" ), XORSTR( "m_nModelIndex" ), NULL );
	get_netvar( int, get_view_model_index, XORSTR( "DT_BaseViewModel" ), XORSTR( "m_nViewModelIndex" ), NULL );
	get_netvar( CBaseHandle, get_owner, XORSTR( "DT_BaseViewModel" ), XORSTR( "m_hOwner" ), NULL );

	DATAMAP( float, m_flCycle );
	DATAMAP( float, m_flAnimTime );

	get_virtual_fn( func_index::send_view_model_matcing_sequence_, send_view_model_matcing_sequence( int sequence ), void( __thiscall* )( void*, int ), sequence );
};

class C_CSRagdoll : public C_BaseEntity {
public:
	get_netvar( Vector, get_force, XORSTR( "DT_CSRagdoll" ), XORSTR( "m_vecForce" ), NULL );
	get_netvar( Vector, get_ragdoll_velocity, XORSTR( "DT_CSRagdoll" ), XORSTR( "m_vecRagdollVelocity" ), NULL );
};

class C_BasePlayer : public C_BaseEntity {
public:
	get_netvar( int, get_life_state, XORSTR( "DT_BasePlayer" ), XORSTR( "m_lifeState" ), NULL );
	get_netvar( int, get_flags, XORSTR( "DT_BasePlayer" ), XORSTR( "m_fFlags" ), NULL );
	get_netvar( int, get_health, XORSTR( "DT_BasePlayer" ), XORSTR( "m_iHealth" ), NULL );
	get_netvar( int, get_tick_base, XORSTR( "DT_BasePlayer" ), XORSTR( "m_nTickBase" ), NULL );
	get_netvar(int, get_observer_mode, XORSTR("DT_BasePlayer"), XORSTR("m_iObserverMode"), NULL);
	get_netvar( float, get_fall_velocity, XORSTR( "DT_BasePlayer" ), XORSTR( "m_flFallVelocity" ), NULL );
	get_netvar( Vector, get_view_origin, XORSTR( "DT_BasePlayer" ), XORSTR( "m_vecViewOffset[0]" ), NULL );
	get_netvar( Vector, get_velocity, XORSTR( "DT_BasePlayer" ), XORSTR( "m_vecVelocity[0]" ), NULL );
	get_netvar( QAngle, get_view_punch, XORSTR( "DT_BasePlayer" ), XORSTR( "m_viewPunchAngle" ), NULL );
	get_netvar( QAngle, get_punch, XORSTR( "DT_BasePlayer" ), XORSTR( "m_aimPunchAngle" ), NULL );
	get_netvar( CHandle<C_BasePlayer>, get_observer_target, XORSTR( "DT_BasePlayer" ), XORSTR( "m_hObserverTarget" ), NULL );
	get_virtual_fn( 223, GetLayerSequenceCycleRate( C_AnimationLayer* AnimationLayer, int32_t iLayerSequence ), float_t( __thiscall* )( void*, C_AnimationLayer*, int32_t ), AnimationLayer, iLayerSequence );

	__forceinline int32_t GetSequenceActivity( int32_t iSequence )
	{
		studiohdr_t* pStudioHDR = g_mdl_info->get_studio_model( this->GetModel( ) );
		if ( !pStudioHDR )
			return -1;

		static auto pGetSequenceActivity = ( void* )( utils::find_signature( "client.dll", "55 8B EC 53 8B 5D 08 56 8B F1 83" ) );
		return ( ( int32_t( __fastcall* )( void*, void*, int ) )( pGetSequenceActivity ) )( this, pStudioHDR, iSequence );
	}

	__forceinline float_t GetMaxPlayerSpeed( )
	{
		// get max speed of player from his weapon
		C_BaseCombatWeapon* pWeapon = this->get_weapon_handle( ).Get( );
		if ( pWeapon )
		{
			CCSWeaponInfo* pWeaponData = pWeapon->get_weapon_data( );
			if ( pWeaponData )
				return this->is_scoped( ) ? pWeaponData->m_flMaxPlayerSpeedAlt : pWeaponData->m_flMaxPlayerSpeed;
		}

		return 260.0f;
	}

	__forceinline int32_t& m_nFinalPredictedTick( )
	{
		return *( int32_t* ) ( ( DWORD ) ( this ) + 0x3444 );
	}
	
	void SetupBones_AttachmentHelper( )
	{
		static auto pSetupBones_AttachmentHelper = ( void* )( utils::find_signature( "client.dll", "55 8B EC 83 EC 48 53 8B 5D 08 89 4D F4" ) );
		return ( ( void( __thiscall* )( void*, void* ) )( pSetupBones_AttachmentHelper ) )( this, this->GetStudioHdr( ) );
	}

	void InvalidatePhysicsRecursive( int nSequence )
	{
		static auto pInvalidatePhysicsRecursive = ( void* )( utils::find_signature( "client.dll", "55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3" ) );
		return ( ( void( __thiscall* )( void*, int ) )( pInvalidatePhysicsRecursive ) )( this, nSequence );
	}

	get_netvar( bool, m_bIsWalking, XORSTR( "DT_CSPlayer" ), XORSTR( "m_bIsWalking" ), NULL );
	get_netvar( bool, get_has_heavy_armor, XORSTR( "DT_CSPlayer" ), XORSTR( "m_bHasHeavyArmor" ), NULL );
	get_netvar( bool, get_has_helmet, XORSTR( "DT_CSPlayer" ), XORSTR( "m_bHasHelmet" ), NULL );
	get_netvar( bool, get_gun_game_immunity, XORSTR( "DT_CSPlayer" ), XORSTR( "m_bGunGameImmunity" ), NULL );
	get_netvar( bool, is_scoped, XORSTR( "DT_CSPlayer" ), XORSTR( "m_bIsScoped" ), NULL );
	get_netvar( int, get_armor, XORSTR( "DT_CSPlayer" ), XORSTR( "m_ArmorValue" ), NULL );
	get_netvar( int, get_shots_fired, XORSTR( "DT_CSPlayer" ), XORSTR( "m_iShotsFired" ), NULL );
	get_netvar( int, get_ragdoll, XORSTR( "DT_CSPlayer" ), XORSTR( "m_hRagdoll" ), NULL )
	get_netvar( float, get_flash_duration, XORSTR( "DT_CSPlayer" ), XORSTR( "m_flFlashDuration" ), NULL );
	get_netvar( float, get_health_boost_time, XORSTR( "DT_CSPlayer" ), XORSTR( "m_flHealthShotBoostExpirationTime" ), NULL );
	get_netvar( int, get_survival_team, XORSTR( "DT_CSPlayer" ), XORSTR( "m_nSurvivalTeam" ), NULL );
	get_netvar( int, get_hitbox_set, XORSTR( "DT_BaseAnimating" ), XORSTR( "m_nHitboxSet" ), NULL );
	get_netvar( float, get_next_attack, XORSTR( "DT_BaseCombatCharacter" ), XORSTR( "m_flNextAttack" ), NULL );
	get_netvar( QAngle, get_angles, XORSTR( "DT_BasePlayer" ), XORSTR( "deadflag" ), 0x4 );
	get_netvar( int, get_n_body, "DT_BaseAnimating", "m_nBody", NULL );
	get_netvar( QAngle, m_angEyeAngles, XORSTR( "DT_CSPlayer" ), XORSTR( "m_angEyeAngles[0]" ), NULL );

	get_netvar( CHandle<C_BaseCombatWeapon>, get_weapon_handle, XORSTR( "DT_BaseCombatCharacter" ), XORSTR( "m_hActiveWeapon" ), NULL );
	get_netvar( CHandle<C_BaseViewModel>, get_view_model, XORSTR( "DT_BasePlayer" ), XORSTR( "m_hViewModel[0]" ), NULL );

	get_p_netvar( CHandle<C_BaseCombatWeapon>, get_weapons, XORSTR( "DT_BaseCombatCharacter" ), XORSTR( "m_hMyWeapons" ), NULL );
	get_p_netvar( int, get_wearables, XORSTR( "DT_BaseCombatCharacter" ), XORSTR( "m_hMyWearables" ), NULL );

	get_virtual_fn( func_index::update_client_side_animation_, update_client_side_animation( ), void( __thiscall* )( void* ) );
	get_virtual_fn( func_index::get_abs_angles_, get_abs_angles( ), QAngle& ( __thiscall* )( void* ) );

	int32_t get_user_id( ) {

		if ( player_info_t player_info; g_engine->GetPlayerInfo( EntIndex( ), player_info ) )
			return player_info.userId;

		return -1;
	}

	__forceinline bool CanFire(int32_t ShiftAmount = 0, bool bCheckRevolver = false)
	{
		C_BaseCombatWeapon* pCombatWeapon = this->get_weapon_handle().Get();
		if (!pCombatWeapon || (pCombatWeapon->get_definition_index() != WEAPON_TASER && !pCombatWeapon->IsGun()))
			return true;

		float_t flServerTime = (this->get_tick_base() - ShiftAmount) * g_globals->interval_per_tick;
		if (bCheckRevolver)
			if (pCombatWeapon->get_post_pone_fire_ready_time() >= flServerTime || pCombatWeapon->m_Activity() != 208)
				return false;

		if (this->get_next_attack() > flServerTime)
			return false;

		return pCombatWeapon->get_next_primary_attack() <= flServerTime;
	}

	bool is_teammate( C_BasePlayer* local_player );

	bool is_alive( );
	player_info_t get_player_info( );
	Vector get_eye_position( );
	Vector get_bone_position( int hitbox_id );
	bool is_visible( C_BasePlayer* player, int bone );
	bool is_visible_with_smoke_check( C_BasePlayer* player, int bone );
	bool is_breakable( );
	bool is_visible( C_BasePlayer* player, const Vector& pos );
	CUtlVector<matrix3x4_t>& get_cached_bone_data( );
	matrix3x4_t* bone_cache( );
	matrix3x4_t get_bone_matrix( int bone );
	bool get_update( C_BasePlayer* local_player );
	bool is_flashed( );
	C_CSGOPlayerAnimationState* m_PlayerAnimationStateCSGO( )
	{
		return *( C_CSGOPlayerAnimationState** )( ( DWORD )( this ) + 0x9960 );
	}


	inline C_AnimationLayer* GetAnimationLayers( )
	{
		return *( C_AnimationLayer** ) ( ( DWORD ) ( this ) + 0x2990 );
	}

	inline std::array < float, 24 >& m_aPoseParameters( )
	{
		return *( std::array < float, 24 >* )( ( DWORD ) ( this ) + 0x2778 );
	}


	float get_spawn_time( );
};

class C_PlantedC4 : public C_BaseEntity {
public:
	get_netvar( bool, get_bomb_ticking, XORSTR( "DT_PlantedC4" ), XORSTR( "m_bBombTicking" ), NULL );
	get_netvar( bool, get_bomb_defused, XORSTR( "DT_PlantedC4" ), XORSTR( "m_bBombDefused" ), NULL );
	get_netvar( float, get_c4_blow, XORSTR( "DT_PlantedC4" ), XORSTR( "m_flC4Blow" ), NULL );
	get_netvar( float, get_timer_length, XORSTR( "DT_PlantedC4" ), XORSTR( "m_flTimerLength" ), NULL );
	get_netvar( float, get_defuse_length, XORSTR( "DT_PlantedC4" ), XORSTR( "m_flDefuseLength" ), NULL );
	get_netvar( float, get_defuse_count_down, XORSTR( "DT_PlantedC4" ), XORSTR( "m_flDefuseCountDown" ), NULL );
	get_netvar( CHandle<C_BaseEntity>, get_bomb_defuser, XORSTR( "DT_PlantedC4" ), XORSTR( "m_hBombDefuser" ), NULL );

	float get_bomb_timer( );
};

class C_FogController : public C_BaseEntity {
public:
	get_netvar( bool, get_fog_enable, XORSTR( "DT_FogController" ), XORSTR( "m_fog.enable" ), NULL );
	get_netvar( bool, get_fog_blend, XORSTR( "DT_FogController" ), XORSTR( "m_fog.blend" ), NULL );
	get_netvar( float, get_fog_start, XORSTR( "DT_FogController" ), XORSTR( "m_fog.start" ), NULL );
	get_netvar( float, get_fog_end, XORSTR( "DT_FogController" ), XORSTR( "m_fog.end" ), NULL );
	get_netvar( float, get_fog_max_density, XORSTR( "DT_FogController" ), XORSTR( "m_fog.maxdensity" ), NULL );
	get_netvar( int, get_fog_color_primary, XORSTR( "DT_FogController" ), XORSTR( "m_fog.colorPrimary" ), NULL );
	get_netvar( int, get_fog_color_secondary, XORSTR( "DT_FogController" ), XORSTR( "m_fog.colorSecondary" ), NULL );
};

class C_CascadeLight : public C_BaseEntity {
public:
	get_netvar( Vector, get_shadow_direction, XORSTR( "DT_CascadeLight" ), XORSTR( "m_shadowDirection" ), NULL );
	get_netvar( Vector, get_env_light_shadow_direction, XORSTR( "DT_CascadeLight" ), XORSTR( "m_envLightShadowDirection" ), NULL );
	get_netvar( int, get_enabled, XORSTR( "DT_CascadeLight" ), XORSTR( "m_bEnabled" ), NULL );
	get_netvar( int, get_use_light_env_angles, XORSTR( "DT_CascadeLight" ), XORSTR( "m_bUseLightEnvAngles" ), NULL );
	get_netvar( int, get_light_color, XORSTR( "DT_CascadeLight" ), XORSTR( "m_LightColor" ), NULL );
	get_netvar( int, get_light_color_scale, XORSTR( "DT_CascadeLight" ), XORSTR( "m_LightColorScale" ), NULL );
	get_netvar( float, get_max_shadow_dist, XORSTR( "DT_CascadeLight" ), XORSTR( "m_flMaxShadowDist" ), NULL );
};