#include "EnginePrediction.hpp"

void C_PredictionSystem::Instance( )
{
	m_flOldCurtime = g_globals->curtime;
	m_flOldFrametime = g_globals->frametime;
	
	m_bInPrediction_Backup = *( bool* )( ( DWORD )( g_prediction ) + 0x8 );
	m_bIsFirstTimePredicted_Backup = *( bool* ) ( ( DWORD ) ( g_prediction ) + 0x18 );
	
	*( bool* )( ( DWORD )( g_prediction ) + 0x8 ) = true;
	*( bool* ) ( ( DWORD ) ( g_prediction ) + 0x18 ) = false;

	return g_PredictionSystem->Repredict( );
}

void C_PredictionSystem::Repredict( )
{
	static void* PredictionPlayer	=   ( void* ) ( ( DWORD ) ( utils::find_signature( ( "client.dll" ), ( "89 35 ? ? ? ? F3 0F 10 48 20" ) ) ) + 0x2 );
	static void* PredictionSeed	=   ( void* ) ( ( DWORD ) ( utils::find_signature( ( "client.dll" ), ( "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04" ) ) ) + 0x2 );

	g_globals->curtime = g_globals->GetLocalPlayer( )->get_tick_base( ) * g_globals->interval_per_tick;
	if ( !( g_globals->GetLocalPlayer( )->get_flags( ) & entity_flags::fl_frozen ) )
		g_globals->frametime = g_globals->interval_per_tick;
	
	Vector vecPreviousOrigin = g_globals->GetLocalPlayer( )->get_abs_origin( );

	g_globals->GetLocalPlayer( )->m_vecAbsVelocity( ) = g_globals->GetLocalPlayer( )->get_velocity( );

	*( CUserCmd** )( ( uintptr_t )( g_globals->GetLocalPlayer( ) ) + 0x3348 ) = g_FuckThisSDK->m_pCmd;
	( **( ( C_BasePlayer*** )( PredictionPlayer ) ) ) = g_globals->GetLocalPlayer( );
	( *( *( int32_t** )( PredictionSeed ) ) ) = g_FuckThisSDK->m_pCmd->random_seed;
	
	g_game_movement->StartTrackPredictionErrors( g_globals->GetLocalPlayer( ) );
	g_move_helper->set_host( g_globals->GetLocalPlayer( ) );

	g_prediction->SetupMove( g_globals->GetLocalPlayer( ), g_FuckThisSDK->m_pCmd, g_move_helper, &m_MoveData );
	g_game_movement->ProcessMovement( g_globals->GetLocalPlayer( ), &m_MoveData );
	g_prediction->FinishMove( g_globals->GetLocalPlayer( ), g_FuckThisSDK->m_pCmd, &m_MoveData );
	
	g_move_helper->set_host( nullptr );
	g_game_movement->FinishTrackPredictionErrors( g_globals->GetLocalPlayer( ) );

	*( CUserCmd** )( ( uintptr_t )( g_globals->GetLocalPlayer( ) ) + 0x3348 ) = NULL;
	( **( ( C_BasePlayer*** )( PredictionPlayer ) ) ) = NULL;
	( *( *( int32_t** )( PredictionSeed ) ) ) = -1;

	m_LastPredictedCmd = g_FuckThisSDK->m_pCmd;
	if ( g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( ) )
		g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( )->update_accuracy_penalty( );

	return g_globals->GetLocalPlayer( )->set_abs_origin( vecPreviousOrigin );
}

void C_PredictionSystem::ResetData( )
{
	m_aNetvarData = { };
	
	m_flOldCurtime = 0.0f;
	m_flOldFrametime = 0.0f;
	m_bInPrediction_Backup = false;
	m_bIsFirstTimePredicted_Backup = false;
	m_LastPredictedCmd = NULL;
	m_MoveData = CMoveData( );
	m_nTickBase = 0;
}

void C_PredictionSystem::SaveNetvars( int32_t nCommand )
{
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_fFlags						= g_globals->GetLocalPlayer( )->get_flags( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_hGroundEntity				= g_globals->GetLocalPlayer( )->m_hGroundEntity( ).Get( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flDuckAmount				= g_globals->GetLocalPlayer( )->m_flDuckAmount( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flDuckSpeed				= g_globals->GetLocalPlayer( )->m_flDuckSpeed( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecOrigin					= g_globals->GetLocalPlayer( )->get_origin( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecVelocity				= g_globals->GetLocalPlayer( )->get_velocity( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecAbsVelocity				= g_globals->GetLocalPlayer( )->m_vecAbsVelocity( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecBaseVelocity			= g_globals->GetLocalPlayer( )->m_vecBaseVelocity( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flFallVelocity				= g_globals->GetLocalPlayer( )->get_fall_velocity( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecViewOffset				= g_globals->GetLocalPlayer( )->m_vecViewOffset( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_angAimPunchAngle			= g_globals->GetLocalPlayer( )->get_punch( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecAimPunchAngleVel		= g_globals->GetLocalPlayer( )->m_aimPunchAngleVel( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_angViewPunchAngle			= g_globals->GetLocalPlayer( )->get_view_punch( );

    C_BaseCombatWeapon* pWeapon = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( );
    if ( !pWeapon )
		return;

	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flRecoilIndex = pWeapon->m_flRecoilIndex( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flAccuracyPenalty = pWeapon->m_fAccuracyPenalty( );
}

void C_PredictionSystem::RestoreNetvars( int32_t nCommand )
{
	g_globals->GetLocalPlayer( )->get_flags( )							= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_fFlags;
    g_globals->GetLocalPlayer( )->m_hGroundEntity( )					= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_hGroundEntity.Get( );
    g_globals->GetLocalPlayer( )->m_flDuckAmount( )						= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flDuckAmount;
    g_globals->GetLocalPlayer( )->m_flDuckSpeed( )						= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flDuckSpeed;
    g_globals->GetLocalPlayer( )->get_origin( )							= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecOrigin;
    g_globals->GetLocalPlayer( )->get_velocity( )						= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecVelocity;
    g_globals->GetLocalPlayer( )->m_vecAbsVelocity( )					= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecAbsVelocity;
    g_globals->GetLocalPlayer( )->m_vecBaseVelocity( )					= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecBaseVelocity;
    g_globals->GetLocalPlayer( )->get_fall_velocity( )					= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flFallVelocity;
    g_globals->GetLocalPlayer( )->m_vecViewOffset( )					= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecViewOffset;
    g_globals->GetLocalPlayer( )->get_punch( )							= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_angAimPunchAngle;
    g_globals->GetLocalPlayer( )->m_aimPunchAngleVel( )					= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecAimPunchAngleVel;
    g_globals->GetLocalPlayer( )->get_view_punch( )						= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_angViewPunchAngle;

    C_BaseCombatWeapon* pWeapon = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( );
    if ( !pWeapon )
        return;
    
    pWeapon->m_flRecoilIndex( ) = m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flRecoilIndex;
    pWeapon->m_fAccuracyPenalty( ) = m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flAccuracyPenalty;
}

void C_PredictionSystem::ResetPacket( )
{
	g_globals->curtime = m_flOldCurtime;
	g_globals->frametime = m_flOldFrametime;

	*( bool* )( ( DWORD )( g_prediction ) + 0x8 ) = m_bInPrediction_Backup;
	*( bool* ) ( ( DWORD ) ( g_prediction ) + 0x18 ) = m_bIsFirstTimePredicted_Backup;
}

void C_PredictionSystem::UpdatePacket( )
{
	if ( g_client_state->m_nDeltaTick < 0 )
		return;

	return g_prediction->Update( g_client_state->m_nDeltaTick, g_client_state->m_nDeltaTick > 0, g_client_state->m_nLastCommandAck, g_client_state->chokedcommands + g_client_state->m_nLastOutgoingCommand );
}

void C_PredictionSystem::SaveViewmodelData( )
{
	C_BaseViewModel* hViewmodel = g_globals->GetLocalPlayer( )->get_view_model( ).Get( );
	if ( !hViewmodel )
		return;

	m_iAnimationParity = hViewmodel->m_iAnimationParity( );
	m_iSequence = hViewmodel->m_iSequence( );
	m_flCycle = hViewmodel->m_flCycle( );
	m_flAnimTime = hViewmodel->m_flAnimTime( );
}

void C_PredictionSystem::AdjustViewmodelData( )
{
	C_BaseViewModel* hViewmodel = g_globals->GetLocalPlayer( )->get_view_model( ).Get( );
	if ( !hViewmodel )
		return;

	if ( m_iSequence != hViewmodel->m_iSequence( ) || m_iAnimationParity != hViewmodel->m_iAnimationParity( ) )
		return;

	hViewmodel->m_flCycle( ) = m_flCycle;
	hViewmodel->m_flAnimTime( ) = m_flAnimTime;
}