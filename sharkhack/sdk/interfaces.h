#pragma once
#include <Windows.h>
#include <deque>
#include <time.h>

#pragma comment(lib, "winmm.lib")

#define get_virtual(_class, index) (*reinterpret_cast<void***>(_class))[index]
#define get_virtual_fn( index, function_name, type, ... )  auto function_name { return CallVFunction<type>( this, index )( this, __VA_ARGS__ ); };
#define get_static_offset( type, func, offset ) type& func() { return *reinterpret_cast< type* >( reinterpret_cast< uintptr_t >( this ) + offset ); }
#define get_p_static_offset( type, ptr, offset ) ( *( type* ) ( ( std::uintptr_t ) (ptr) + ( offset ) ) )

#include "../utils/memory.h"

//interface
#include "interfaces/ibaseclient.h"
#include "interfaces/iappsystem.h"
#include "interfaces/clientclass.h"
#include "interfaces/cinput.h"
#include "interfaces/ipanel.h"
#include "interfaces/clientmode.h"
#include "interfaces/icliententitylist.h"
#include "interfaces/icliententity.h"
#include "interfaces/ivengineclient.h"
#include "interfaces/isurface.h"
#include "interfaces/ienginetrace.h"
#include "interfaces/icollideable.h"
#include "interfaces/iserver.h"
#include "interfaces/ienginesound.h"
#include "interfaces/imaterialsystem.h"
#include "interfaces/imaterial.h"
#include "interfaces/icvar.h"
#include "interfaces/ilocalize.h"
#include "interfaces/ivmodelinfo.h"
#include "interfaces/clientstate.h"
#include "interfaces/igameevent.h"
#include "interfaces/igameeventmanager.h"
#include "interfaces/ceffects.h"
#include "interfaces/iglobalvars.h"
#include "interfaces/ibasefilesystem.h"
#include "interfaces/istudiorender.h"
#include "interfaces/cgamemovement.h"
#include "interfaces/cprediction.h"
#include "interfaces/cmovehelper.h"
#include "interfaces/iweaponsystem.h"
#include "interfaces/glowmanager.h"
#include "interfaces/iphysics.h"
#include "interfaces/iviewrenderbeams.h"
#include "interfaces/csplayerresource.h"
#include "interfaces/citemsystem.h"
#include "interfaces/ceconitem.h"
#include "interfaces/ceconitemview.h"
#include "interfaces/cplayerinventory.h"
#include "interfaces/csinventorymanager.h"
#include "Interfaces/igametypes.h"
#include "Interfaces/idemoplayer.h"
#include "Interfaces/cuienginesource2.h"
#include "interfaces/isteamgamecoordinator.h"
#include "interfaces/imemalloc.h"
#include "interfaces/imatchframework.h"
#include "interfaces/ivdebugoverlay.h"
#include "interfaces/iinputsystem.h"
#include "interfaces/cmergedmdl.h"
#include "interfaces/ivmodelrender.h"
#include "interfaces/irenderview.h"
#include "interfaces/ipanoramaengine.h"

#include "interfaces/studio.h"
#include "interfaces/vector.h"
#include "interfaces/qangle.h"

#include "utils/classids.h"
#include "utils/weaponids.h"
#include "utils/key_values.h"
#include "utils/message.h"
#include "utils/writer.h"

#include "../config/config.h"
#include "../config_manager/config_manager.h"

#include "../utils/xor_str.h"

typedef bool( __thiscall* GetBool_t ) ( LPVOID );
typedef void( __thiscall* PacketEnd_t ) ( LPVOID );
typedef void( __thiscall* PacketStart_t )( LPVOID, int32_t, int32_t );
typedef void( __cdecl* CL_Move_t )( float_t, bool );
typedef void( __thiscall* ModifyEyePosition_t )( LPVOID, Vector& );
typedef void( __thiscall* DoExtraBoneProcessing_t )( LPVOID );
typedef bool( __thiscall* SetupBones_t )( LPVOID, LPVOID, int32_t, int32_t, float_t );
typedef void( __thiscall* StandardBlendingRules_t )( LPVOID, CStudioHdr*, Vector*, Quaternion*, float_t, int32_t );
typedef bool( __thiscall* SendNetMessage_t )( LPVOID, INetMessage&, bool, bool );
typedef void( __thiscall* FireEvents_t ) ( LPVOID );
typedef bool( __thiscall* WriteUsercmdDeltaToBuffer_t ) ( LPVOID, int32_t, bf_write*, int32_t, int32_t, bool );
typedef void( __thiscall* ProcessMovement_t )( LPVOID, LPVOID, CMoveData* );
typedef int32_t(__thiscall* ListLeavesInBox_t)(LPVOID, const Vector&, const Vector&, unsigned short*, int32_t);

enum ROTATE_MODE
{
	ROTATE_SERVER,
	ROTATE_LEFT,
	ROTATE_CENTER,
	ROTATE_RIGHT,
	ROTATE_LOW_LEFT,
	ROTATE_LOW_RIGHT
};

enum ANTIAIM_MODE
{
	ANTIAIM_FULL = 1,
	ANTIAIM_DESYNC
};

enum AUTOSTOP
{
	AUTOSTOP_ACCURACY,
	AUTOSTOP_EARLY
};

enum AIM_POINTS
{
	PREFER_SAFE,
	FORCE_SAFE,
	PREFER_BODY,
	FORCE_BODY
};

enum DOUBLETAP_OPTIONS
{
	MOVE_BETWEEN_SHOTS,
	FULL_STOP
};

enum FAKELAG_TRIGGERS
{
	FAKELAG_MOVE,
	FAKELAG_AIR,
	FAKELAG_PEEK
};

enum RAGE_WEAPON
{
	AUTO = 0,
	SCOUT,
	AWP,
	DEAGLE,
	REVOLVER,
	PISTOL,
	RIFLE,
	SHOTGUN,
	SMG
};


struct IDirect3DDevice9;

inline IBaseClientDLL* g_client;
inline CGlobalVarsBase* g_globals;
inline CInput* g_input;
inline IPanel* g_panel;
inline IClientMode* g_client_mode;
inline IClientEntityList* g_ent_list;
inline IVEngineClient* g_engine;
inline IDirect3DDevice9* g_device;
inline ISurface* g_surface;
inline IServerGameDLL* g_server;
inline IEngineSound* g_sound;
inline uintptr_t* g_spatial_partition;
inline IMaterialSystem* g_mat_system;
inline ICvar* g_cvar;
inline ILocalize* g_localize;
inline CSInventoryManager* g_inventory_manager;
inline CPlayerInventory* g_player_inventory;
inline IVModelInfo* g_mdl_info;
inline CClientState* g_client_state;
inline IGameEventManager2* g_game_events;
inline IBaseFileSystem* g_base_file_sys;
inline IStudioRender* g_studio_render;
inline CEffects* g_effects;
inline IEngineTrace* g_engine_trace;
inline CGameMovement* g_game_movement;
inline CPrediction* g_prediction;
inline CMoveData* g_move_data;
inline CMoveHelper* g_move_helper;
inline uintptr_t* g_prediction_random_seed;
inline IWeaponSystem* g_weapon_system;
inline CGlowObjectManager* g_glow_manager;
inline IPhysicsSurfaceProps* g_physprops;
inline IFileSystem* g_file_sys;
inline IViewRenderBeams* g_view_render_beams;
inline CSPlayerResource** g_player_resource;
inline IGameTypes* g_game_types;
inline IDemoPlayer* g_demo_player;
inline uintptr_t g_random_seed;
inline uintptr_t g_random_float;
inline uintptr_t* g_panorama_marshall_helper;
inline CUIEngineSource2* g_ui_engine_source2;
inline ISteamGameCoordinator* g_steam_game_coordinator;
inline IMemAlloc* g_mem_alloc;
inline IMatchFramework* g_match_framework;
inline IVDebugOverlay* g_debug_overlay;
inline IInputSystem* g_input_sys;
inline IPanoramaUIEngine* g_panorama_engine;
inline IVModelRender* g_model_render;
inline IRenderView* g_render_view;
inline i_input_system* g_input_system;

typedef void( __thiscall* FrameStageNotify_t )( void*, int );
namespace interfaces {
	void on_init();
}

#define TIME_TO_TICKS( flTime ) ( ( int32_t )( 0.5f + ( float_t )( ( flTime ) ) / g_globals->interval_per_tick ) )
#define TICKS_TO_TIME( iTick ) ( float_t )( ( iTick ) * g_globals->interval_per_tick )
#define get_play_sound( sound ) g_engine->ClientCmdUnrestricted(std::string("play " + std::string(sound)).c_str());
#define get_custom_play_sound( sound ) PlaySoundA((char*)sound, NULL, SND_ASYNC | SND_MEMORY);

class C_AnimationLayer
{
public:
	void* m_pThis;			 //0x0000
	float			m_flAnimationTime;	 //0x0004
	float			m_flFadeoutTime;	 //0x0008
	int				m_nFlags;			 //0x001C
	int				m_iActivity;		 //0x0010
	int				m_nOrder;			 //0x0014
	int				m_nSequence;		 //0x0018
	float			m_flPrevCycle;		 //0x001C
	float			m_flWeight;			 //0x0020
	float			m_flWeightDeltaRate; //0x0024
	float			m_flPlaybackRate;	 //0x0028
	float			m_flCycle;			 //0x002C
	void* m_pOwner;			 //0x0030
	int				m_nBitFlags;		 //0x0034
};

#pragma pack(push, 1)
struct C_AimLayer
{
	float m_flUnknown0;
	float m_flTotalTime;
	float m_flUnknown1;
	float m_flUnknown2;
	float m_flWeight;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct C_AimLayers
{
	C_AimLayer layers[ 3 ];
};
#pragma pack(pop)

struct AnimstatePose_t
{
	bool		m_bInitialized;
	int			m_nIndex;
	const char* m_szName;

	AnimstatePose_t( )
	{
		m_bInitialized = false;
		m_nIndex = -1;
		m_szName = "";
	}
};

struct procedural_foot_t
{
	Vector m_vecPosAnim;
	Vector m_vecPosAnimLast;
	Vector m_vecPosPlant;
	Vector m_vecPlantVel;
	float m_flLockAmount;
	float m_flLastPlantTime;
};
class C_CSGOPlayerAnimationState
{
public:
	void* m_pThis;
	bool					m_bIsReset;
	bool					m_bUnknownClientBoolean;
	char					m_aSomePad[ 2 ];
	int						m_nTick;
	float					m_flFlashedStartTime;
	float					m_flFlashedEndTime;
	C_AimLayers				m_AimLayers;
	int						m_iModelIndex;
	int						m_iUnknownArray[ 3 ];
	C_BasePlayer* m_pBasePlayer;
	LPVOID					m_pWeapon;
	LPVOID					m_pWeaponLast;
	float					m_flLastUpdateTime;
	int						m_nLastUpdateFrame;
	float					m_flLastUpdateIncrement;
	float					m_flEyeYaw;
	float					m_flEyePitch;
	float					m_flFootYaw;
	float					m_flFootYawLast;
	float					m_flMoveYaw;
	float					m_flMoveYawIdeal;
	float					m_flMoveYawCurrentToIdeal;
	float					m_flTimeToAlignLowerBody;
	float					m_flPrimaryCycle;
	float					m_flMoveWeight;
	float					m_flMoveWeightSmoothed;
	float					m_flAnimDuckAmount;
	float					m_flDuckAdditional;
	float					m_flRecrouchWeight;
	Vector					m_vecPositionCurrent;
	Vector					m_vecPositionLast;
	Vector					m_vecVelocity;
	Vector					m_vecVelocityNormalized;
	Vector					m_vecVelocityNormalizedNonZero;
	float					m_flVelocityLengthXY;
	float					m_flVelocityLengthZ;
	float					m_flSpeedAsPortionOfRunTopSpeed;
	float					m_flSpeedAsPortionOfWalkTopSpeed;
	float					m_flSpeedAsPortionOfCrouchTopSpeed;
	float					m_flDurationMoving;
	float					m_flDurationStill;
	bool					m_bOnGround;
	bool					m_bLanding;
	char					m_pad[ 2 ];
	float					m_flJumpToFall;
	float					m_flDurationInAir;
	float					m_flLeftGroundHeight;
	float					m_flLandAnimMultiplier;
	float					m_flWalkToRunTransition;
	bool					m_bLandedOnGroundThisFrame;
	bool					m_bLeftTheGroundThisFrame;
	float					m_flInAirSmoothValue;
	bool					m_bOnLadder;
	float					m_flLadderWeight;
	float					m_flLadderSpeed;
	bool					m_bWalkToRunTransitionState;
	bool					m_bDefuseStarted;
	bool					m_bPlantAnimStarted;
	bool					m_bTwitchAnimStarted;
	bool					m_bAdjustStarted;
	char					m_ActivityModifiers[ 20 ];
	float					m_flNextTwitchTime;
	float					m_flTimeOfLastKnownInjury;
	float					m_flLastVelocityTestTime;
	Vector					m_vecVelocityLast;
	Vector					m_vecTargetAcceleration;
	Vector					m_vecAcceleration;
	float					m_flAccelerationWeight;
	float					m_flAimMatrixTransition;
	float					m_flAimMatrixTransitionDelay;
	bool					m_bFlashed;
	float					m_flStrafeChangeWeight;
	float					m_flStrafeChangeTargetWeight;
	float					m_flStrafeChangeCycle;
	int						m_nStrafeSequence;
	bool					m_bStrafeChanging;
	float					m_flDurationStrafing;
	float					m_flFootLerp;
	bool					m_bFeetCrossed;
	bool					m_bPlayerIsAccelerating;
	AnimstatePose_t			m_tPoseParamMappings[ 20 ];
	float					m_flDurationMoveWeightIsTooHigh;
	float					m_flStaticApproachSpeed;
	int						m_nPreviousMoveState;
	float					m_flStutterStep;
	float					m_flActionWeightBiasRemainder;
	procedural_foot_t		m_footLeft;
	procedural_foot_t		m_footRight;
	float					m_flCameraSmoothHeight;
	bool					m_bSmoothHeightValid;
	float					m_flLastTimeVelocityOverTen;
	float					m_flFuckValve;
	float					m_flAimYawMin;
	float					m_flAimYawMax;
	float					m_flAimPitchMin;
	float					m_flAimPitchMax;
	int						m_nAnimstateModelVersion;

	int32_t SelectSequenceFromActivityModifier( int32_t iActivity );
	bool IsLayerSequenceFinished( C_AnimationLayer* pAnimationLayer, float_t flTime );
	void SetLayerSequence( C_AnimationLayer* pAnimationLayer, int32_t iActivity );
	void SetLayerCycle( C_AnimationLayer* pAnimationLayer, float_t flCycle );
};

class C_LagRecord
{
public:
	float_t m_SimulationTime = 0.0f;
	float_t m_LowerBodyYaw = 0.0f;
	float_t m_DuckAmount = 0.0f;
	float_t m_BruteYaw = 0.0f;

	int32_t m_UpdateDelay = 0;
	int32_t m_RotationMode = ROTATE_SERVER;
	int32_t m_Flags = 0;
	int32_t m_AdjustTick = 0;

	QAngle m_EyeAngles = QAngle( 0, 0, 0 );
	QAngle m_AbsAngles = QAngle( 0, 0, 0 );
	Vector m_Velocity = Vector( 0, 0, 0 );
	Vector m_Origin = Vector( 0, 0, 0 );
	Vector m_AbsOrigin = Vector( 0, 0, 0 );
	Vector m_Mins = Vector( 0, 0, 0 );
	Vector m_Maxs = Vector( 0, 0, 0 );

	std::array < std::array < C_AnimationLayer, 13 >, 6 > m_AnimationLayers = { };
	std::array < float_t, 24 > m_PoseParameters = { };
	std::array < std::array < matrix3x4_t, MAXSTUDIOBONES >, 4 > m_Matricies = { };

	bool m_bIsShooting = false;
	bool m_bAnimResolved = false;
	bool m_bJumped = false;
};
class C_HitboxData
{
public:
	int32_t m_iHitbox = -1;
	float_t m_flDamage = 0.0f;
	Vector m_vecPoint = Vector( 0, 0, 0 );
	bool m_bIsSafeHitbox = false;
	bool m_bForcedToSafeHitbox = false;

	C_BasePlayer* pPlayer = NULL;
	C_LagRecord LagRecord = C_LagRecord( );
};
class C_HitboxHitscanData
{
public:
	C_HitboxHitscanData( int32_t iHitbox = -1, bool bForceSafe = false )
	{
		this->m_iHitbox = iHitbox;
		this->m_flWeaponDamage = 0.0f;
		this->m_bForceSafe = bForceSafe;
	}

	int32_t m_iHitbox = -1;
	float_t m_flWeaponDamage = 0.0f;
	bool m_bForceSafe = false;
};
class C_TargetData
{
public:
	C_BasePlayer* m_Player = NULL;
	C_LagRecord m_LagRecord = C_LagRecord( );
	C_HitboxData m_Hitbox = C_HitboxData( );
};
class C_ShotData
{
public:
	C_TargetData m_Target = C_TargetData( );

	Vector m_vecStartPosition = Vector( 0, 0, 0 );
	Vector m_vecEndPosition = Vector( 0, 0, 0 );

	int32_t m_iShotTick = 0;
	float m_Damage = 0;

	bool m_bDidIntersectPlayer = false;
	bool m_bHasBeenFired = false;
	bool m_bHasBeenRegistered = false;
	bool m_bHasBeenHurted = false;
	bool m_bHasMaximumAccuracy = false;

	std::vector < Vector > m_vecImpacts = { };
};
struct FuckThisSDK_t
{
	CUserCmd* m_pCmd = NULL;
	bool* m_pbSendPacket = NULL;

	C_BasePlayer* m_Player = nullptr;
	bool m_bFakeDuck = false;
	bool m_bVisualFakeDuck = false;
	bool m_bIsPeeking = false;

	struct
	{
		std::array < C_AnimationLayer, 13 > GetAnimationLayers;
		std::array < C_AnimationLayer, 13 > m_FakeAnimationLayers;

		std::array < float_t, 24 > m_PoseParameters;
		std::array < float_t, 24 > m_FakePoseParameters;
		C_CSGOPlayerAnimationState m_FakeAnimationState;

		float_t m_flSpawnTime = 0.0f;
		float_t m_flLowerBodyYaw = 0.0f;
		float_t m_flNextLowerBodyYawUpdateTime = 0.0f;

		std::array < int32_t, 2 > m_iMoveType;
		std::array < int32_t, 2 > m_iFlags;

		std::array < Vector, MAXSTUDIOBONES > m_vecBoneOrigins;
		std::array < Vector, MAXSTUDIOBONES > m_vecFakeBoneOrigins;

		Vector m_vecNetworkedOrigin = Vector( 0, 0, 0 );
		Vector m_vecShootPosition = Vector( 0, 0, 0 );

		bool m_bDidShotAtChokeCycle = false;
		QAngle m_angShotChokedAngle = QAngle( 0, 0, 0 );

		float_t m_flShotTime = 0.0f;
		QAngle m_angForcedAngles = QAngle( 0, 0, 0 );

		QAngle m_angFakeAngles = QAngle( 0, 0, 0 );

		std::array < matrix3x4_t, MAXSTUDIOBONES > m_aMainBones;
		std::array < matrix3x4_t, MAXSTUDIOBONES > m_aDesyncBones;
		std::array < matrix3x4_t, MAXSTUDIOBONES > m_aLagBones;
	} m_LocalData;

	struct
	{
		bool m_bAnimationUpdate = false;
		bool m_bSetupBones = false;
	} m_AnimationData;

	struct
	{
		std::array < int32_t, 65 > m_MissedShots = { };
		std::array < int32_t, 65 > m_LastMissedShots = { };
		std::array < int32_t, 65 > m_BruteSide = { };
		std::array < int32_t, 65 > m_LastBruteSide = { };
		std::array < int32_t, 65 > m_LastTickbaseShift = { };
		std::array < float_t, 65 > m_LastValidTime = { };
		std::array < Vector, 65 > m_LastValidOrigin = { };
		std::array < bool, 65 > m_AnimResoled = { };
		std::array < bool, 65 > m_FirstSinceTickbaseShift = { };
	} m_ResolverData;
	std::deque < C_ShotData > m_ShotData;
	struct
	{
		std::array < float_t, 256 > m_aInaccuracy;
		std::array < float_t, 256 > m_aSpread;
		std::array < float_t, 256 > m_FirstRandom;
		std::array < float_t, 256 > m_SecondRandom;

		float_t m_flInaccuracy = 0.0f;
		float_t m_flSpread = 0.0f;
		float_t m_flMinInaccuracy = 0.0f;

		bool m_bCanFire_Shift = false;
		bool m_bCanFire_Default = false;
		bool m_bRestoreAutoStop = false;
		bool m_bHasValidAccuracyData = false;
		bool m_bHasMaximumAccuracy = false;
		bool m_bDoingSecondShot = false;
	} m_AccuracyData;
	struct
	{
		int m_TicksToStop = 0;
	} m_MovementData;
	struct
	{
		C_TargetData m_CurrentTarget;
	} m_RageData;

	std::array < std::deque < C_LagRecord >, 65 > m_CachedPlayerRecords;
	int m_nMaxChoke = 14;

	struct
	{
		FrameStageNotify_t m_FrameStageNotify;
		CL_Move_t m_CL_Move = NULL;
		PacketStart_t m_PacketStart = NULL;
		PacketEnd_t m_PacketEnd = NULL;
		ProcessMovement_t m_ProcessMovement = NULL;
		FireEvents_t m_UpdateClientSideAnimation = NULL;
		FireEvents_t m_PhysicsSimulate = NULL;
		SendNetMessage_t m_SendNetMessage = NULL;
		StandardBlendingRules_t m_StandardBlendingRules = NULL;
		SetupBones_t m_SetupBones = NULL;
		ModifyEyePosition_t m_ModifyEyePosition = NULL;
		DoExtraBoneProcessing_t m_DoExtraBoneProcessing = NULL;
		WriteUsercmdDeltaToBuffer_t m_WriteUsercmdDeltaToBuffer = NULL;
		GetBool_t m_IsHLTV = NULL;
		GetBool_t m_IsPaused = NULL;
		GetBool_t m_InPrediction = NULL;
		GetBool_t m_SvCheats_GetBool = NULL;
		GetBool_t m_ClDoResetLatch_GetBool = NULL;
		ListLeavesInBox_t m_ListLeavesInBox = NULL;

	} m_Originals;
};

inline FuckThisSDK_t* g_FuckThisSDK = new FuckThisSDK_t( );