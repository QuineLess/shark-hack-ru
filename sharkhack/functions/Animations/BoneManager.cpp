#include "BoneManager.hpp"

void C_BoneManager::BuildMatrix( C_BasePlayer* pPlayer, matrix3x4_t* aMatrix, bool bSafeMatrix )
{
	std::array < C_AnimationLayer, 13 > aAnimationLayers;

	float_t flCurTime = g_globals->curtime;
	float_t flRealTime = g_globals->realtime;
	float_t flFrameTime = g_globals->frametime;
	float_t flAbsFrameTime = g_globals->absoluteframetime;
	int32_t iFrameCount = g_globals->framecount;
	int32_t iTickCount = g_globals->tickcount;
	float_t flInterpolation = g_globals->interpolation_amount;

	g_globals->curtime = pPlayer->m_flSimulationTime( );
	g_globals->realtime = pPlayer->m_flSimulationTime( );
	g_globals->frametime = g_globals->interval_per_tick;
	g_globals->absoluteframetime = g_globals->interval_per_tick;
	g_globals->framecount = INT_MAX;
	g_globals->tickcount = TIME_TO_TICKS( pPlayer->m_flSimulationTime( ) );
	g_globals->interpolation_amount = 0.0f;

	int32_t nClientEffects = pPlayer->m_nClientEffects( );
	int32_t nLastSkipFramecount = pPlayer->m_nLastSkipFramecount( );
	int32_t nOcclusionMask = pPlayer->m_nOcclusionMask( );
	int32_t nOcclusionFrame = pPlayer->m_nOcclusionFrame( );
	int32_t iEffects = pPlayer->m_fEffects( );
	bool bJiggleBones = pPlayer->m_bJiggleBones( );
	bool bMaintainSequenceTransition = pPlayer->m_bMaintainSequenceTransition( );
	Vector vecAbsOrigin = pPlayer->get_abs_origin( );

	int32_t iMask = BONE_USED_BY_ANYTHING;
	if ( bSafeMatrix )
		iMask = BONE_USED_BY_HITBOX;

	std::memcpy( aAnimationLayers.data( ), pPlayer->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * 13 );

	pPlayer->InvalidateBoneCache( );
	pPlayer->GetBoneAccessor( ).m_ReadableBones = NULL;
	pPlayer->GetBoneAccessor( ).m_WritableBones = NULL;

	if ( pPlayer->m_PlayerAnimationStateCSGO( ) )
		pPlayer->m_PlayerAnimationStateCSGO( )->m_pWeaponLast = pPlayer->m_PlayerAnimationStateCSGO( )->m_pWeapon;

	pPlayer->m_nOcclusionFrame( ) = 0;
	pPlayer->m_nOcclusionMask( ) = 0;
	pPlayer->m_nLastSkipFramecount( ) = 0;

	if ( pPlayer != g_globals->GetLocalPlayer( ) )
		pPlayer->set_abs_origin( pPlayer->get_origin( ) );

	pPlayer->m_fEffects( ) |= 8;
	pPlayer->m_nClientEffects( ) |= 2;
	pPlayer->m_bJiggleBones( ) = false;
	pPlayer->m_bMaintainSequenceTransition( ) = false;
	
	if ( pPlayer == g_globals->GetLocalPlayer( ) )
	{
		pPlayer->GetAnimationLayers( )[ 12 ].m_flWeight = 0.0f;
		if ( pPlayer->GetSequenceActivity( pPlayer->GetAnimationLayers( )[ 3 ].m_nSequence ) == ACT_CSGO_IDLE_TURN_BALANCEADJUST )
		{
			pPlayer->GetAnimationLayers( )[ 3 ].m_flCycle = 0.0f;
			pPlayer->GetAnimationLayers( )[ 3 ].m_flWeight = 0.0f;
		}
	}

	g_FuckThisSDK->m_AnimationData.m_bSetupBones = true;
	pPlayer->SetupBones( aMatrix, MAXSTUDIOBONES, iMask, pPlayer->m_flSimulationTime( ) );
	g_FuckThisSDK->m_AnimationData.m_bSetupBones = false;

	pPlayer->m_bMaintainSequenceTransition( ) = bMaintainSequenceTransition;
	pPlayer->m_nClientEffects( ) = nClientEffects;
	pPlayer->m_bJiggleBones( ) = bJiggleBones;
	pPlayer->m_fEffects( ) = iEffects;
	pPlayer->m_nLastSkipFramecount( ) = nLastSkipFramecount;
	pPlayer->m_nOcclusionFrame( ) = nOcclusionFrame;
	pPlayer->m_nOcclusionMask( ) = nOcclusionMask;
	
	if ( pPlayer != g_globals->GetLocalPlayer( ) )
		pPlayer->set_abs_origin( vecAbsOrigin );

	std::memcpy( pPlayer->GetAnimationLayers( ), aAnimationLayers.data( ), sizeof( C_AnimationLayer ) * 13 );

	g_globals->curtime = flCurTime;
	g_globals->realtime = flRealTime;
	g_globals->frametime = flFrameTime;
	g_globals->absoluteframetime = flAbsFrameTime;
	g_globals->framecount = iFrameCount;
	g_globals->tickcount = iTickCount;
	g_globals->interpolation_amount = flInterpolation;
}