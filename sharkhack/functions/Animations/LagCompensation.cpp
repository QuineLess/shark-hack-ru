#include "LagCompensation.hpp"
#include "Animations.hpp"
#include "../Networking/Networking.hpp"
#include "../Exploits/Exploits.hpp"
#include "../RageBot/RageBot.hpp"

void C_LagCompensation::Instance( )
{
	for ( int32_t iPlayerID = 1; iPlayerID <= g_globals->max_clients; iPlayerID++ )
	{
		C_BasePlayer* pPlayer = static_cast < C_BasePlayer* > ( g_ent_list->GetClientEntity( iPlayerID ) );
		if ( !pPlayer || !pPlayer->is_player( ) || !pPlayer->is_alive( ) || pPlayer->get_team( ) == g_globals->GetLocalPlayer( )->get_team( ) )
		{
			g_AnimationSync->MarkAsDormant( iPlayerID );

			g_FuckThisSDK->m_CachedPlayerRecords[ iPlayerID ].clear( );
			continue;
		}

		if ( pPlayer->IsDormant( ) )
		{
			g_AnimationSync->MarkAsDormant( iPlayerID );
			continue;
		}

		if ( g_AnimationSync->HasLeftOutOfDormancy( iPlayerID ) )
			g_FuckThisSDK->m_CachedPlayerRecords[ iPlayerID ].clear( );

		if ( pPlayer->m_flOldSimulationTime( ) >= pPlayer->m_flSimulationTime( ) )
		{
			if ( pPlayer->m_flOldSimulationTime( ) > pPlayer->m_flSimulationTime( ) )
				g_FuckThisSDK->m_CachedPlayerRecords[ iPlayerID ].clear( );

			continue;
		}
		
		bool bHasPreviousRecord = false;
		if ( g_FuckThisSDK->m_CachedPlayerRecords[ iPlayerID ].empty( ) )
			bHasPreviousRecord = false;
		else if ( TIME_TO_TICKS( fabs( pPlayer->m_flSimulationTime( ) - g_FuckThisSDK->m_CachedPlayerRecords[ iPlayerID ].back( ).m_SimulationTime ) ) <= 17 )
			bHasPreviousRecord = true;

		C_LagRecord LagRecord;
		this->FillRecord( pPlayer, LagRecord );

		this->UnsetBreakingLagCompensation( iPlayerID );
		if ( bHasPreviousRecord )
		{
			if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).m_flCycle == this->GetPreviousRecord( iPlayerID ).m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).m_flCycle )
				continue;

			g_AnimationSync->SetPreviousRecord( iPlayerID, this->GetPreviousRecord( iPlayerID ) );
			if ( ( LagRecord.m_Origin - this->GetPreviousRecord( iPlayerID ).m_Origin ).LengthSqr( ) > 4096.0f )
			{
				this->SetBreakingLagCompensation( iPlayerID );
				this->ClearRecords( iPlayerID );
			}
		}

		g_FuckThisSDK->m_CachedPlayerRecords[ iPlayerID ].emplace_back( LagRecord );
		while ( g_FuckThisSDK->m_CachedPlayerRecords[ iPlayerID ].size( ) > 32 )
			g_FuckThisSDK->m_CachedPlayerRecords[ iPlayerID ].pop_front( );
	}
}

void C_LagCompensation::FillRecord( C_BasePlayer* pPlayer, C_LagRecord& LagRecord )
{
	std::memcpy( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).data( ), pPlayer->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * 13 );

	LagRecord.m_UpdateDelay = 1;
	LagRecord.m_Flags = pPlayer->get_flags( );
	LagRecord.m_EyeAngles = pPlayer->m_angEyeAngles( );
	LagRecord.m_LowerBodyYaw = pPlayer->m_flLowerBodyYaw( );
	LagRecord.m_Mins = pPlayer->GetCollideable( )->OBBMins( );
	LagRecord.m_Maxs = pPlayer->GetCollideable( )->OBBMaxs( );
	LagRecord.m_SimulationTime = pPlayer->m_flSimulationTime( );
	LagRecord.m_Origin = pPlayer->get_origin( );
	LagRecord.m_AbsOrigin = pPlayer->get_abs_origin( );
	LagRecord.m_DuckAmount = pPlayer->m_flDuckAmount( );
	LagRecord.m_Velocity = pPlayer->get_velocity( );

	if ( pPlayer->get_weapon_handle( ).Get( ) )
		if ( pPlayer->get_weapon_handle( )->m_flLastShotTime( ) <= pPlayer->m_flSimulationTime( ) )
			if ( pPlayer->get_weapon_handle( )->m_flLastShotTime( ) > pPlayer->m_flOldSimulationTime( ) )
				LagRecord.m_bIsShooting = true;
}

void C_LagCompensation::ResetData( )
{
	for ( int32_t iPlayerID = 0; iPlayerID < 65; iPlayerID++ )
	{
		if ( !g_FuckThisSDK->m_CachedPlayerRecords[ iPlayerID ].empty( ) )
			g_FuckThisSDK->m_CachedPlayerRecords[ iPlayerID ].clear( );

		m_BreakingLagcompensation[ iPlayerID ] = false;	
	}
}

bool C_LagCompensation::IsValidTime( float_t flTime )
{
	int32_t nTickbase = g_globals->GetLocalPlayer( )->get_tick_base( );
	if ( nTickbase <= 0 )
		return false;
	
	float_t flDeltaTime = fmin( g_Networking->GetLatency( ) + this->GetLerpTime( ), 0.2f ) - TICKS_TO_TIME( nTickbase - TIME_TO_TICKS( flTime ) );
	if ( fabs( flDeltaTime ) > 0.2f )
		return false;
		
	int32_t nDeadTime = TICKS_TO_TIME( g_Networking->GetServerTick( ) ) - 0.2f;
	if ( TIME_TO_TICKS( flTime + this->GetLerpTime( ) ) < nDeadTime )
		return false;

	return true;
}

float_t C_LagCompensation::GetLerpTime( )
{
	static auto m_SvMinUpdateRate              =   g_cvar->FindVar( ( "sv_minupdaterate" ) );
	static auto m_SvMaxUpdateRate              =   g_cvar->FindVar( ( "sv_maxupdaterate" ) );
	static auto m_SvClientMinInterpRatio       =   g_cvar->FindVar( ( "sv_client_min_interp_ratio" ) );
	static auto m_SvClientMaxInterpRatio       =   g_cvar->FindVar( ( "sv_client_max_interp_ratio" ) );
	static auto m_ClInterp                     =   g_cvar->FindVar( ( "cl_interp" ) );
	static auto m_ClInterpRatio                =   g_cvar->FindVar( ( "cl_interp_ratio" ) );
	static auto m_ClUpdateRate                 =   g_cvar->FindVar( ( "cl_updaterate" ) );

	float_t flUpdateRate = std::clamp( m_ClUpdateRate->GetFloat( ), m_SvMinUpdateRate->GetFloat( ), m_SvMaxUpdateRate->GetFloat( ) );
	float_t flLerpRatio = std::clamp( m_ClInterpRatio->GetFloat( ), m_SvClientMinInterpRatio->GetFloat( ), m_SvClientMaxInterpRatio->GetFloat( ) );
	return std::clamp( flLerpRatio / flUpdateRate, m_ClInterp->GetFloat( ), 1.0f );
}