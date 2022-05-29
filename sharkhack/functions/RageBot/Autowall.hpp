#pragma once
#include "../../sdk/interfaces.h"
#include "../../utils/entity.h"

#define CHAR_TEX_ANTLION		'A'
#define CHAR_TEX_BLOODYFLESH	'B'
#define	CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_EGGSHELL		'E' ///< the egg sacs in the tunnels in ep2.
#define CHAR_TEX_FLESH			'F'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_ALIENFLESH		'H'
#define CHAR_TEX_CLIP			'I'
#define CHAR_TEX_GRASS			'J'	// L4D addition
#define CHAR_TEX_SNOW			'K'
#define CHAR_TEX_PLASTIC		'L'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_SAND			'N'
#define CHAR_TEX_FOLIAGE		'O'
#define CHAR_TEX_COMPUTER		'P'
#define CHAR_TEX_ASPHALT		'Q'	// L4D addition
#define CHAR_TEX_BRICK			'R'	// L4D addition
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_TILE			'T'
#define CHAR_TEX_CARDBOARD		'U'	// L4D addition
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_WOOD			'W'
//#define CHAR_TEX_UNUSED		'X' ///< do not use - "fake" materials use this (ladders, wading, clips, etc)
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_WARPSHIELD		'Z' ///< wierd-looking jello effect for advisor shield.

#define FIRST_CHAR_TEX			CHAR_TEX_ANTLION
#define LAST_CHAR_TEX			CHAR_TEX_WARPSHIELD

#define CHAR_TEX_CLAY			1 	// L4D addition
#define CHAR_TEX_PLASTER		2	// L4D addition
#define CHAR_TEX_ROCK			3	// L4D addition
#define CHAR_TEX_RUBBER			4	// L4D addition
#define CHAR_TEX_SHEETROCK		5	// L4D addition
#define CHAR_TEX_CLOTH			6	// L4D addition
#define CHAR_TEX_CARPET			7	// L4D addition
#define CHAR_TEX_PAPER			8	// L4D addition
#define CHAR_TEX_UPHOLSTERY		9	// L4D addition
#define CHAR_TEX_PUDDLE			10	// L4D addition
#define CHAR_TEX_MUD			11	// L4D addition
struct PenetrationData_t
{
	Vector m_vecShootPosition = Vector( 0, 0, 0 );
	Vector m_vecTargetPosition = Vector( 0, 0, 0 );
	Vector m_vecDirection = Vector( 0, 0, 0 );

	int32_t m_PenetrationCount = 4;

	float_t m_flPenetrationPower = 0.0f;
	float_t m_flPenetrationDistance = 0.0f;

	float_t m_flDamageModifier = 0.5f;
	float_t m_flPenetrationModifier = 1.0f;

	float_t m_flMaxRange = 0.0f;
	float_t m_flWeaponDamage = 0.0f;
	float_t m_flCurrentDamage = 0.0f;
	float_t m_flCurrentDistance = 0.0f;

	CGameTrace m_EnterTrace;
	CGameTrace m_ExitTrace;

	CCSWeaponInfo* m_WeaponData = NULL;
	C_BaseCombatWeapon* m_Weapon = NULL;
};

class C_AutoWall
{
public:
	virtual bool TraceToExit( Vector vecStart, Vector& vecEnd, CGameTrace* pEnterTrace, CGameTrace* pExitTrace );
	virtual bool HandleBulletPenetration( );
	virtual bool IsArmored( C_BasePlayer* pPlayer, int32_t nHitGroup );
	virtual void ScaleDamage( CGameTrace Trace, float_t& flDamage );
	virtual void ClipTraceToPlayers( C_BasePlayer* pPlayer, Vector vecStart, Vector vecEnd, CGameTrace* Trace, CTraceFilter* Filter, uint32_t nMask );
	virtual Vector GetPointDirection( Vector vecShootPosition, Vector vecTargetPosition );
	virtual float GetPointDamage( Vector vecShootPosition, Vector vecTargetPosition );
	virtual bool SimulateFireBullet( );
	virtual void CacheWeaponData( );
private:
	PenetrationData_t m_PenetrationData;
};

inline C_AutoWall* g_AutoWall = new C_AutoWall( );