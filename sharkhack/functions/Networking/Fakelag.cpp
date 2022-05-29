#include "Fakelag.hpp"
#include "Networking.hpp"

#include "../Animations/LocalAnimations.hpp"
#include "../Animations/LagCompensation.hpp"
#include "../RageBot/RageBot.hpp"
#include "../Exploits/Exploits.hpp"

void C_FakeLag::Instance( )
{
	if ( g_FuckThisSDK->m_bFakeDuck )
		return;

	int32_t iChokeAmount = -1;
	if ( config.m_bAntiAim )
		iChokeAmount = 1;

	if ( config.m_bFakeLagEnabled )
	{
		iChokeAmount = config.m_iLagLimit;
		if ( g_globals->GetLocalPlayer( )->get_flags( ) & entity_flags::fl_on_ground )
		{
			if ( g_globals->GetLocalPlayer( )->get_velocity( ).Length2D( ) > 23.40f )
				if ( config.m_aFakelagTriggers[ FAKELAG_MOVE ] )
					iChokeAmount = config.m_iTriggerLimit;
		}
		else if ( config.m_aFakelagTriggers[ FAKELAG_AIR ] )
			iChokeAmount = config.m_iTriggerLimit;

		if ( m_iForcedChoke )
			iChokeAmount = m_iForcedChoke;
	}

	if ( g_client_state->chokedcommands < iChokeAmount )
		*g_FuckThisSDK->m_pbSendPacket = false;

	if ( g_client_state->chokedcommands > 13 ) 
		*g_FuckThisSDK->m_pbSendPacket = true;

	C_BaseCombatWeapon* pCombatWeapon = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( );
	if ( pCombatWeapon )
	{
		if ( g_FuckThisSDK->m_pCmd->buttons & IN_ATTACK )
		{
			bool bIsRevolver = false;
			if ( pCombatWeapon )
				if ( pCombatWeapon->get_definition_index( ) == WEAPON_REVOLVER )
					bIsRevolver = true;

			int32_t nShiftAmount = g_ExploitSystem->GetShiftAmount( );
			if ( nShiftAmount > 9 || bIsRevolver )
				nShiftAmount = 0;

			bool bCanFire = g_globals->GetLocalPlayer( )->CanFire( nShiftAmount, bIsRevolver );
			if ( bCanFire )
				*g_FuckThisSDK->m_pbSendPacket = true;
		}
	}

	if ( g_client_state->chokedcommands >= g_FuckThisSDK->m_nMaxChoke )
		*g_FuckThisSDK->m_pbSendPacket = true;
	
	if ( *g_FuckThisSDK->m_pbSendPacket )
		m_iForcedChoke = 0;

	if ( config.m_bFakeLagEnabled )
	{
		if ( config.m_aFakelagTriggers[ FAKELAG_PEEK ] )
		{
			if ( g_FuckThisSDK->m_bIsPeeking )
			{
				if ( !m_iForcedChoke )
					*g_FuckThisSDK->m_pbSendPacket = true;

				m_iForcedChoke = config.m_iTriggerLimit;
			}
		}
	}

	if ( m_bDidForceTicksAllowed )
		return;

	return this->ForceTicksAllowedForProcessing( );
}

void C_FakeLag::ForceTicksAllowedForProcessing( )
{
	*g_FuckThisSDK->m_pbSendPacket = false;
	if ( g_client_state->chokedcommands < 14 )
		return;

	m_bDidForceTicksAllowed = true;
}

void C_FakeLag::ResetData( )
{
	m_bDidForceTicksAllowed = false;
	m_iForcedChoke = NULL;
}