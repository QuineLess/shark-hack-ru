#include "Animations.hpp"
#include "BoneManager.hpp"

void C_AnimationSync::Instance( )
{
	for ( int32_t iPlayerID = 1; iPlayerID <= g_globals->max_clients; iPlayerID++ )
	{
		C_BasePlayer* pPlayer = static_cast < C_BasePlayer* >( g_ent_list->GetClientEntity( iPlayerID ) );
		if ( !pPlayer || !pPlayer->is_player( ) || !pPlayer->is_alive( ) || pPlayer->get_team( ) == g_globals->GetLocalPlayer( )->get_team( ) )
		{
			g_FuckThisSDK->m_ResolverData.m_AnimResoled[ iPlayerID ] = false;
			g_FuckThisSDK->m_ResolverData.m_MissedShots[ iPlayerID ] = 0;
			g_FuckThisSDK->m_ResolverData.m_LastMissedShots[ iPlayerID ] = 0;

			continue;
		}

		bool bHasPreviousRecord = false;
		if ( pPlayer->m_flOldSimulationTime( ) >= pPlayer->m_flSimulationTime( ) )
		{
			if ( pPlayer->m_flOldSimulationTime( ) > pPlayer->m_flSimulationTime( ) )
				this->UnmarkAsDormant( iPlayerID );

			continue;
		}

		auto& LagRecords = g_FuckThisSDK->m_CachedPlayerRecords[ iPlayerID ];
		if ( LagRecords.empty( ) )
			continue;

		C_LagRecord PreviousRecord = m_PreviousRecord[ iPlayerID ];
		if ( TIME_TO_TICKS( fabs( pPlayer->m_flSimulationTime( ) - PreviousRecord.m_SimulationTime ) ) <= 17 )
			bHasPreviousRecord = true;

		C_LagRecord& LatestRecord = LagRecords.back( );
		if ( this->HasLeftOutOfDormancy( iPlayerID ) )
			bHasPreviousRecord = false;

		if ( LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).m_flCycle == PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).m_flCycle )
		{
			pPlayer->m_flSimulationTime( ) = pPlayer->m_flOldSimulationTime( );
			continue;
		}

		LatestRecord.m_UpdateDelay = TIME_TO_TICKS( pPlayer->m_flSimulationTime( ) - this->GetPreviousRecord( iPlayerID ).m_SimulationTime );
		if ( LatestRecord.m_UpdateDelay > 17 )
			LatestRecord.m_UpdateDelay = 1;

		if ( LatestRecord.m_UpdateDelay < 1 )
			continue;
		
		if ( bHasPreviousRecord )
		{
			float_t flWeight = 1.0f - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).m_flWeight;
			if ( flWeight > 0.0f )
			{
				float_t flPreviousRate = this->GetPreviousRecord( pPlayer->EntIndex( ) ).m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).m_flPlaybackRate;
				float_t flCurrentRate = LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).m_flPlaybackRate;

				if ( flPreviousRate == flCurrentRate )
				{
					int32_t iPreviousSequence = this->GetPreviousRecord( pPlayer->EntIndex( ) ).m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).m_nSequence;
					int32_t iCurrentSequence = LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).m_nSequence;

					if ( iPreviousSequence == iCurrentSequence )
					{
						float_t flSpeedNormalized = ( flWeight / 2.8571432f ) + 0.55f;
						if ( flSpeedNormalized > 0.0f )
						{
							float_t flSpeed = flSpeedNormalized * pPlayer->GetMaxPlayerSpeed( );
							if ( flSpeed > 0.0f )
								if ( LatestRecord.m_Velocity.Length2D( ) > 0.0f )
									LatestRecord.m_Velocity = ( LatestRecord.m_Velocity / LatestRecord.m_Velocity.Length( ) ) * flSpeed;
						}
					}
				} 
			}
		}
			
		std::array < C_AnimationLayer, 13 > AnimationLayers;
		std::array < float_t, 24 > PoseParameters;
		C_CSGOPlayerAnimationState AnimationState;

		// бэкапим данные
		std::memcpy( AnimationLayers.data( ), pPlayer->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * 13 );
		std::memcpy( PoseParameters.data( ), pPlayer->m_aPoseParameters( ).data( ), sizeof( float_t ) * 24 );
		std::memcpy( &AnimationState, pPlayer->m_PlayerAnimationStateCSGO( ), sizeof( AnimationState ) );

		// ротейтим игрока для сэйфпоинтов
		for ( int32_t i = ROTATE_LEFT; i <= ROTATE_LOW_RIGHT; i++ )
		{
			// ротейтим игрока твою мать 
			this->UpdatePlayerAnimations( pPlayer, LatestRecord, PreviousRecord, bHasPreviousRecord, i );

			// сохраняем некоторые данные
			std::memcpy( LatestRecord.m_AnimationLayers.at( i ).data( ), pPlayer->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * 13 );

			// сетапим лееры сервера
			std::memcpy( pPlayer->GetAnimationLayers( ), AnimationLayers.data( ), sizeof( C_AnimationLayer ) * 13 );

			// сетапим кости
			if ( i < ROTATE_LOW_LEFT )
				g_BoneManager->BuildMatrix( pPlayer, LatestRecord.m_Matricies[ i ].data( ), true );

			// ресторим дефолтные данные
			std::memcpy( pPlayer->m_aPoseParameters( ).data( ), PoseParameters.data( ), sizeof( float_t ) * 24 );
			std::memcpy( pPlayer->m_PlayerAnimationStateCSGO( ), &AnimationState, sizeof( AnimationState ) );
		}

		if ( !LatestRecord.m_bIsShooting )
		{
			if ( LatestRecord.m_UpdateDelay > 1 && bHasPreviousRecord )
			{
				LatestRecord.m_RotationMode = g_FuckThisSDK->m_ResolverData.m_LastBruteSide[ iPlayerID ];
				if ( LatestRecord.m_Velocity.Length2D( ) > 1.0f )
				{
					float_t flLeftDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_LEFT ).at( 6 ).m_flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).m_flPlaybackRate );
					float_t flLowLeftDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_LOW_LEFT ).at( 6 ).m_flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).m_flPlaybackRate );
					float_t flLowRightDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_LOW_RIGHT ).at( 6 ).m_flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).m_flPlaybackRate );
					float_t flRightDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_RIGHT ).at( 6 ).m_flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).m_flPlaybackRate );
					float_t flCenterDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_CENTER ).at( 6 ).m_flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).m_flPlaybackRate );
				
					LatestRecord.m_bAnimResolved = false;
					{
						float flLastDelta = 0.0f;
						if ( flLeftDelta > flCenterDelta )
						{
							LatestRecord.m_RotationMode = ROTATE_LEFT;
							flLastDelta = flLastDelta;
						}

						if ( flRightDelta > flLastDelta )
						{
							LatestRecord.m_RotationMode = ROTATE_RIGHT;
							flLastDelta = flRightDelta;
						}

						if ( flLowLeftDelta > flLastDelta )
						{
							LatestRecord.m_RotationMode = ROTATE_LOW_LEFT;
							flLastDelta = flLowLeftDelta;
						}

						if ( flLowRightDelta > flLastDelta )
						{
							LatestRecord.m_RotationMode = ROTATE_LOW_RIGHT;
							flLastDelta = flLowRightDelta;
						}

						LatestRecord.m_bAnimResolved = true;
					}
			
					bool bIsValidResolved = true;
					if ( bHasPreviousRecord )
					{
						if ( fabs( ( LatestRecord.m_Velocity - PreviousRecord.m_Velocity ).Length2D( ) ) > 5.0f || PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 7 ).m_flWeight < 1.0f )
							bIsValidResolved = false;
					}

					g_FuckThisSDK->m_ResolverData.m_AnimResoled[ iPlayerID ] = LatestRecord.m_bAnimResolved;
					if ( LatestRecord.m_bAnimResolved && LatestRecord.m_RotationMode != ROTATE_SERVER )
						g_FuckThisSDK->m_ResolverData.m_LastBruteSide[ iPlayerID ] = LatestRecord.m_RotationMode;
			
					if ( LatestRecord.m_RotationMode == ROTATE_SERVER )
						LatestRecord.m_RotationMode = g_FuckThisSDK->m_ResolverData.m_LastBruteSide[ iPlayerID ];
				}
				else
				{
					if ( g_FuckThisSDK->m_ResolverData.m_LastBruteSide[ iPlayerID ] < 0 )
					{
						float_t flFeetDelta = math::AngleNormalize( math::angle_diff( math::AngleNormalize( pPlayer->m_flLowerBodyYaw( ) ), math::AngleNormalize( pPlayer->m_angEyeAngles( ).y ) ) );
						if ( flFeetDelta > 0.0f )
							LatestRecord.m_RotationMode = ROTATE_LEFT;
						else
							LatestRecord.m_RotationMode = ROTATE_RIGHT;

						g_FuckThisSDK->m_ResolverData.m_LastBruteSide[ iPlayerID ] = LatestRecord.m_RotationMode;
					}
				}

				g_FuckThisSDK->m_ResolverData.m_AnimResoled[ iPlayerID ] = LatestRecord.m_bAnimResolved;
				if ( LatestRecord.m_RotationMode == g_FuckThisSDK->m_ResolverData.m_BruteSide[ iPlayerID ] )
				{
					if ( g_FuckThisSDK->m_ResolverData.m_MissedShots[ iPlayerID ] > 0 )
					{
						int iNewRotation = 0;
						switch ( LatestRecord.m_RotationMode )
						{
							case ROTATE_LEFT: iNewRotation = ROTATE_RIGHT; break;
							case ROTATE_RIGHT: iNewRotation = ROTATE_LEFT; break;
							case ROTATE_LOW_RIGHT: iNewRotation = ROTATE_LOW_LEFT; break;
							case ROTATE_LOW_LEFT: iNewRotation = ROTATE_LOW_RIGHT; break;
						}

						LatestRecord.m_RotationMode = iNewRotation;
					}
				}

				this->UpdatePlayerAnimations( pPlayer, LatestRecord, PreviousRecord, bHasPreviousRecord, LatestRecord.m_RotationMode );
			}
			else
				this->UpdatePlayerAnimations( pPlayer, LatestRecord, PreviousRecord, bHasPreviousRecord, ROTATE_SERVER );
		}
		else
			this->UpdatePlayerAnimations( pPlayer, LatestRecord, PreviousRecord, bHasPreviousRecord, ROTATE_SERVER );

		// форсим правильные лееры
		std::memcpy( pPlayer->GetAnimationLayers( ), AnimationLayers.data( ), sizeof( C_AnimationLayer ) * 13 );

		// сэйвим позы
		std::memcpy( LatestRecord.m_PoseParameters.data( ), pPlayer->m_aPoseParameters( ).data( ), sizeof( float_t ) * 24 );

		// сетапим кости
		g_BoneManager->BuildMatrix( pPlayer, LatestRecord.m_Matricies[ ROTATE_SERVER ].data( ), false );

		// сэйвим кости
		for ( int i = 0; i < MAXSTUDIOBONES; i++ )
			m_BoneOrigins[ iPlayerID ][ i ] = pPlayer->get_abs_origin( ) - LatestRecord.m_Matricies[ ROTATE_SERVER ][ i ].GetOrigin( );

		// кэшируем кости
		std::memcpy( m_CachedMatrix[ iPlayerID ].data( ), LatestRecord.m_Matricies[ ROTATE_SERVER ].data( ), sizeof( matrix3x4_t ) * MAXSTUDIOBONES );

		// плеер вышел с дорманта
		this->UnmarkAsDormant( iPlayerID );
	}
}

void C_AnimationSync::UpdatePlayerAnimations( C_BasePlayer* pPlayer, C_LagRecord& LagRecord, C_LagRecord PreviousRecord, bool bHasPreviousRecord, int32_t iRotationMode )
{
	float_t flCurTime = g_globals->curtime;
	float_t flRealTime = g_globals->realtime;
	float_t flAbsFrameTime = g_globals->absoluteframetime;
	float_t flFrameTime = g_globals->frametime;
	float_t iFrameCount = g_globals->framecount;
	float_t iTickCount = g_globals->tickcount;
	float_t flInterpolationAmount = g_globals->interpolation_amount;

	float_t flLowerBodyYaw = LagRecord.m_LowerBodyYaw;
	float_t flDuckAmount = LagRecord.m_DuckAmount;
	int32_t iFlags = LagRecord.m_Flags;
	int32_t iEFlags = pPlayer->m_iEFlags( );
	
	if ( this->HasLeftOutOfDormancy( pPlayer->EntIndex( ) ) )
	{
		float_t flLastUpdateTime = LagRecord.m_SimulationTime - g_globals->interval_per_tick;
		if ( pPlayer->get_flags( ) & entity_flags::fl_on_ground )
		{
			pPlayer->m_PlayerAnimationStateCSGO( )->m_bLanding = false;
			pPlayer->m_PlayerAnimationStateCSGO( )->m_bOnGround = true;

			float_t flLandTime = 0.0f;
			if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).m_flCycle > 0.0f && 
				 LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).m_flPlaybackRate > 0.0f )
			{ 
				int32_t iLandActivity = pPlayer->GetSequenceActivity( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).m_nSequence );
				if ( iLandActivity == ACT_CSGO_LAND_LIGHT || iLandActivity == ACT_CSGO_LAND_HEAVY )
				{
					flLandTime = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).m_flCycle / LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).m_flPlaybackRate;
					if ( flLandTime > 0.0f )
						flLastUpdateTime = LagRecord.m_SimulationTime - flLandTime;
				}
			}

			LagRecord.m_Velocity.z = 0.0f;
		}
		else
		{
			float_t flJumpTime = 0.0f;
			if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).m_flCycle > 0.0f && 
				 LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).m_flPlaybackRate > 0.0f )
			{ 
				int32_t iJumpActivity = pPlayer->GetSequenceActivity( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).m_nSequence );
				if ( iJumpActivity == ACT_CSGO_JUMP )
				{
					flJumpTime = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).m_flCycle / LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).m_flPlaybackRate;
					if ( flJumpTime > 0.0f )
						flLastUpdateTime = LagRecord.m_SimulationTime - flJumpTime;
				}
			}
			
			pPlayer->m_PlayerAnimationStateCSGO( )->m_bOnGround = false;
			pPlayer->m_PlayerAnimationStateCSGO( )->m_flDurationInAir = flJumpTime - g_globals->interval_per_tick;
		}

		float_t flWeight = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).m_flWeight;
		if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).m_flPlaybackRate < 0.00001f )
			LagRecord.m_Velocity.Zero( );
		else
		{
			float_t flPostVelocityLength = LagRecord.m_Velocity.Length( );
			if ( flWeight > 0.0f && flWeight < 0.95f )
		{
				float_t flMaxSpeed = pPlayer->GetMaxPlayerSpeed( );
				if ( flPostVelocityLength > 0.0f )
				{
					float_t flMaxSpeedMultiply = 1.0f;
					if ( pPlayer->get_flags( ) & 6 )
						flMaxSpeedMultiply = 0.34f;
					else if ( pPlayer->m_bIsWalking( ) )
						flMaxSpeedMultiply = 0.52f;

					LagRecord.m_Velocity = ( LagRecord.m_Velocity / flPostVelocityLength ) * ( flWeight * ( flMaxSpeed * flMaxSpeedMultiply ) );
				}
			}
		}

		pPlayer->m_PlayerAnimationStateCSGO( )->m_flLastUpdateTime = flLastUpdateTime;
	}

	if ( bHasPreviousRecord )
	{
		pPlayer->m_PlayerAnimationStateCSGO( )->m_flStrafeChangeCycle = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 7 ).m_flCycle;
		pPlayer->m_PlayerAnimationStateCSGO( )->m_flStrafeChangeWeight = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 7 ).m_flWeight;
		pPlayer->m_PlayerAnimationStateCSGO( )->m_nStrafeSequence = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 7 ).m_nSequence;
		pPlayer->m_PlayerAnimationStateCSGO( )->m_flPrimaryCycle = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).m_flCycle;
		pPlayer->m_PlayerAnimationStateCSGO( )->m_flMoveWeight = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).m_flWeight;
		pPlayer->m_PlayerAnimationStateCSGO( )->m_flAccelerationWeight = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 12 ).m_flWeight;
		std::memcpy( pPlayer->GetAnimationLayers( ), PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).data( ), sizeof( C_AnimationLayer ) * 13 );
	}
	else
	{
		pPlayer->m_PlayerAnimationStateCSGO( )->m_flStrafeChangeCycle = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 7 ).m_flCycle;
		pPlayer->m_PlayerAnimationStateCSGO( )->m_flStrafeChangeWeight = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 7 ).m_flWeight;
		pPlayer->m_PlayerAnimationStateCSGO( )->m_nStrafeSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 7 ).m_nSequence;
		pPlayer->m_PlayerAnimationStateCSGO( )->m_flPrimaryCycle = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).m_flCycle;
		pPlayer->m_PlayerAnimationStateCSGO( )->m_flMoveWeight = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).m_flWeight;
		pPlayer->m_PlayerAnimationStateCSGO( )->m_flAccelerationWeight = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 12 ).m_flWeight;
		std::memcpy( pPlayer->GetAnimationLayers( ), LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).data( ), sizeof( C_AnimationLayer ) * 13 );
	}

	if ( LagRecord.m_UpdateDelay > 1 )
	{
		int32_t iActivityTick = 0;
		int32_t iActivityType = 0;

		if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).m_flWeight > 0.0f && PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).m_flWeight <= 0.0f )
		{
			int32_t iLandSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).m_nSequence;
			if ( iLandSequence > 2 )
			{
				int32_t iLandActivity = pPlayer->GetSequenceActivity( iLandSequence );
				if ( iLandActivity == ACT_CSGO_LAND_LIGHT || iLandActivity == ACT_CSGO_LAND_HEAVY )
				{
					float_t flCurrentCycle = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).m_flCycle;
					float_t flCurrentRate = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).m_flPlaybackRate;
	
					if ( flCurrentCycle > 0.0f && flCurrentRate > 0.0f )
					{	
						float_t flLandTime = ( flCurrentCycle / flCurrentRate );
						if ( flLandTime > 0.0f )
						{
							iActivityTick = TIME_TO_TICKS( LagRecord.m_SimulationTime - flLandTime ) + 1;
							iActivityType = ACTIVITY_LAND;
						}
					}
				}
			}
		}

		if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).m_flCycle > 0.0f && LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).m_flPlaybackRate > 0.0f )
		{
			int32_t iJumpSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).m_nSequence;
			if ( iJumpSequence > 2 )
			{
				int32_t iJumpActivity = pPlayer->GetSequenceActivity( iJumpSequence );
				if ( iJumpActivity == ACT_CSGO_JUMP )
				{
					float_t flCurrentCycle = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).m_flCycle;
					float_t flCurrentRate = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).m_flPlaybackRate;
	
					if ( flCurrentCycle > 0.0f && flCurrentRate > 0.0f )
					{	
						float_t flJumpTime = ( flCurrentCycle / flCurrentRate );
						if ( flJumpTime > 0.0f )
						{
							iActivityTick = TIME_TO_TICKS( LagRecord.m_SimulationTime - flJumpTime ) + 1;
							iActivityType = ACTIVITY_JUMP;
						}
					}
				}
			}
		}

		for ( int32_t iSimulationTick = 1; iSimulationTick <= LagRecord.m_UpdateDelay; iSimulationTick++ )
		{
			float_t flSimulationTime = PreviousRecord.m_SimulationTime + TICKS_TO_TIME( iSimulationTick );
			g_globals->curtime = flSimulationTime;
			g_globals->realtime = flSimulationTime;
			g_globals->frametime = g_globals->interval_per_tick;
			g_globals->absoluteframetime = g_globals->interval_per_tick;
			g_globals->framecount = TIME_TO_TICKS( flSimulationTime );
			g_globals->tickcount = TIME_TO_TICKS( flSimulationTime );
			g_globals->interpolation_amount = 0.0f;
			
			pPlayer->m_flDuckAmount( ) = Interpolate( PreviousRecord.m_DuckAmount, LagRecord.m_DuckAmount, iSimulationTick, LagRecord.m_UpdateDelay );
			pPlayer->get_velocity( ) = Interpolate( PreviousRecord.m_Velocity, LagRecord.m_Velocity, iSimulationTick, LagRecord.m_UpdateDelay );
			pPlayer->m_vecAbsVelocity( ) = pPlayer->get_velocity( );
			pPlayer->m_flLowerBodyYaw( ) = PreviousRecord.m_LowerBodyYaw;

			if ( iSimulationTick < LagRecord.m_UpdateDelay )
			{
				int32_t iCurrentSimulationTick = TIME_TO_TICKS( flSimulationTime );
				if ( iActivityType > ACTIVITY_NONE )
				{
					bool bIsOnGround = pPlayer->get_flags( ) & entity_flags::fl_on_ground;
					if ( iActivityType == ACTIVITY_JUMP )
					{
						if ( iCurrentSimulationTick == iActivityTick - 1 )
							bIsOnGround = true;
						else if ( iCurrentSimulationTick == iActivityTick )
						{
							// reset animation layer
							pPlayer->GetAnimationLayers( )[ 4 ].m_flCycle = 0.0f;
							pPlayer->GetAnimationLayers( )[ 4 ].m_nSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).m_nSequence;
							pPlayer->GetAnimationLayers( )[ 4 ].m_flPlaybackRate = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).m_flPlaybackRate;

							// reset player ground state
							bIsOnGround = false;
						}
						
					}
					else if ( iActivityType == ACTIVITY_LAND )
					{
						if ( iCurrentSimulationTick == iActivityTick - 1 )
							bIsOnGround = false;
						else if ( iCurrentSimulationTick == iActivityTick )
						{
							// reset animation layer
							pPlayer->GetAnimationLayers( )[ 5 ].m_flCycle = 0.0f;
							pPlayer->GetAnimationLayers( )[ 5 ].m_nSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).m_nSequence;
							pPlayer->GetAnimationLayers( )[ 5 ].m_flPlaybackRate = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).m_flPlaybackRate;

							// reset player ground state
							bIsOnGround = true;
						}
					}

					if ( bIsOnGround )
						pPlayer->get_flags( ) |= entity_flags::fl_on_ground;
					else
						pPlayer->get_flags( ) &= ~entity_flags::fl_on_ground;
				}

				if ( iRotationMode )
				{
					LagRecord.m_BruteYaw = math::AngleNormalize( LagRecord.m_BruteYaw );
					switch ( iRotationMode )
					{
						case ROTATE_LEFT: LagRecord.m_BruteYaw = math::AngleNormalize( LagRecord.m_BruteYaw - 60.0f ); break;
						case ROTATE_RIGHT: LagRecord.m_BruteYaw = math::AngleNormalize( LagRecord.m_BruteYaw + 60.0f ); break;
						case ROTATE_LOW_LEFT: LagRecord.m_BruteYaw = math::AngleNormalize( LagRecord.m_BruteYaw - 25.0f ); break;
						case ROTATE_LOW_RIGHT: LagRecord.m_BruteYaw = math::AngleNormalize( LagRecord.m_BruteYaw + 25.0f ); break;
					}

					pPlayer->m_PlayerAnimationStateCSGO( )->m_flFootYaw = LagRecord.m_BruteYaw;
				}
			}
			else
			{
				pPlayer->get_velocity( ) = LagRecord.m_Velocity;
				pPlayer->m_vecAbsVelocity( ) = LagRecord.m_Velocity;
				pPlayer->m_flDuckAmount( ) = LagRecord.m_DuckAmount;
				pPlayer->m_flLowerBodyYaw( ) = LagRecord.m_LowerBodyYaw;
				pPlayer->get_flags( ) = LagRecord.m_Flags;
			}

			pPlayer->m_PlayerAnimationStateCSGO( )->m_nLastUpdateFrame = INT_MAX;

			bool bClientSideAnimation = pPlayer->m_bClientSideAnimation( );
			pPlayer->m_bClientSideAnimation( ) = true;
		
			for ( int32_t iLayer = NULL; iLayer < 13; iLayer++ )
				pPlayer->GetAnimationLayers( )[ iLayer ].m_pOwner = pPlayer;

			g_FuckThisSDK->m_AnimationData.m_bAnimationUpdate = true;
			pPlayer->update_client_side_animation( );
			g_FuckThisSDK->m_AnimationData.m_bAnimationUpdate = false;
		
			pPlayer->m_bClientSideAnimation( ) = bClientSideAnimation;
		}
	}
	else
	{
		g_globals->curtime = LagRecord.m_SimulationTime;
		g_globals->realtime = LagRecord.m_SimulationTime;
		g_globals->frametime = g_globals->interval_per_tick;
		g_globals->absoluteframetime = g_globals->interval_per_tick;
		g_globals->framecount = TIME_TO_TICKS( LagRecord.m_SimulationTime );
		g_globals->tickcount = TIME_TO_TICKS( LagRecord.m_SimulationTime );
		g_globals->interpolation_amount = 0.0f;

		pPlayer->get_velocity( ) = LagRecord.m_Velocity;
		pPlayer->m_vecAbsVelocity( ) = LagRecord.m_Velocity;

		if ( iRotationMode )
		{
			LagRecord.m_BruteYaw = math::AngleNormalize( LagRecord.m_BruteYaw );
			switch ( iRotationMode )
			{
				case ROTATE_LEFT: LagRecord.m_BruteYaw = math::AngleNormalize( LagRecord.m_BruteYaw - 60.0f ); break;
				case ROTATE_RIGHT: LagRecord.m_BruteYaw = math::AngleNormalize( LagRecord.m_BruteYaw + 60.0f ); break;
				case ROTATE_LOW_LEFT: LagRecord.m_BruteYaw = math::AngleNormalize( LagRecord.m_BruteYaw - 25.0f ); break;
				case ROTATE_LOW_RIGHT: LagRecord.m_BruteYaw = math::AngleNormalize( LagRecord.m_BruteYaw + 25.0f ); break;
			}

			pPlayer->m_PlayerAnimationStateCSGO( )->m_flFootYaw = LagRecord.m_BruteYaw;
		}

		if ( pPlayer->m_PlayerAnimationStateCSGO( )->m_nLastUpdateFrame > g_globals->framecount - 1 )
			pPlayer->m_PlayerAnimationStateCSGO( )->m_nLastUpdateFrame = g_globals->framecount - 1;

		bool bClientSideAnimation = pPlayer->m_bClientSideAnimation( );
		pPlayer->m_bClientSideAnimation( ) = true;
		
		for ( int32_t iLayer = NULL; iLayer < 13; iLayer++ )
			pPlayer->GetAnimationLayers( )[ iLayer ].m_pOwner = pPlayer;

		g_FuckThisSDK->m_AnimationData.m_bAnimationUpdate = true;
		pPlayer->update_client_side_animation( );
		g_FuckThisSDK->m_AnimationData.m_bAnimationUpdate = false;
		
		pPlayer->m_bClientSideAnimation( ) = bClientSideAnimation;
	}

	pPlayer->m_flLowerBodyYaw( ) = flLowerBodyYaw;
	pPlayer->m_flDuckAmount( ) = flDuckAmount;
	pPlayer->m_iEFlags( ) = iEFlags;
	pPlayer->get_flags( ) = iFlags;

	g_globals->curtime = flCurTime;
	g_globals->realtime = flRealTime;
	g_globals->absoluteframetime = flAbsFrameTime;
	g_globals->frametime = flFrameTime;
	g_globals->framecount = iFrameCount;
	g_globals->tickcount = iTickCount;
	g_globals->interpolation_amount = flInterpolationAmount;

	return pPlayer->InvalidatePhysicsRecursive( 8 );
}

bool C_AnimationSync::GetCachedMatrix( C_BasePlayer* pPlayer, matrix3x4_t* aMatrix )
{
	std::memcpy( aMatrix, m_CachedMatrix[ pPlayer->EntIndex( ) ].data( ), sizeof( matrix3x4_t ) * pPlayer->m_CachedBoneData( ).Count( ) );
	return true;
}

void C_AnimationSync::OnUpdateClientSideAnimation( C_BasePlayer* pPlayer )
{
	for ( int i = 0; i < MAXSTUDIOBONES; i++ )
		m_CachedMatrix[ pPlayer->EntIndex( ) ][ i ].SetOrigin( pPlayer->get_abs_origin( ) - m_BoneOrigins[ pPlayer->EntIndex( ) ][ i ] );

	std::memcpy( pPlayer->m_CachedBoneData( ).Base( ), m_CachedMatrix[ pPlayer->EntIndex( ) ].data( ), sizeof( matrix3x4_t ) * pPlayer->m_CachedBoneData( ).Count( ) );
	std::memcpy( pPlayer->GetBoneAccessor( ).GetBoneArrayForWrite( ), m_CachedMatrix[ pPlayer->EntIndex( ) ].data( ), sizeof( matrix3x4_t ) * pPlayer->m_CachedBoneData( ).Count( ) );
	
	return pPlayer->SetupBones_AttachmentHelper( );
}