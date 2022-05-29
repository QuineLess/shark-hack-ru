#include "Networking.hpp"
#include "../Exploits/Exploits.hpp"

inline bool IsVectorValid( Vector vecOriginal, Vector vecCurrent )
{
	Vector vecDelta = vecOriginal - vecCurrent;
	for ( int i = 0; i < 3; i++ )
	{
		if ( fabsf( vecDelta[ i ] ) > 0.03125f )
			return false;
	}

	return true;
}
inline bool IsAngleValid( QAngle vecOriginal, QAngle vecCurrent )
{
	QAngle vecDelta = vecOriginal - vecCurrent;
	for ( int i = 0; i < 3; i++ )
	{
		if ( fabsf( vecDelta[ i ] ) > 0.03125f )
			return false;
	}

	return true;
}
inline bool IsFloatValid( float_t flOriginal, float_t flCurrent )
{
	if ( fabsf( flOriginal - flCurrent ) > 0.03125f )
		return false;

	return true;
}

void C_Networking::OnPacketEnd( CClientState* m_ClientState )
{
	if ( !g_globals->GetLocalPlayer( ) || !g_globals->GetLocalPlayer( )->is_alive( ) )
		return;

	if ( *( int32_t* )( ( DWORD )( m_ClientState ) + 0x16C ) != *( int32_t* )( ( DWORD )( m_ClientState ) + 0x164 ) )
		return;

	return this->RestoreNetvarData( g_client_state->m_nLastOutgoingCommand );
}

void C_Networking::StartNetwork( )
{
	if ( !g_client_state || !g_client_state->m_NetChannel )
		return;

	if ( !g_globals->GetLocalPlayer( ) || !g_globals->GetLocalPlayer( )->is_alive( ) )
		return;

	m_Sequence = g_client_state->m_NetChannel->m_nOutSequenceNr;
}

bool C_Networking::ShouldProcessPacketStart( int nCommand )
{
	if ( !g_globals->GetLocalPlayer( ) || !g_globals->GetLocalPlayer( )->is_alive( ) )
		return true;

	for ( auto m_Cmd = m_CommandList.begin( ); m_Cmd != m_CommandList.end( ); m_Cmd++ )
	{
		if ( *m_Cmd != nCommand )
			continue;

		m_CommandList.erase( m_Cmd );
		return true;
	}

	return false;
}

void C_Networking::FinishNetwork( )
{
	if ( !g_client_state || !g_client_state->m_NetChannel )
		return;

	if ( !g_globals->GetLocalPlayer( ) || !g_globals->GetLocalPlayer( )->is_alive( ) )
		return;

	int32_t iChokedCommand = g_client_state->m_NetChannel->m_nChokedPackets;
	int32_t iSequenceNumber = g_client_state->m_NetChannel->m_nOutSequenceNr;

	g_client_state->m_NetChannel->m_nChokedPackets = 0;
	g_client_state->m_NetChannel->m_nOutSequenceNr = m_Sequence;
		
	g_client_state->m_NetChannel->SendDatagram( NULL );

	g_client_state->m_NetChannel->m_nOutSequenceNr = iSequenceNumber;
	g_client_state->m_NetChannel->m_nChokedPackets = iChokedCommand;
}

void C_Networking::ProcessInterpolation( bool bPostFrame )
{
	if ( !g_globals->GetLocalPlayer( ) || !g_globals->GetLocalPlayer( )->is_alive( ) )
		return;

	if ( !bPostFrame )
	{
		m_FinalPredictedTick = g_globals->GetLocalPlayer( )->m_nFinalPredictedTick( );
		m_flInterp = g_globals->interpolation_amount;

		g_globals->GetLocalPlayer( )->m_nFinalPredictedTick( ) = g_globals->GetLocalPlayer( )->get_tick_base( );
		if ( g_ExploitSystem->CanSkipInterpolation( ) )
			g_globals->interpolation_amount = 0.0f;
		
		return;
	}

	g_globals->GetLocalPlayer( )->m_nFinalPredictedTick( ) = m_FinalPredictedTick;
	g_globals->interpolation_amount = 0.0f;
}

void C_Networking::SaveNetvarData( int nCommand )
{
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_nTickbase				= g_globals->GetLocalPlayer( )->get_tick_base( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_flDuckAmount				= g_globals->GetLocalPlayer( )->m_flDuckAmount( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_flDuckSpeed				= g_globals->GetLocalPlayer( )->m_flDuckSpeed( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_vecOrigin				= g_globals->GetLocalPlayer( )->get_origin( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_vecVelocity				= g_globals->GetLocalPlayer( )->get_velocity( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_vecBaseVelocity			= g_globals->GetLocalPlayer( )->m_vecBaseVelocity( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_flFallVelocity			= g_globals->GetLocalPlayer( )->get_fall_velocity( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_vecViewOffset			= g_globals->GetLocalPlayer( )->m_vecViewOffset( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_angAimPunchAngle			= g_globals->GetLocalPlayer( )->get_punch( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_vecAimPunchAngleVel		= g_globals->GetLocalPlayer( )->m_aimPunchAngleVel( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_angViewPunchAngle		= g_globals->GetLocalPlayer( )->get_view_punch( );
}

void C_Networking::RestoreNetvarData( int nCommand )
{
	auto aNetVars = &m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ];
	if ( aNetVars->m_nTickbase != g_globals->GetLocalPlayer( )->get_tick_base( ) )
		return;

	if ( IsVectorValid( aNetVars->m_vecVelocity, g_globals->GetLocalPlayer( )->get_velocity( ) ) )
		g_globals->GetLocalPlayer( )->get_velocity( )							=	aNetVars->m_vecVelocity;

	if ( IsVectorValid( aNetVars->m_vecBaseVelocity, g_globals->GetLocalPlayer( )->m_vecBaseVelocity( ) ) )
		g_globals->GetLocalPlayer( )->m_vecBaseVelocity( )						=	aNetVars->m_vecBaseVelocity;
	
	if ( std::fabsf( g_globals->GetLocalPlayer( )->m_vecViewOffset( ).z - aNetVars->m_vecViewOffset.z ) <= 0.25f )
		g_globals->GetLocalPlayer( )->m_vecViewOffset( )						=	aNetVars->m_vecViewOffset;

	if ( IsAngleValid( aNetVars->m_angAimPunchAngle, g_globals->GetLocalPlayer( )->get_punch( ) ) )
		g_globals->GetLocalPlayer( )->get_punch( )								=	aNetVars->m_angAimPunchAngle;
		
	if ( IsVectorValid( aNetVars->m_vecAimPunchAngleVel, g_globals->GetLocalPlayer( )->m_aimPunchAngleVel( ) ) )
		g_globals->GetLocalPlayer( )->m_aimPunchAngleVel( )						=	aNetVars->m_vecAimPunchAngleVel;

	if ( IsAngleValid( aNetVars->m_angViewPunchAngle, g_globals->GetLocalPlayer( )->get_view_punch( ) ) )
		g_globals->GetLocalPlayer( )->get_view_punch( )							=	aNetVars->m_angViewPunchAngle;

	if ( std::fabsf( aNetVars->m_flFallVelocity - g_globals->GetLocalPlayer( )->get_fall_velocity( ) ) <= 0.5f )
		g_globals->GetLocalPlayer( )->get_fall_velocity( )						=	aNetVars->m_flFallVelocity;

	if ( IsFloatValid( aNetVars->m_flDuckAmount, g_globals->GetLocalPlayer( )->m_flDuckAmount( ) ) )
		g_globals->GetLocalPlayer( )->m_flDuckAmount( )							=	aNetVars->m_flDuckAmount;

	if ( IsFloatValid( aNetVars->m_flDuckSpeed, g_globals->GetLocalPlayer( )->m_flDuckSpeed( ) ) )
		g_globals->GetLocalPlayer( )->m_flDuckSpeed( )							=	aNetVars->m_flDuckSpeed;

	if ( g_globals->GetLocalPlayer( )->m_vecViewOffset( ).z > 64.0f )
		g_globals->GetLocalPlayer( )->m_vecViewOffset( ).z = 64.0f;
	else if ( g_globals->GetLocalPlayer( )->m_vecViewOffset( ).z <= 46.05f )
		g_globals->GetLocalPlayer( )->m_vecViewOffset( ).z = 46.0f;
}

void C_Networking::UpdateLatency( )
{
	INetChannelInfo* pNetChannelInfo = g_engine->GetNetChannel( );
	if ( !pNetChannelInfo )
		return;

	m_Latency = pNetChannelInfo->GetLatency( 0 ) + pNetChannelInfo->GetLatency( 1 );
}

int32_t C_Networking::GetServerTick( )
{	
	int32_t nExtraChoke = 0;
	if ( g_FuckThisSDK->m_bFakeDuck )
		nExtraChoke = 14 - g_client_state->chokedcommands;

	return g_globals->tickcount + ( int ) ( ( ( m_Latency ) / g_globals->interval_per_tick ) + 0.5f ) + nExtraChoke;
}

int32_t C_Networking::GetTickRate( )
{
	return ( int32_t )( 1.0f / g_globals->interval_per_tick );
}

float_t C_Networking::GetLatency( )
{
	return m_Latency;
}

void C_Networking::ResetData( )
{
	m_Latency = 0.0f;
	m_TickRate = 0;

	m_aCompressData = { };
}