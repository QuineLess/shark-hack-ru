#include "RageBot.hpp"
#include "Autowall.hpp"
#include "Antiaim.hpp"

#include "../Exploits/Exploits.hpp"
#include "../../utils/math.h"
#include "../Prediction/EnginePrediction.hpp"
#include "../Animations/LocalAnimations.hpp"
#include "../Networking/Networking.hpp"

void C_RageBot::Instance( )
{
	if (!m_RageSettings.m_bEnabled)
		return;

	if ( !g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( ) || !g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( )->get_weapon_data( ) )
		return;

	g_FuckThisSDK->m_RageData.m_CurrentTarget = C_TargetData( );
	if ( ( g_globals->GetLocalPlayer( )->get_flags( ) & entity_flags::fl_frozen ) )
		return;

	if ( !g_globals->GetLocalPlayer( )->CanFire( ) )
	{
		g_FuckThisSDK->m_pCmd->buttons &= ~IN_ATTACK;
		return;
	}
	
	if (g_globals->GetLocalPlayer()->get_weapon_handle().Get()->get_ammo() == 0)
		return;

	if ( g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( )->get_definition_index( ) == WEAPON_TASER )
		return this->TaserBot( );
	
	this->AutoRevolver( );
	if ( !g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( )->IsGun( ) )
		return;
	
	if ( g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( )->get_definition_index( ) == WEAPON_REVOLVER )
	{
		if ( !g_globals->GetLocalPlayer( )->CanFire( NULL, true ) )
		{
			g_FuckThisSDK->m_pCmd->buttons |= IN_ATTACK;
			return;
		}
	}

	this->ScanPlayers( );
	if ( !g_FuckThisSDK->m_RageData.m_CurrentTarget.m_Player )
		return;

	int32_t nHitChance = m_RageSettings.m_iHitChance;
	if ( g_FuckThisSDK->m_AccuracyData.m_bDoingSecondShot )
		nHitChance = m_RageSettings.m_iDoubleTapHitChance;
	g_FuckThisSDK->m_AccuracyData.m_bDoingSecondShot = false;

	float_t flCalculatedHitchance = this->GetHitChance( g_FuckThisSDK->m_RageData.m_CurrentTarget );
	if ( flCalculatedHitchance < nHitChance )
	{
		if ( g_globals->GetLocalPlayer( )->get_weapon_handle( )->IsSniper( ) )
		{
			if ( !g_globals->GetLocalPlayer( )->is_scoped( ) )
			{
				this->AutoScope( );
				return;
			}
			else 
				return;
		}
		else
			return;
	}

	float_t flCalculatedAccuracy = this->GetAccuracyBoost( g_FuckThisSDK->m_RageData.m_CurrentTarget );
	if ( flCalculatedAccuracy < m_RageSettings.m_iAccuracyBoost )
	{
		if ( g_globals->GetLocalPlayer( )->get_weapon_handle( )->IsSniper( ) )
		{
			if ( !g_globals->GetLocalPlayer( )->is_scoped( ) )
			{
				this->AutoScope( );
				return;
			}
			else
				return;
		}
		else
			return;
	}
	
	QAngle angCalculatedAngle = math::CalcAngle( g_FuckThisSDK->m_LocalData.m_vecShootPosition, g_FuckThisSDK->m_RageData.m_CurrentTarget.m_Hitbox.m_vecPoint );
	if ( !g_FuckThisSDK->m_bFakeDuck )
		*g_FuckThisSDK->m_pbSendPacket = true;

	g_FuckThisSDK->m_pCmd->tick_count = TIME_TO_TICKS( g_FuckThisSDK->m_RageData.m_CurrentTarget.m_LagRecord.m_SimulationTime + g_LagCompensation->GetLerpTime( ) );
	g_FuckThisSDK->m_pCmd->viewangles = angCalculatedAngle;
	g_FuckThisSDK->m_pCmd->buttons |= IN_ATTACK;

	static auto m_WeaponRecoilScale = g_cvar->FindVar( "weapon_recoil_scale" );
	if ( m_WeaponRecoilScale->GetFloat( ) > 0.0f )
		g_FuckThisSDK->m_pCmd->viewangles -= m_WeaponRecoilScale->GetFloat( ) * g_globals->GetLocalPlayer( )->get_punch( );

	C_ShotData ShotData;

	ShotData.m_Target = g_FuckThisSDK->m_RageData.m_CurrentTarget;
	ShotData.m_vecStartPosition = g_FuckThisSDK->m_LocalData.m_vecShootPosition;
	ShotData.m_vecEndPosition = g_FuckThisSDK->m_RageData.m_CurrentTarget.m_Hitbox.m_vecPoint;
	ShotData.m_iShotTick = g_Networking->GetServerTick( );
	ShotData.m_bHasMaximumAccuracy = this->HasMaximumAccuracy( );

	g_FuckThisSDK->m_ShotData.emplace_back( ShotData );
}

void C_RageBot::UpdatePeekState( )
{
	g_FuckThisSDK->m_bIsPeeking = false;
	if ( g_globals->GetLocalPlayer( )->get_velocity( ).Length2D( ) < 5.40f )
		return;

	int nScannedTargets = 0;
	if ( m_nLastPeekID >= g_globals->max_clients )
		m_nLastPeekID = 1;

	for ( ; m_nLastPeekID <= g_globals->max_clients; m_nLastPeekID++ )
	{
		C_BasePlayer* Player = static_cast < C_BasePlayer* > ( g_ent_list->GetClientEntity( m_nLastPeekID ) );
		if ( !Player || !Player->is_player( ) || !Player->is_alive( ) || Player->IsDormant( ) || Player->get_team( ) == g_globals->GetLocalPlayer( )->get_team( ) )
			continue;

		if ( Player->get_gun_game_immunity( ) || ( Player->get_flags( ) & entity_flags::fl_frozen ) )
			continue;

		const auto m_LagRecords = g_FuckThisSDK->m_CachedPlayerRecords[ m_nLastPeekID ];
		if ( m_LagRecords.empty( ) )
			continue;

		if ( nScannedTargets > 2 )
			break;

		// force player
		this->AdjustPlayerRecord( Player, m_LagRecords.back( ) );
	
		// next shoot pos
		Vector vecNextShootPosition = g_FuckThisSDK->m_LocalData.m_vecShootPosition + ( g_globals->GetLocalPlayer( )->get_velocity( ) * g_globals->interval_per_tick ) * 4.0f;

		// scan record from next position
		C_HitboxData HitboxData = this->ScanPlayerRecord( Player, m_LagRecords.back( ), vecNextShootPosition );
		if ( !HitboxData.m_flDamage )
		{
			// increase
			nScannedTargets++;

			// we didn't find the player that we peek
			continue;
		}

		// we are actually peeking somebody
		g_FuckThisSDK->m_Player = Player;
		g_FuckThisSDK->m_bIsPeeking = true;

		break;
	}
}

void C_RageBot::ScanPlayers( )
{
	int nScannedTargets = 0;
	if ( m_nLastRageID >= g_globals->max_clients )
		m_nLastRageID = 1;

	bool bSkipLC = false;
	static IConVar* pSkipLC = g_cvar->FindVar( "cl_lagcompensation" );
	if ( pSkipLC->GetBool( ) != true )
		bSkipLC = true;

	for ( ; m_nLastRageID <= g_globals->max_clients; m_nLastRageID++ )
	{
		C_BasePlayer* pPlayer = static_cast < C_BasePlayer* > ( g_ent_list->GetClientEntity( m_nLastRageID ) );
		if ( !pPlayer || !pPlayer->is_player( ) || !pPlayer->is_alive( ) || pPlayer->get_team( ) == g_globals->GetLocalPlayer( )->get_team( ) )
			continue;
		
		Vector vecShootPosition = g_FuckThisSDK->m_LocalData.m_vecShootPosition;
		if ( pPlayer->get_gun_game_immunity( ) || ( pPlayer->get_flags( ) & entity_flags::fl_frozen ) )
			continue;

		const auto m_LagRecords = g_FuckThisSDK->m_CachedPlayerRecords[ m_nLastRageID ];
		if ( m_LagRecords.empty( ) )
			continue;

		if ( nScannedTargets > 2 )
			break;

		C_LagRecord LagRecord = this->GetFirstAvailableRecord( pPlayer );
		if ( !g_LagCompensation->IsValidTime( LagRecord.m_SimulationTime ) )
		{
			if ( m_RageSettings.m_AutoStopOptions[ AUTOSTOP_EARLY ] && m_RageSettings.m_bAutoStop )
			{
				this->AdjustPlayerRecord( pPlayer, m_LagRecords.back( ) );
				if ( g_FuckThisSDK->m_MovementData.m_TicksToStop > 0 )
				{
					C_HitboxData HitboxData = this->ScanPlayerRecord( pPlayer, m_LagRecords.back( ), vecShootPosition + ( g_globals->GetLocalPlayer( )->get_velocity( ) * g_globals->interval_per_tick ) * g_FuckThisSDK->m_MovementData.m_TicksToStop );
					if ( HitboxData.m_flDamage >= this->GetMinDamage( pPlayer ) )
					{
						float_t flDistance = g_globals->GetLocalPlayer( )->get_origin( ).DistTo( pPlayer->get_origin( ) );
						if ( flDistance )
						{
							int nIndex = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( )->get_definition_index( );
							if ( ( nIndex != WEAPON_SCAR20 && nIndex != WEAPON_G3SG1 ) || flDistance > 430.0f )
								this->AutoScope( );
						}

						if ( this->CanAutoStop( ) )
							g_FuckThisSDK->m_AccuracyData.m_bRestoreAutoStop = false;
					}
				}
			}

			continue;
		}

		this->AdjustPlayerRecord( pPlayer, LagRecord );
		if ( m_RageSettings.m_AutoStopOptions[ AUTOSTOP_EARLY ] && m_RageSettings.m_bAutoStop )
		{
			if ( g_FuckThisSDK->m_MovementData.m_TicksToStop > 0 )
			{
				C_HitboxData HitboxData = this->ScanPlayerRecord( pPlayer, LagRecord, vecShootPosition + ( g_globals->GetLocalPlayer( )->get_velocity( ) * g_globals->interval_per_tick ) * g_FuckThisSDK->m_MovementData.m_TicksToStop );
				if ( HitboxData.m_flDamage >= this->GetMinDamage( pPlayer ) )
				{
					float_t flDistance = g_globals->GetLocalPlayer( )->get_origin( ).DistTo( pPlayer->get_origin( ) );
					if ( flDistance )
					{
						int nIndex = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( )->get_definition_index( );
						if ( ( nIndex != WEAPON_SCAR20 && nIndex != WEAPON_G3SG1 ) || flDistance > 430.0f )
							this->AutoScope( );
					}

					if ( this->CanAutoStop( ) )
						g_FuckThisSDK->m_AccuracyData.m_bRestoreAutoStop = false;
				}
			}
		}

		C_HitboxData HitboxData = this->ScanPlayerRecord( pPlayer, LagRecord, vecShootPosition );
		if ( HitboxData.m_flDamage >= this->GetMinDamage( pPlayer ) )
		{
			g_FuckThisSDK->m_RageData.m_CurrentTarget.m_Hitbox = HitboxData;
			g_FuckThisSDK->m_RageData.m_CurrentTarget.m_LagRecord = LagRecord;
			g_FuckThisSDK->m_RageData.m_CurrentTarget.m_Player = pPlayer;

			if ( this->CanAutoStop( ) )
				g_FuckThisSDK->m_AccuracyData.m_bRestoreAutoStop = false;

			float_t flDistance = g_globals->GetLocalPlayer( )->get_origin( ).DistTo( pPlayer->get_origin( ) );
			if ( flDistance )
			{
				int nIndex = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( )->get_definition_index( );
				if ( ( nIndex != WEAPON_SCAR20 && nIndex != WEAPON_G3SG1 ) || flDistance > 430.0f )
					this->AutoScope( );
			}

			this->AdjustPlayerRecord( g_FuckThisSDK->m_RageData.m_CurrentTarget.m_Player, g_FuckThisSDK->m_RageData.m_CurrentTarget.m_LagRecord );
			break;
		}

		if ( bSkipLC )
			continue;
		
		C_LagRecord BTRecord = C_LagRecord( );
		C_HitboxData BTHitscan = C_HitboxData( );
		
		if ( !this->FindPlayerRecord( pPlayer, &BTRecord, &BTHitscan ) )
		{
			nScannedTargets++;

			this->AdjustPlayerRecord( pPlayer, m_LagRecords.back( ) );
			continue;
		}

		g_FuckThisSDK->m_RageData.m_CurrentTarget.m_Player = pPlayer;
		g_FuckThisSDK->m_RageData.m_CurrentTarget.m_LagRecord = BTRecord;
		g_FuckThisSDK->m_RageData.m_CurrentTarget.m_Hitbox = BTHitscan;
		
		if ( this->CanAutoStop( ) )
			g_FuckThisSDK->m_AccuracyData.m_bRestoreAutoStop = false;
		
		float_t flDistance = g_globals->GetLocalPlayer( )->get_abs_origin( ).DistTo( pPlayer->get_abs_origin( ) );
		if ( flDistance )
		{
			int nIndex = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( )->get_definition_index( );
			if ( ( nIndex != WEAPON_SCAR20 && nIndex != WEAPON_G3SG1 ) || flDistance > 450.0f )
				this->AutoScope( );
		}

		this->AdjustPlayerRecord( g_FuckThisSDK->m_RageData.m_CurrentTarget.m_Player, g_FuckThisSDK->m_RageData.m_CurrentTarget.m_LagRecord );
		break;
	}
}

bool C_RageBot::FindPlayerRecord( C_BasePlayer* pPlayer, C_LagRecord* OutRecord, C_HitboxData* OutHitbox )
{
	bool bDidScanLastRecord = false;
	for ( auto LagRecord : g_FuckThisSDK->m_CachedPlayerRecords[ pPlayer->EntIndex( ) ] )
	{
		if ( !g_LagCompensation->IsValidTime( LagRecord.m_SimulationTime ) )
			continue;

		Vector vecShootPosition = g_FuckThisSDK->m_LocalData.m_vecShootPosition;
		if ( !LagRecord.m_bIsShooting && bDidScanLastRecord )
			continue;
		
		bDidScanLastRecord = true;
		this->AdjustPlayerRecord( pPlayer, LagRecord );

		C_HitboxData HitboxData = this->ScanPlayerRecord( pPlayer, LagRecord, vecShootPosition );
		if ( HitboxData.m_flDamage < this->GetMinDamage( pPlayer ) )
			continue;

		*OutRecord = LagRecord;
		*OutHitbox = HitboxData;

		return true;
	}

	return false;
}

int32_t C_RageBot::GetHitgroupFromHitbox( int32_t iHitbox )
{
	int32_t iHitgroup = 0;
	switch ( iHitbox )
	{
		case HITBOX_HEAD:
			iHitgroup = HITGROUP_HEAD;
		break;

		case HITBOX_CHEST:
			iHitgroup = HITGROUP_CHEST;
		break;

		case HITBOX_PELVIS:
			iHitgroup = HITGROUP_GENERIC;
		break;

		case HITBOX_LEFT_CALF:
		case HITBOX_LEFT_FOOT:
		case HITBOX_LEFT_THIGH:
			iHitgroup = HITGROUP_LEFTLEG;
		break;

		case HITBOX_RIGHT_CALF:
		case HITBOX_RIGHT_FOOT:
		case HITBOX_RIGHT_THIGH:
			iHitgroup = HITGROUP_RIGHTLEG;
		break;

		case HITBOX_LEFT_UPPER_ARM:
		case HITBOX_LEFT_HAND:
		case HITBOX_LEFT_FOREARM:
			iHitgroup = HITGROUP_LEFTARM;
		break;

		case HITBOX_RIGHT_UPPER_ARM:
		case HITBOX_RIGHT_HAND:
		case HITBOX_RIGHT_FOREARM:
			iHitgroup = HITGROUP_RIGHTARM;
		break;

		case HITBOX_STOMACH:
			iHitgroup = HITGROUP_STOMACH;
		break;

		default: break;
	}

	return iHitgroup;
}

bool SortHitboxes( C_HitboxHitscanData First, C_HitboxHitscanData Second )
{
	if ( First.m_iHitbox == HITBOX_HEAD )
		return false;
	else if ( Second.m_iHitbox == HITBOX_HEAD )
		return true;

	return First.m_flWeaponDamage > Second.m_flWeaponDamage;
}

C_HitboxData C_RageBot::ScanPlayerRecord( C_BasePlayer* pPlayer, C_LagRecord LagRecord, Vector vecStartPosition )
{
	C_BaseCombatWeapon* pCombatWeapon = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( );
	if ( !pCombatWeapon )
		return C_HitboxData( );

	CCSWeaponInfo* pWeaponData = pCombatWeapon->get_weapon_data( );
	if ( !pWeaponData )
		return C_HitboxData( );

	std::vector < C_HitboxHitscanData > aHitboxesData;
	
	bool bForcedSafety =utils::key_bind(config.safe_point_key, config.safe_point_key_mode);
	if ( m_RageSettings.m_Hitboxes[ 0 ] )
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_HEAD, m_RageSettings.m_SafeHitboxes[ HITBOX_HEAD ] || bForcedSafety ) );
	
	if ( m_RageSettings.m_Hitboxes[ 1 ] )
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_CHEST, m_RageSettings.m_SafeHitboxes[ 1 ] || bForcedSafety ) );
	
	if ( m_RageSettings.m_Hitboxes[ 2 ] )
	{
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_LEFT_FOREARM, m_RageSettings.m_SafeHitboxes[ 2 ] || bForcedSafety ) );
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_RIGHT_FOREARM, m_RageSettings.m_SafeHitboxes[ 2 ] || bForcedSafety ) );
	}

	if ( m_RageSettings.m_Hitboxes[ 3 ] )
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_PELVIS, m_RageSettings.m_SafeHitboxes[ 3 ] || bForcedSafety ) );

	if ( m_RageSettings.m_Hitboxes[ 4 ] )
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_STOMACH, m_RageSettings.m_SafeHitboxes[ 4 ] || bForcedSafety ) );
		
	if ( m_RageSettings.m_Hitboxes[ 5 ] )
	{
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_LEFT_THIGH, m_RageSettings.m_SafeHitboxes[ 5 ] || bForcedSafety ) );
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_RIGHT_THIGH, m_RageSettings.m_SafeHitboxes[ 5 ] || bForcedSafety ) );
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_LEFT_CALF, m_RageSettings.m_SafeHitboxes[ 5 ] || bForcedSafety ) );
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_RIGHT_CALF, m_RageSettings.m_SafeHitboxes[ 5 ] || bForcedSafety ) );
	}
	
	std::vector < C_HitboxData > m_ScanData;
	for ( auto Hitbox : aHitboxesData )
	{
		std::vector < Vector > m_Hitboxes = this->GetHitboxPoints( pPlayer, LagRecord, vecStartPosition, Hitbox.m_iHitbox );
		if ( m_Hitboxes.empty( ) )
			continue;

		for ( auto Point : m_Hitboxes )
		{
			float_t flDamage = g_AutoWall->GetPointDamage( vecStartPosition, Point );
			if ( flDamage < this->GetMinDamage( pPlayer ) )
				continue;

			C_HitboxData ScanData;
			ScanData.pPlayer = pPlayer;
			ScanData.LagRecord = LagRecord;
			ScanData.m_bForcedToSafeHitbox = Hitbox.m_bForceSafe;
			ScanData.m_flDamage = flDamage;
			ScanData.m_iHitbox = Hitbox.m_iHitbox;
			ScanData.m_vecPoint = Point;
			ScanData.m_bIsSafeHitbox = this->IsSafePoint( pPlayer, LagRecord, vecStartPosition, Point, Hitbox.m_iHitbox );

			if ( flDamage >= pPlayer->get_health( ) || pWeaponData->m_iDamage >= pPlayer->get_health( ) )
				if ( m_RageSettings.m_bLethalSafety && !ScanData.m_bIsSafeHitbox )
					continue;

			if ( Hitbox.m_bForceSafe && !ScanData.m_bIsSafeHitbox )
				continue;

			m_ScanData.emplace_back( ScanData );
		}
	}

	if ( m_ScanData.empty( ) )
		return C_HitboxData( );

	bool bHasLethalDamage = false;
	int nLethalPosition = 0;

	for ( int i = 0; i < m_ScanData.size( ); i++ )
	{
		if ( m_ScanData[ i ].m_flDamage < pPlayer->get_health( ) || m_ScanData[ i ].m_iHitbox == HITBOX_HEAD )
			continue;

		if ( !m_ScanData[ i ].m_bIsSafeHitbox )
			if ( m_ScanData[ i ].m_bForcedToSafeHitbox || m_RageSettings.m_bLethalSafety )
				continue;

		nLethalPosition = i;
		bHasLethalDamage = true;

		break;
	}

	if ( bHasLethalDamage )
		return m_ScanData[ nLethalPosition ];

	bool bHasHeadSafety = false;
	for ( int i = 0; i < m_ScanData.size( ); i++ )
	{
		if ( m_ScanData[ i ].m_iHitbox != HITBOX_HEAD || m_ScanData[ i ].m_flDamage < pPlayer->get_health( ) )
			continue;

		if ( !m_ScanData[ i ].m_bIsSafeHitbox && !LagRecord.m_bAnimResolved || !( pPlayer->get_flags( ) & entity_flags::fl_on_ground ) )
			continue;

		return m_ScanData[ i ];
	}

	auto SortSafety = [ ]( C_HitboxData First, C_HitboxData Second ) -> bool
	{
		if ( First.m_bIsSafeHitbox )
			return false;

		return true;
	};

	auto SortBody = [ ]( C_HitboxData First, C_HitboxData Second ) -> bool
	{
		if ( First.m_iHitbox == HITBOX_HEAD )
			return false;

		return true;
	};

	auto SortDamage = [ ]( C_HitboxData First, C_HitboxData Second ) -> bool
	{
		return First.m_flDamage > Second.m_flDamage;
	};
	
	std::sort( m_ScanData.begin( ), m_ScanData.end( ), SortDamage );
	if ( m_RageSettings.m_bPreferBody )
		std::sort( m_ScanData.begin( ), m_ScanData.end( ), SortBody );

	if ( m_RageSettings.m_bPreferSafe )
		std::sort( m_ScanData.begin( ), m_ScanData.end( ), SortSafety );

	return m_ScanData.front( );
}

C_LagRecord C_RageBot::GetFirstAvailableRecord( C_BasePlayer* pPlayer )
{
	const auto m_LagRecords = g_FuckThisSDK->m_CachedPlayerRecords[ pPlayer->EntIndex( ) ];
	if ( m_LagRecords.empty( ) )
		return C_LagRecord( );
	
	C_LagRecord LagRecord = C_LagRecord( );
	for ( int32_t i = 0; i < m_LagRecords.size( ); i++ )
	{
		auto m_Record = m_LagRecords[ i ];
		if ( !g_LagCompensation->IsValidTime( m_Record.m_SimulationTime ) )
			continue;

		LagRecord = m_LagRecords[ i ];
	}

	return LagRecord;
}

void C_RageBot::SaveMovementData( )
{
	m_flForwardMove = g_FuckThisSDK->m_pCmd->forwardmove;
	m_flSideMove = g_FuckThisSDK->m_pCmd->sidemove;
}

void C_RageBot::ForceMovementData( )
{
	if ( !g_FuckThisSDK->m_AccuracyData.m_bRestoreAutoStop )
		return;

	g_FuckThisSDK->m_pCmd->forwardmove = m_flForwardMove;
	g_FuckThisSDK->m_pCmd->sidemove = m_flSideMove;
	
	g_PredictionSystem->RestoreNetvars( g_FuckThisSDK->m_pCmd->command_number );
	return g_PredictionSystem->Repredict( );
}

void C_RageBot::FakeDuck( )
{
	if ( ( g_globals->GetLocalPlayer( )->get_flags( ) & entity_flags::fl_frozen ) )
		return;

	bool bShouldFakeDuck = config.m_bFakeDuck && ( g_globals->GetLocalPlayer( )->get_flags( ) & entity_flags::fl_on_ground );
	bool bFakeDuckBackup = g_FuckThisSDK->m_bFakeDuck;

	if ( bShouldFakeDuck )
		g_FuckThisSDK->m_bVisualFakeDuck = true;
	else if ( g_globals->GetLocalPlayer( )->m_flDuckAmount( ) == 0.0f || g_globals->GetLocalPlayer( )->m_flDuckAmount( ) >= 1.0f )
		g_FuckThisSDK->m_bVisualFakeDuck = false;

	g_FuckThisSDK->m_bFakeDuck = bShouldFakeDuck;
	if ( !bFakeDuckBackup && bShouldFakeDuck )
	{
		g_FuckThisSDK->m_bFakeDuck = true;
		if ( g_client_state->chokedcommands )
			g_FuckThisSDK->m_bFakeDuck = false;
		else
			g_FuckThisSDK->m_pCmd->buttons &= ~IN_DUCK;

		return;
	}
	else if ( !bShouldFakeDuck )
	{
		float m_flAwaitedDuck = ( g_FuckThisSDK->m_pCmd->buttons & IN_DUCK ) ? 1.0f : 0.0f;
		if ( m_flAwaitedDuck != g_globals->GetLocalPlayer( )->m_flDuckAmount( ) )
		{
			g_FuckThisSDK->m_pCmd->buttons |= IN_BULLRUSH;

			if ( m_flAwaitedDuck < g_globals->GetLocalPlayer( )->m_flDuckAmount( ) )
				g_FuckThisSDK->m_pCmd->buttons &= ~IN_DUCK;
			else
				g_FuckThisSDK->m_pCmd->buttons |= IN_DUCK;

			g_FuckThisSDK->m_bFakeDuck = true;
		}
		
		if ( !g_FuckThisSDK->m_bFakeDuck )
			return;
	}
	else
	{
		g_FuckThisSDK->m_pCmd->buttons |= IN_BULLRUSH;

		if ( g_client_state->chokedcommands < 7 )
			g_FuckThisSDK->m_pCmd->buttons &= ~IN_DUCK;
		else
			g_FuckThisSDK->m_pCmd->buttons |= IN_DUCK;
	}

	if ( !bShouldFakeDuck )
	{
		g_FuckThisSDK->m_bFakeDuck = false;
		return;
	}

	if ( g_client_state->chokedcommands < 14 )
	{
		*g_FuckThisSDK->m_pbSendPacket = false;
		return;
	}

	*g_FuckThisSDK->m_pbSendPacket = true;
}

void C_RageBot::SetupPacket( )
{
	C_BaseCombatWeapon* pCombatWeapon = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( );
	if ( !pCombatWeapon )
		return;

	CCSWeaponInfo* pWeaponData = pCombatWeapon->get_weapon_data( );
	if ( !pWeaponData )
		return;

	if ( !g_FuckThisSDK->m_AccuracyData.m_bHasValidAccuracyData )
	{
		for ( int32_t iSeed = 0; iSeed < 256; iSeed++ )
		{
			utils::random_seed( iSeed + 1 );

			g_FuckThisSDK->m_AccuracyData.m_FirstRandom[ iSeed ] = utils::random_float( 0.f, 2.f * ( 22.0f / 7.0f ) );
			g_FuckThisSDK->m_AccuracyData.m_SecondRandom[ iSeed ] = utils::random_float( 0.f, 2.f * ( 22.0f / 7.0f ) );
			g_FuckThisSDK->m_AccuracyData.m_aInaccuracy[ iSeed ] = utils::random_float( 0.f, 1.0f );
			g_FuckThisSDK->m_AccuracyData.m_aSpread[ iSeed ] = utils::random_float( 0.f, 1.0f );
		}

		g_FuckThisSDK->m_AccuracyData.m_bHasValidAccuracyData = true;
	}

	float_t flDefaultInaccuracy = 0.0f;
	if ( g_globals->GetLocalPlayer( )->m_flDuckAmount( ) )
	{
		if ( g_globals->GetLocalPlayer( )->is_scoped( ) )
			flDefaultInaccuracy = pWeaponData->m_flInaccuracyCrouchAlt;
		else
			flDefaultInaccuracy = pWeaponData->m_flInaccuracyCrouch;
	}
	else
	{
		if ( g_globals->GetLocalPlayer( )->is_scoped( ) )
			flDefaultInaccuracy = pWeaponData->m_flInaccuracyStandAlt;
		else
			flDefaultInaccuracy = pWeaponData->m_flInaccuracyStand;
	}
	g_FuckThisSDK->m_AccuracyData.m_flMinInaccuracy = flDefaultInaccuracy;

	int32_t iCurrentWeapon = -1;
	switch ( pCombatWeapon->get_definition_index( ) )
	{
		case WEAPON_AK47:
		case WEAPON_M4A1:
		case WEAPON_M4A1_SILENCER:
		case WEAPON_FAMAS:
		case WEAPON_SG556:
		case WEAPON_GALILAR:
			iCurrentWeapon = RAGE_WEAPON::RIFLE; break;
		case WEAPON_MAG7:
		case WEAPON_NOVA:
		case WEAPON_XM1014:
		case WEAPON_SAWEDOFF:
			iCurrentWeapon = RAGE_WEAPON::SHOTGUN; break;
		case WEAPON_MP7:
		case WEAPON_MP9:
		case WEAPON_P90:
		case WEAPON_M249:
		case WEAPON_NEGEV:
		case WEAPON_UMP45:
			iCurrentWeapon = RAGE_WEAPON::SMG; break;
		case WEAPON_SCAR20:
		case WEAPON_G3SG1:
			iCurrentWeapon = RAGE_WEAPON::AUTO; break;
		case WEAPON_GLOCK:
		case WEAPON_HKP2000: 
		case WEAPON_USP_SILENCER:
		case WEAPON_CZ75A:
		case WEAPON_TEC9:
		case WEAPON_ELITE:
		case WEAPON_FIVESEVEN:
		case WEAPON_P250:
			iCurrentWeapon = RAGE_WEAPON::PISTOL; break;
		case WEAPON_SSG08:
			iCurrentWeapon = RAGE_WEAPON::SCOUT; break;
		case WEAPON_AWP:
			iCurrentWeapon = RAGE_WEAPON::AWP; break;
		case WEAPON_DEAGLE:
			iCurrentWeapon = RAGE_WEAPON::DEAGLE; break;
		case WEAPON_REVOLVER:
			iCurrentWeapon = RAGE_WEAPON::REVOLVER; break;
		default: iCurrentWeapon = -1;
	}

	g_FuckThisSDK->m_AccuracyData.m_bHasMaximumAccuracy = this->HasMaximumAccuracy( );
	if ( pCombatWeapon->get_definition_index( ) == WEAPON_TASER )
	{
		m_RageSettings = C_RageSettings( );
		m_RageSettings.m_bEnabled = true;
		m_RageSettings.m_bAutoStop = true;

		m_RageSettings.m_bPreferSafe = true;
		m_RageSettings.m_bPreferBody = true;
		m_RageSettings.m_iBodyScale = 80;
		m_RageSettings.m_bLethalSafety = true;
		m_RageSettings.m_iHitChance = 85;

		m_RageSettings.m_Hitboxes[ 0 ] = false;
		m_RageSettings.m_Hitboxes[ 1 ] = false;
		m_RageSettings.m_Hitboxes[ 2 ] = false;
		m_RageSettings.m_Hitboxes[ 3 ] = true;
		m_RageSettings.m_Hitboxes[ 4 ] = true;
		m_RageSettings.m_Hitboxes[ 5 ] = false;
		m_RageSettings.m_Hitboxes[ 6 ] = false;

		m_RageSettings.m_SafeHitboxes[ 0 ] = false;
		m_RageSettings.m_SafeHitboxes[ 1 ] = false;
		m_RageSettings.m_SafeHitboxes[ 2 ] = false;
		m_RageSettings.m_SafeHitboxes[ 3 ] = true;
		m_RageSettings.m_SafeHitboxes[ 4 ] = true;
		m_RageSettings.m_SafeHitboxes[ 5 ] = false;
		m_RageSettings.m_SafeHitboxes[ 6 ] = false;
		
		m_RageSettings.m_Multipoints[ 0 ] = false;
		m_RageSettings.m_Multipoints[ 1 ] = false;
		m_RageSettings.m_Multipoints[ 2 ] = false;
		m_RageSettings.m_Multipoints[ 3 ] = true;
		m_RageSettings.m_Multipoints[ 4 ] = true;
		m_RageSettings.m_Multipoints[ 5 ] = false;
		m_RageSettings.m_Multipoints[ 6 ] = false;

		return; 
	}

	if ( iCurrentWeapon <= -1 )
	{
		m_RageSettings = C_RageSettings( );
		return;
	}

	m_RageSettings = config.m_aRageSettings[ iCurrentWeapon ];
}

int32_t C_RageBot::GetMinDamage( C_BasePlayer* pPlayer )
{
	int32_t iMinDamage = m_RageSettings.m_iMinDamage;
	if (utils::key_bind(config.m_bOverrideDamage_key, config.m_bOverrideDamage_key_mode) )
		iMinDamage = m_RageSettings.m_iMinDamageOverride;

	iMinDamage = min( iMinDamage, pPlayer->get_health( ) );
	if ( iMinDamage <= 0 )
		iMinDamage = this->GetAutoDamage( pPlayer->get_armor( ) != 0 );
	
	if ( iMinDamage > 99 )
		iMinDamage = pPlayer->get_health( ) + ( iMinDamage - 100 );

	return iMinDamage;
}

int32_t C_RageBot::GetAutoDamage( bool bIsPlayerArmoured )
{
	int32_t nItemID = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( )->get_definition_index( );
	switch ( nItemID )
	{
	case WEAPON_SCAR20:
	case WEAPON_G3SG1:
		return bIsPlayerArmoured ? 42 : 33;
	case WEAPON_SSG08:
		return 84;
	case WEAPON_AWP:
		return 100;
	case WEAPON_REVOLVER:
	case WEAPON_DEAGLE:
		return 44;
	case WEAPON_TEC9:
	case WEAPON_HKP2000:
	case WEAPON_USP_SILENCER:
	case WEAPON_P250:
	case WEAPON_FLASHBANG:
	case WEAPON_CZ75A:
	case WEAPON_GLOCK:
	case WEAPON_ELITE:
		return bIsPlayerArmoured ? 20 : 34; 
	}

	return 0;
}

void C_RageBot::PredictAutoStop( )
{
	g_FuckThisSDK->m_MovementData.m_TicksToStop = 0;
	if ( !this->CanAutoStop( ) || !m_RageSettings.m_bAutoStop )
		return;

	float_t flMaxSpeed = g_globals->GetLocalPlayer( )->GetMaxPlayerSpeed( );
	for ( ; g_FuckThisSDK->m_MovementData.m_TicksToStop < 6; g_FuckThisSDK->m_MovementData.m_TicksToStop++ )
	{
		this->AutoStop( );

		g_prediction->SetupMove( g_globals->GetLocalPlayer( ), g_FuckThisSDK->m_pCmd, g_move_helper, &m_MoveData );
		g_game_movement->ProcessMovement( g_globals->GetLocalPlayer( ), &m_MoveData );
		g_prediction->FinishMove( g_globals->GetLocalPlayer( ), g_FuckThisSDK->m_pCmd, &m_MoveData );

		if ( !m_RageSettings.m_AutoStopOptions[ AUTOSTOP_ACCURACY ] )
		{
			if ( g_globals->GetLocalPlayer( )->get_velocity( ).Length2D( ) < flMaxSpeed * 0.34f )
				break;
		}
		else if ( g_globals->GetLocalPlayer( )->get_velocity( ).Length2D( ) < 5.0f )
			break;
	}

	g_PredictionSystem->RestoreNetvars( g_FuckThisSDK->m_pCmd->command_number );
}

void C_RageBot::ResetData( )
{
	g_FuckThisSDK->m_AccuracyData.m_aInaccuracy = { };
	g_FuckThisSDK->m_AccuracyData.m_aSpread = { };
	
	g_FuckThisSDK->m_AccuracyData.m_flSpread = 0.0f;
	g_FuckThisSDK->m_AccuracyData.m_flInaccuracy = 0.0f;
	g_FuckThisSDK->m_AccuracyData.m_flMinInaccuracy = 0.0f;
	g_FuckThisSDK->m_AccuracyData.m_bHasValidAccuracyData = false;

	g_FuckThisSDK->m_RageData.m_CurrentTarget = C_TargetData( );

	g_FuckThisSDK->m_MovementData.m_TicksToStop = 0;

	if ( !g_FuckThisSDK->m_ShotData.empty( ) )
		return g_FuckThisSDK->m_ShotData.clear( );
}

void C_RageBot::OnWeaponFire( IGameEvent* pEvent )
{
	if ( !g_globals->GetLocalPlayer( ) || !g_globals->GetLocalPlayer( )->is_alive( ) )
		return;

	if ( g_engine->GetPlayerForUserID( pEvent->GetInt( ( "userid" ) ) ) != g_engine->GetLocalPlayer( ) )
		return;

	C_ShotData* ShotData = nullptr;
	for ( int i = 0; i < g_FuckThisSDK->m_ShotData.size( ); i++ )
	{
		if ( g_Networking->GetServerTick( ) - g_FuckThisSDK->m_ShotData[ i ].m_iShotTick > g_Networking->GetTickRate( ) )
		{
			g_FuckThisSDK->m_ShotData.erase( g_FuckThisSDK->m_ShotData.begin( ) + i );
			continue;
		}

		ShotData = &g_FuckThisSDK->m_ShotData[ i ];
	}

	if ( ShotData )
		ShotData->m_bHasBeenFired = true;
}

void C_RageBot::OnPlayerHurt( IGameEvent* pEvent )
{
	if ( !g_globals->GetLocalPlayer( ) || !g_globals->GetLocalPlayer( )->is_alive( ) )
		return;

	if ( g_engine->GetPlayerForUserID( pEvent->GetInt( ( "attacker" ) ) ) != g_engine->GetLocalPlayer( ) )
		return;

	C_ShotData* ShotData = nullptr;
	for ( int i = 0; i < g_FuckThisSDK->m_ShotData.size( ); i++ )
	{
		if ( g_Networking->GetServerTick( ) - g_FuckThisSDK->m_ShotData[ i ].m_iShotTick > g_Networking->GetTickRate( ) )
		{
			g_FuckThisSDK->m_ShotData.erase( g_FuckThisSDK->m_ShotData.begin( ) + i );
			continue;
		}

		ShotData = &g_FuckThisSDK->m_ShotData[ i ];
	}

	if ( !ShotData )
		return;

	ShotData->m_bHasBeenHurted = true;
	ShotData->m_Damage = pEvent->GetInt( ( "dmg_health" ) );
}

void C_RageBot::OnBulletImpact( IGameEvent* pEvent )
{
	if ( !g_globals->GetLocalPlayer( ) || !g_globals->GetLocalPlayer( )->is_alive( ) )
		return;
	
	Vector vecBulletImpact = Vector( pEvent->GetInt( ( "x" ) ), pEvent->GetInt( ( "y" ) ), pEvent->GetInt( ( "z" ) ) );
	if ( g_engine->GetPlayerForUserID( pEvent->GetInt( ( "userid" ) ) ) != g_engine->GetLocalPlayer( ) )
		return;

	C_ShotData* ShotData = nullptr;
	for ( int i = 0; i < g_FuckThisSDK->m_ShotData.size( ); i++ )
	{
		if ( g_Networking->GetServerTick( ) - g_FuckThisSDK->m_ShotData[ i ].m_iShotTick > g_Networking->GetTickRate( ) )
		{
			g_FuckThisSDK->m_ShotData.erase( g_FuckThisSDK->m_ShotData.begin( ) + i );
			continue;
		}

		ShotData = &g_FuckThisSDK->m_ShotData[ i ];
	}
	
	if ( !ShotData )
		return;

	ShotData->m_vecImpacts.emplace_back( vecBulletImpact );
	ShotData->m_bHasBeenRegistered = true;
}

void C_RageBot::OnNetworkUpdate( )
{
	if ( !g_globals->GetLocalPlayer( ) || !g_globals->GetLocalPlayer( )->is_alive( ) )
	{
		if ( !g_FuckThisSDK->m_ShotData.empty( ) )
			g_FuckThisSDK->m_ShotData.clear( );

		return;
	}

	for ( int i = 0; i < g_FuckThisSDK->m_ShotData.size( ); i++ )
	{
		auto Shot = &g_FuckThisSDK->m_ShotData[ i ];
		if ( !Shot->m_bHasBeenFired || !Shot->m_bHasBeenRegistered )
			continue;
	
		if ( !Shot->m_Target.m_Player )
		{
			g_FuckThisSDK->m_ShotData.erase( g_FuckThisSDK->m_ShotData.begin( ) + i );
			continue;
		}

		if ( Shot->m_Target.m_Player->is_alive( ) )
		{
			const auto LagRecords = g_FuckThisSDK->m_CachedPlayerRecords[ Shot->m_Target.m_Player->EntIndex( ) ];
			if ( LagRecords.empty( ) )
			{
				g_FuckThisSDK->m_ShotData.erase( g_FuckThisSDK->m_ShotData.begin( ) + i );
				continue;
			}

			this->AdjustPlayerRecord( Shot->m_Target.m_Player, Shot->m_Target.m_LagRecord );
			for ( auto& Impact : Shot->m_vecImpacts )
			{
				if ( !this->DoesIntersectHitbox( Shot->m_Target.m_Player, Shot->m_Target.m_Hitbox.m_iHitbox, Shot->m_vecStartPosition, Impact ) )
					continue;

				Shot->m_bDidIntersectPlayer = true;
				Shot->m_vecEndPosition = Impact;
				break;
			}
			this->AdjustPlayerRecord( Shot->m_Target.m_Player, LagRecords.back( ) );
		}

		if ( Shot->m_bHasBeenHurted )
		{
			g_FuckThisSDK->m_ShotData.erase( g_FuckThisSDK->m_ShotData.begin( ) + i );
			continue;
		}

		if ( !Shot->m_Target.m_Player->is_alive( ) )
		{
			g_FuckThisSDK->m_ShotData.erase( g_FuckThisSDK->m_ShotData.begin( ) + i );
			continue;
		}

		float_t flClientYaw = math::AngleNormalize( math::CalcAngle( Shot->m_vecStartPosition, Shot->m_Target.m_Hitbox.m_vecPoint ).y );
		float_t flServerYaw = math::AngleNormalize( math::CalcAngle( Shot->m_vecStartPosition, Shot->m_vecImpacts.back( ) ).y );

		bool bMissedShotDueToOcclusion = false;
		if ( Shot->m_vecStartPosition.DistTo( Shot->m_vecImpacts.back( ) ) < Shot->m_vecStartPosition.DistTo( Shot->m_Target.m_Hitbox.m_vecPoint ) )
		{
			if ( fabs( math::AngleNormalize( fabs( math::angle_diff( flClientYaw, flServerYaw ) ) ) ) <= 5.0f )
			{
				if ( g_AutoWall->GetPointDamage( Shot->m_vecStartPosition, Shot->m_vecImpacts.back( ) ) < 1.0f )
					bMissedShotDueToOcclusion = true;
			}
		}

		if ( Shot->m_bDidIntersectPlayer )
		{
			g_FuckThisSDK->m_ResolverData.m_BruteSide[ Shot->m_Target.m_Player->EntIndex( ) ] = Shot->m_Target.m_LagRecord.m_RotationMode;
			g_FuckThisSDK->m_ResolverData.m_MissedShots[ Shot->m_Target.m_Player->EntIndex( ) ]++;
		}

		g_FuckThisSDK->m_ShotData.erase( g_FuckThisSDK->m_ShotData.begin( ) + i );
	}
}

void C_RageBot::BackupPlayers( )
{
	for ( int nPlayerID = 1; nPlayerID <= g_globals->max_clients; nPlayerID++ )
	{
		C_BasePlayer* pPlayer = static_cast < C_BasePlayer* > ( g_ent_list->GetClientEntity( nPlayerID ) );
		if ( !pPlayer || !pPlayer->is_alive( ) || pPlayer->IsDormant( ) || pPlayer->get_team( ) == pPlayer->get_team( ) || !pPlayer->is_player( ) )
			continue;

		if ( g_FuckThisSDK->m_CachedPlayerRecords[ nPlayerID ].empty( ) )
			continue;

		m_BackupData[ nPlayerID ] = g_FuckThisSDK->m_CachedPlayerRecords[ nPlayerID ].back( );
		m_BackupData[ nPlayerID ].m_AdjustTick = g_FuckThisSDK->m_pCmd->command_number;
	}
}

void C_RageBot::RestorePlayers( )
{
	for ( int nPlayerID = 1; nPlayerID <= g_globals->max_clients; nPlayerID++ )
	{
		C_BasePlayer* pPlayer = static_cast < C_BasePlayer* > ( g_ent_list->GetClientEntity( nPlayerID ) );
		if ( !pPlayer || !pPlayer->is_alive( ) || pPlayer->IsDormant( ) || pPlayer->get_team( ) == pPlayer->get_team( ) || !pPlayer->is_player( ) )
			continue;

		if ( m_BackupData[ nPlayerID ].m_AdjustTick != g_FuckThisSDK->m_pCmd->command_number )
			continue;

		pPlayer->get_flags( ) = m_BackupData[ nPlayerID ].m_Flags;
		pPlayer->m_flSimulationTime( ) = m_BackupData[ nPlayerID ].m_SimulationTime;
		pPlayer->m_angEyeAngles( ) = m_BackupData[ nPlayerID ].m_EyeAngles;

		pPlayer->get_origin( ) = ( m_BackupData[ nPlayerID ].m_Origin );
		pPlayer->set_abs_origin( m_BackupData[ nPlayerID ].m_Origin );

		std::memcpy( pPlayer->GetAnimationLayers( ), m_BackupData[ nPlayerID ].m_AnimationLayers.at( ROTATE_SERVER ).data( ), sizeof( C_AnimationLayer ) * 13 );
		std::memcpy( pPlayer->m_aPoseParameters( ).data( ), m_BackupData[ nPlayerID ].m_PoseParameters.data( ), sizeof( float_t ) * 24 );
	
		pPlayer->GetCollideable( )->OBBMaxs( ) = m_BackupData[ nPlayerID ].m_Maxs;
		pPlayer->GetCollideable( )->OBBMins( ) = m_BackupData[ nPlayerID ].m_Mins;
	
		this->AdjustPlayerBones( pPlayer, m_BackupData[ nPlayerID ].m_Matricies[ ROTATE_SERVER ] );
	}
}

void C_RageBot::AutoStop( )
{
	if ( !this->CanAutoStop( ) || !m_RageSettings.m_bAutoStop )
		return;

	if ( !m_RageSettings.m_AutoStopOptions[ AUTOSTOP_ACCURACY ] )
	{
		if ( g_globals->GetLocalPlayer( )->get_velocity( ).Length2D( ) < g_globals->GetLocalPlayer( )->GetMaxPlayerSpeed( ) * 0.34f )
		{
			g_AntiAim->SlowWalk( true );
			return;
		}
	}

	QAngle angResistance = QAngle( 0, 0, 0 );
    math::vector_angles( ( g_globals->GetLocalPlayer( )->get_velocity( ) * -1.f ), angResistance );

	angResistance.y = g_FuckThisSDK->m_pCmd->viewangles.y - angResistance.y;
	angResistance.x = g_FuckThisSDK->m_pCmd->viewangles.x - angResistance.x;

    Vector vecResistance = Vector( 0, 0, 0 );
    math::angle_vectors( angResistance, &vecResistance );

    g_FuckThisSDK->m_pCmd->forwardmove = std::clamp( vecResistance.x, -450.f, 450.0f );
	g_FuckThisSDK->m_pCmd->sidemove = std::clamp( vecResistance.y, -450.f, 450.0f );
}

void C_RageBot::AutoScope( )
{
	if ( g_globals->GetLocalPlayer( )->is_scoped( ) || !g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( ) || !g_globals->GetLocalPlayer( )->get_weapon_handle( )->IsSniper( ) )
		return;

	if ( m_RageSettings.m_bAutoScope )
		g_FuckThisSDK->m_pCmd->buttons |= IN_ATTACK2;
}

void C_RageBot::AdjustPlayerBones( C_BasePlayer* pPlayer, std::array < matrix3x4_t, MAXSTUDIOBONES > aMatrix )
{
	std::memcpy( pPlayer->m_CachedBoneData( ).Base( ), aMatrix.data( ), sizeof( matrix3x4_t ) * pPlayer->m_CachedBoneData( ).Count( ) );
	return pPlayer->InvalidateBoneCache( );
}

void C_RageBot::AdjustPlayerRecord( C_BasePlayer* pPlayer, C_LagRecord LagRecord )
{
	pPlayer->get_flags( ) = LagRecord.m_Flags;
	pPlayer->m_flSimulationTime( ) = LagRecord.m_SimulationTime;
	pPlayer->m_angEyeAngles( ) = LagRecord.m_EyeAngles;

	pPlayer->get_origin( ) = ( LagRecord.m_Origin );
	pPlayer->set_abs_origin( LagRecord.m_Origin );

	std::memcpy( pPlayer->GetAnimationLayers( ), LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).data( ), sizeof( C_AnimationLayer ) * 13 );
	std::memcpy( pPlayer->m_aPoseParameters( ).data( ), LagRecord.m_PoseParameters.data( ), sizeof( float_t ) * 24 );
	
	pPlayer->GetCollideable( )->OBBMaxs( ) = LagRecord.m_Maxs;
	pPlayer->GetCollideable( )->OBBMins( ) = LagRecord.m_Mins;
	
	return this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_SERVER ] );
}

typedef int32_t( __fastcall* ClipRayToHitbox_t )( const Ray_t&, mstudiobbox_t*, matrix3x4_t&, trace_t& );
bool C_RageBot::DoesIntersectHitbox( C_BasePlayer* pPlayer, int32_t iHitbox, Vector vecStartPosition, Vector vecEndPosition )
{
	if ( !pPlayer || !pPlayer->is_alive( ) )
		return false;

	trace_t Trace;
	Ray_t Ray;

	Ray.Init( vecStartPosition, vecEndPosition );
	Trace.fraction = 1.0f;
	Trace.startsolid = false;

	studiohdr_t* pStudioHdr = ( studiohdr_t* )( g_mdl_info->get_studio_model( pPlayer->GetModel( ) ) );
	if ( !pStudioHdr)
		return false;

	mstudiohitboxset_t* pHitset = pStudioHdr->GetHitboxSet( pPlayer->get_hitbox_set( ) );
	if ( !pHitset )
		return false;

	mstudiobbox_t* pHitbox = pHitset->GetHitbox( iHitbox );
	if ( !pHitbox )
		return false;

	static auto pClipRayToHitbox = ( void* )( utils::find_signature( "client.dll", "55 8B EC 83 E4 F8 F3 0F 10 42" ) );
	return ( ( ClipRayToHitbox_t )( pClipRayToHitbox ) )( Ray, pHitbox, pPlayer->m_CachedBoneData( ).Base( )[ pHitbox->bone ], Trace ) > -1;
}

bool C_RageBot::IsSafePoint( C_BasePlayer* pPlayer, C_LagRecord LagRecord, Vector vecStartPosition, Vector vecEndPosition, int32_t iHitbox )
{
	this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_LEFT ] );
	if ( !this->DoesIntersectHitbox( pPlayer, iHitbox, vecStartPosition, vecEndPosition ) )
	{
		this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_SERVER ] );
		return false; 
	}

	this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_CENTER ] );
	if ( !this->DoesIntersectHitbox( pPlayer, iHitbox, vecStartPosition, vecEndPosition ) )
	{
		this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_SERVER ] );
		return false;
	}

	this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_RIGHT ] );
	if ( !this->DoesIntersectHitbox( pPlayer, iHitbox, vecStartPosition, vecEndPosition ) )
	{
		this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_SERVER ] );
		return false;
	}

	this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_SERVER ] );
	return true;
}

std::vector < Vector > C_RageBot::GetHitboxPoints( C_BasePlayer* pPlayer, C_LagRecord LagRecord, Vector vecStartPosition, int32_t iHitbox )
{
	studiohdr_t* pStudioHdr = ( studiohdr_t* )( g_mdl_info->get_studio_model( pPlayer->GetModel( ) ) );
	if ( !pStudioHdr)
		return { };

	mstudiohitboxset_t* pHitset = pStudioHdr->GetHitboxSet( pPlayer->get_hitbox_set( ) );
	if ( !pHitset )
		return { };

	mstudiobbox_t* pHitbox = pHitset->GetHitbox( iHitbox );
	if ( !pHitbox )
		return { };

	float_t flModifier = fmaxf( pHitbox->m_flRadius, 0.f );

	Vector vecMax;
	Vector vecMin;

	math::vector_transform( Vector( pHitbox->bbmax.x + flModifier, pHitbox->bbmax.y + flModifier, pHitbox->bbmax.z + flModifier ), LagRecord.m_Matricies[ ROTATE_SERVER ][ pHitbox->bone ], vecMax );
	math::vector_transform( Vector( pHitbox->bbmin.x - flModifier, pHitbox->bbmin.y - flModifier, pHitbox->bbmin.z - flModifier ), LagRecord.m_Matricies[ ROTATE_SERVER ][ pHitbox->bone ], vecMin );

	Vector vecCenter = ( vecMin + vecMax ) * 0.5f;

	QAngle angAngle = math::CalcAngle( vecStartPosition, vecCenter );

	Vector vecForward;
	math::angle_vectors( angAngle, &vecForward );

	Vector vecRight = vecForward.Cross( Vector( 0, 0, 2.33f ) );
	Vector vecLeft = Vector( -vecRight.x, -vecRight.y, vecRight.z );

	Vector vecTop = Vector( 0, 0, 3.25f );
	Vector vecBottom = Vector( 0, 0, -3.25f );

	int32_t iAngleToPlayer = ( int32_t )( fabsf( math::AngleNormalize( math::AngleNormalize( math::AngleNormalize( pPlayer->m_angEyeAngles( ).y ) - math::AngleNormalize( math::CalcAngle( vecStartPosition, pPlayer->get_abs_origin( ) ).y + 180.0f ) ) ) ) );
	int32_t iDistanceToPlayer = ( int32_t )( g_globals->GetLocalPlayer( )->get_abs_origin( ).DistTo( pPlayer->get_origin( ) ) );

	std::vector < Vector > aMultipoints = { };
	if ( iHitbox == HITBOX_HEAD )
	{
		if ( !m_RageSettings.m_Multipoints[ 0 ] )
		{
			aMultipoints.emplace_back( vecCenter );
			return aMultipoints;
		}

		float_t flScale = m_RageSettings.m_iHeadScale;

		aMultipoints.emplace_back( vecCenter );
		aMultipoints.emplace_back( vecCenter + ( ( vecTop + vecRight ) * ( flScale / 75.0f ) ) );
		aMultipoints.emplace_back( vecCenter + ( ( vecTop + vecLeft ) * ( flScale / 75.0f ) ) );
	}
	else if ( iHitbox == HITBOX_CHEST )
	{
		if ( !m_RageSettings.m_Multipoints[ 1 ] )
		{
			aMultipoints.emplace_back( vecCenter );
			return aMultipoints;
		}

		float_t flModifier = 3.05f * ( m_RageSettings.m_iBodyScale / 80.0f );
		if ( iAngleToPlayer < 140 && iAngleToPlayer > 30 )
			flModifier = 2.0f * ( m_RageSettings .m_iBodyScale / 80.0f );

		aMultipoints.emplace_back( vecCenter + Vector( 0, 0, 3 ) );
		aMultipoints.emplace_back( vecCenter + vecRight * flModifier + Vector( 0, 0, 3 ) );
		aMultipoints.emplace_back( vecCenter + vecLeft * flModifier + Vector( 0, 0, 3 ) );
	}
	else if ( iHitbox == HITBOX_STOMACH )
	{
		float_t flModifier = m_RageSettings.m_iBodyScale / 33.0f;
		if ( iAngleToPlayer < 120 && iAngleToPlayer > 60 )
			flModifier = ( m_RageSettings.m_iBodyScale / 33.0f ) - 0.10f;

		aMultipoints.emplace_back( vecCenter + Vector( 0, 0, 3.0f ) );
		aMultipoints.emplace_back( vecCenter + vecRight * flModifier + Vector( 0.0f, 0.0f, 3.0f ) );
		aMultipoints.emplace_back( vecCenter + vecLeft * flModifier + Vector( 0.0f, 0.0f, 3.0f ) );
	}
	else if ( iHitbox == HITBOX_PELVIS )
	{
		float_t flModifier = m_RageSettings.m_iBodyScale / 33.0f;
		if ( iAngleToPlayer < 120 && iAngleToPlayer > 60 )
			flModifier = ( m_RageSettings.m_iBodyScale / 33.0f ) - 0.10f;

		aMultipoints.emplace_back( vecCenter - Vector( 0.0f, 0.0f, 2.0f ) );
		aMultipoints.emplace_back( vecCenter + vecRight * flModifier - Vector( 0.0f, 0.0f, 2.0f ) );
		aMultipoints.emplace_back( vecCenter + vecLeft * flModifier - Vector( 0.0f, 0.0f, 2.0f ) );
	}
	else if ( iHitbox == HITBOX_LEFT_FOOT || iHitbox == HITBOX_RIGHT_FOOT || iHitbox == HITBOX_LEFT_THIGH || iHitbox == HITBOX_RIGHT_THIGH )
	{
		if ( !m_RageSettings.m_Multipoints[ 5 ] )
		{
			aMultipoints.emplace_back( vecCenter );
			return aMultipoints;
		}

		Vector vecAddition = vecLeft;
		if ( iHitbox == HITBOX_LEFT_FOOT || iHitbox == HITBOX_LEFT_THIGH )
			vecAddition = vecRight;
		else if ( iHitbox == HITBOX_RIGHT_FOOT || iHitbox == HITBOX_RIGHT_THIGH )
			vecAddition = vecLeft;

		if ( iHitbox == HITBOX_LEFT_THIGH || iHitbox == HITBOX_RIGHT_THIGH )
			vecCenter -= Vector( 0.0f, 0.0f, 2.5f );

		aMultipoints.emplace_back( vecCenter - ( vecAddition * 0.90f ) );
	}
	else if ( iHitbox == HITBOX_LEFT_FOREARM || iHitbox == HITBOX_RIGHT_FOREARM )
		aMultipoints.emplace_back( vecCenter - ( iHitbox == HITBOX_LEFT_FOREARM ? vecLeft : -vecLeft ) );

	return aMultipoints;
}

void C_RageBot::AutoRevolver( )
{
	if ( ( g_globals->GetLocalPlayer( )->get_flags( ) & entity_flags::fl_frozen ) || !m_RageSettings.m_bEnabled )
		return;

	C_BaseCombatWeapon* pCombatWeapon = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( );
	if ( !pCombatWeapon || !m_RageSettings.m_bEnabled || pCombatWeapon->get_definition_index( ) != WEAPON_REVOLVER )
		return;
	
	g_FuckThisSDK->m_pCmd->buttons &= ~IN_ATTACK2;
	if ( !g_globals->GetLocalPlayer( )->CanFire( ) )
		return;

	if ( pCombatWeapon->get_post_pone_fire_ready_time( ) <= TICKS_TO_TIME( g_globals->GetLocalPlayer( )->get_tick_base( ) ) )
	{
		if ( ( pCombatWeapon->get_next_secondary_attack( ) + ( g_globals->interval_per_tick * -3.0f ) ) <= TICKS_TO_TIME( g_globals->GetLocalPlayer( )->get_tick_base( ) ) )
			g_FuckThisSDK->m_pCmd->buttons |= IN_ATTACK;
	}
	else
		g_FuckThisSDK->m_pCmd->buttons |= IN_ATTACK;
}

void C_RageBot::TaserBot( )
{

}

float_t C_RageBot::GetHitChance( C_TargetData Target )
{
	QAngle angViewAngles = math::CalcAngle( g_FuckThisSDK->m_LocalData.m_vecShootPosition, Target.m_Hitbox.m_vecPoint );
	math::normalize_angles( angViewAngles );

	Vector vecForward, vecRight, vecUp;
	math::angle_vectors( angViewAngles, vecForward, vecRight, vecUp );

	C_BaseCombatWeapon* pWeapon = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( );
	if ( !pWeapon )
		return 0.0f;

	CCSWeaponInfo* pWeaponData = pWeapon->get_weapon_data( );
	if ( !pWeaponData )
		return 0.0f;

	float flMaxDistance = pWeaponData->m_flRange;
	if ( g_FuckThisSDK->m_LocalData.m_vecShootPosition.DistTo( Target.m_Hitbox.m_vecPoint ) > flMaxDistance )
		return 0.0f;

	if ( this->HasMaximumAccuracy( ) )
		return 100.0f;
		
	float_t flWeaponSpread = pWeapon->get_spread( );
	float_t flWeaponInaccuracy = pWeapon->get_inaccuracy( );
	
	int nValidSeeds = 0;
	for ( int iSeed = 0; iSeed < 256; iSeed++ )
	{
		float_t flInaccuracy = g_FuckThisSDK->m_AccuracyData.m_aInaccuracy[ iSeed ]	* flWeaponInaccuracy;
		float_t flSpread = g_FuckThisSDK->m_AccuracyData.m_aSpread[ iSeed ]			* flWeaponSpread;

		Vector vecSpreadView
		(
			( cos( g_FuckThisSDK->m_AccuracyData.m_FirstRandom[ iSeed ] ) * flInaccuracy ) + ( cos( g_FuckThisSDK->m_AccuracyData.m_SecondRandom[ iSeed ] ) * flSpread ),
			( sin( g_FuckThisSDK->m_AccuracyData.m_FirstRandom[ iSeed ] ) * flInaccuracy ) + ( sin( g_FuckThisSDK->m_AccuracyData.m_SecondRandom[ iSeed ] ) * flSpread ),
			0.0f
		);

		Vector vecDirection = Vector( 0.0f, 0.0f, 0.0f );
		vecDirection.x = vecForward.x + ( vecSpreadView.x * vecRight.x ) + ( vecSpreadView.y * vecUp.x );
		vecDirection.y = vecForward.y + ( vecSpreadView.x * vecRight.y ) + ( vecSpreadView.y * vecUp.y );
		vecDirection.z = vecForward.z + ( vecSpreadView.x * vecRight.z ) + ( vecSpreadView.y * vecUp.z );
		vecDirection.Normalized( );

		QAngle angSpreadView;
		math::VectorAngles( vecDirection, vecUp, angSpreadView );
		math::normalize_angles( angSpreadView );

		Vector vecForwardView;
		math::angle_vectors( angSpreadView, &vecForwardView );
		vecForwardView.NormalizeInPlace( );

		vecForwardView = g_FuckThisSDK->m_LocalData.m_vecShootPosition + ( vecForwardView * pWeaponData->m_flRange );
		if ( g_AutoWall->GetPointDamage( g_FuckThisSDK->m_LocalData.m_vecShootPosition, vecForwardView ) )
			nValidSeeds++;

		if ( ( 256 - iSeed + nValidSeeds ) < static_cast< int32_t >( ( float_t ) ( m_RageSettings.m_iHitChance ) * 2.56f ) )
			return ( float_t ) ( nValidSeeds ) / 2.56f;
	}

	return ( float )( nValidSeeds ) / 2.56f;
}

float_t C_RageBot::GetAccuracyBoost( C_TargetData Target )
{
	QAngle angViewAngles = math::CalcAngle( g_FuckThisSDK->m_LocalData.m_vecShootPosition, Target.m_Hitbox.m_vecPoint );
	math::normalize_angles( angViewAngles );

	Vector vecForward, vecRight, vecUp;
	math::angle_vectors( angViewAngles, vecForward, vecRight, vecUp );

	C_BaseCombatWeapon* pWeapon = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( );
	if ( !pWeapon )
		return 0.0f;

	CCSWeaponInfo* pWeaponData = pWeapon->get_weapon_data( );
	if ( !pWeaponData )
		return 0.0f;
	
	if ( pWeapon->get_definition_index( ) == WEAPON_TASER )
		return 100.0f;

	if ( g_FuckThisSDK->m_AccuracyData.m_bHasMaximumAccuracy || g_FuckThisSDK->m_AccuracyData.m_bDoingSecondShot )
		return 100.0f;

	float_t flWeaponSpread = pWeapon->get_spread( );
	float_t flWeaponInaccuracy = pWeapon->get_inaccuracy( );
	int32_t iAccuracyValidSeeds = 0;
		
	for ( int32_t iSeed = 0; iSeed < 256; iSeed++ )
	{
		float_t flInaccuracy = g_FuckThisSDK->m_AccuracyData.m_aInaccuracy[ iSeed ]	* flWeaponInaccuracy;
		float_t flSpread = g_FuckThisSDK->m_AccuracyData.m_aSpread[ iSeed ]			* flWeaponSpread;

		Vector vecSpreadView
		( 
			( cos( g_FuckThisSDK->m_AccuracyData.m_FirstRandom[ iSeed ] ) * flInaccuracy ) + ( cos( g_FuckThisSDK->m_AccuracyData.m_SecondRandom[ iSeed ] ) * flSpread ),
			( sin( g_FuckThisSDK->m_AccuracyData.m_FirstRandom[ iSeed ] ) * flInaccuracy ) + ( sin( g_FuckThisSDK->m_AccuracyData.m_SecondRandom[ iSeed ] ) * flSpread ),
			0.0f 
		);

		Vector vecDirection = Vector( 0.0f, 0.0f, 0.0f );
		vecDirection.x = vecForward.x + ( vecSpreadView.x * vecRight.x ) + ( vecSpreadView.y * vecUp.x );
		vecDirection.y = vecForward.y + ( vecSpreadView.x * vecRight.y ) + ( vecSpreadView.y * vecUp.y );
		vecDirection.z = vecForward.z + ( vecSpreadView.x * vecRight.z ) + ( vecSpreadView.y * vecUp.z );
		vecDirection.Normalized( );

		QAngle angSpreadView;
		math::VectorAngles( vecDirection, vecUp, angSpreadView );
		math::normalize_angles( angSpreadView );

		Vector vecForwardView;
		math::angle_vectors( angSpreadView, &vecForwardView );
		vecForwardView.NormalizeInPlace( );
		
		vecForwardView = g_FuckThisSDK->m_LocalData.m_vecShootPosition + ( vecForwardView * pWeaponData->m_flRange );
		if ( g_AutoWall->GetPointDamage( g_FuckThisSDK->m_LocalData.m_vecShootPosition, vecForwardView ) )
			iAccuracyValidSeeds++;

		if ( ( 256 - iSeed + iAccuracyValidSeeds ) < static_cast< int32_t >( ( float_t )( m_RageSettings.m_iAccuracyBoost ) * 2.56f ) )
			return ( float_t )( iAccuracyValidSeeds ) / 2.56f;
	}

	return ( float_t )( iAccuracyValidSeeds ) / 2.56f;
}

bool C_RageBot::HasMaximumAccuracy( )
{
	C_BaseCombatWeapon* pWeapon = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( );
	if ( !pWeapon )
		return false;

	if ( !pWeapon->IsGun( ) )
		return false;

	CCSWeaponInfo* pWeaponData = pWeapon->get_weapon_data( );
	if ( !pWeaponData )
		return false;

	if ( pWeapon->get_next_primary_attack( ) <= g_globals->curtime )
		return false;

	return pWeapon->get_inaccuracy( ) - g_FuckThisSDK->m_AccuracyData.m_flMinInaccuracy < 0.0001f;
}

bool C_RageBot::CanAutoStop( )
{
	if ( !g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( ) || !g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( )->IsGun( ) )
		return false;

	if ( !g_globals->GetLocalPlayer( )->CanFire( ) )
		return false;

	if ( !( g_globals->GetLocalPlayer( )->get_flags( ) & entity_flags::fl_on_ground ) )
		return false;

	if ( !g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( ) || !g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( )->get_weapon_data( ) )
		return false;

	return m_RageSettings.m_bAutoStop;
}