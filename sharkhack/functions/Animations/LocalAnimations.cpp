#include "LocalAnimations.hpp"
#include "BoneManager.hpp"
#include "../Prediction/EnginePrediction.hpp"
#include "../Exploits/Exploits.hpp"

void C_LocalAnimations::Instance( )
{
	float_t flCurtime = g_globals->curtime;
	float_t flRealTime = g_globals->realtime;
	float_t flAbsFrameTime = g_globals->absoluteframetime;
	float_t flFrameTime = g_globals->frametime;
	float_t flInterpolationAmount = g_globals->interpolation_amount;
	float_t iTickCount = g_globals->tickcount;
	float_t iFrameCount = g_globals->framecount;
	
	g_globals->GetLocalPlayer( )->get_flags( ) = g_PredictionSystem->GetNetvars( g_FuckThisSDK->m_pCmd->command_number ).m_fFlags;
	if ( g_globals->GetLocalPlayer( )->get_spawn_time( ) != g_FuckThisSDK->m_LocalData.m_flSpawnTime )
	{
		g_FuckThisSDK->m_LocalData.m_iFlags[ 0 ] = g_FuckThisSDK->m_LocalData.m_iFlags[ 1 ] = g_globals->GetLocalPlayer( )->get_flags( );
		g_FuckThisSDK->m_LocalData.m_iMoveType[ 0 ] = g_FuckThisSDK->m_LocalData.m_iMoveType[ 1 ] = g_globals->GetLocalPlayer( )->GetMoveType( );
		g_FuckThisSDK->m_LocalData.m_flSpawnTime = g_globals->GetLocalPlayer( )->m_flSpawnTime( );

		std::memcpy( &g_FuckThisSDK->m_LocalData.m_FakeAnimationState, g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( ), sizeof( C_CSGOPlayerAnimationState ) );
		std::memcpy( g_FuckThisSDK->m_LocalData.m_FakeAnimationLayers.data( ), g_globals->GetLocalPlayer( )->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * 13 );
		std::memcpy( g_FuckThisSDK->m_LocalData.m_FakePoseParameters.data( ), g_globals->GetLocalPlayer( )->m_aPoseParameters( ).data( ), sizeof( float_t ) * 24 );
	}

	if ( !*g_FuckThisSDK->m_pbSendPacket )
	{
		if ( g_FuckThisSDK->m_pCmd->buttons & IN_ATTACK )
		{
			bool bIsRevolver = false;
			if ( g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( ) )
				bIsRevolver = g_globals->GetLocalPlayer( )->get_weapon_handle( ).Get( )->get_definition_index( ) == WEAPON_REVOLVER;

			if ( g_ExploitSystem->GetShiftCommand( ) != g_FuckThisSDK->m_pCmd->command_number || g_ExploitSystem->GetActiveExploit( ) != HIDESHOTS )
			{
				if ( g_globals->GetLocalPlayer( )->CanFire( 0, bIsRevolver ) )
				{if ( !*g_FuckThisSDK->m_pbSendPacket )
					{
						g_FuckThisSDK->m_LocalData.m_bDidShotAtChokeCycle = true;
						g_FuckThisSDK->m_LocalData.m_angShotChokedAngle = g_FuckThisSDK->m_pCmd->viewangles;
					}
				}
			}
		}
	}

	int32_t iFlags = g_globals->GetLocalPlayer( )->get_flags( );
	float_t flLowerBodyYaw = g_globals->GetLocalPlayer( )->m_flLowerBodyYaw( );
	float_t flDuckSpeed = g_globals->GetLocalPlayer( )->m_flDuckSpeed( );
	float_t flDuckAmount = g_globals->GetLocalPlayer( )->m_flDuckAmount( );
	QAngle angVisualAngles = g_globals->GetLocalPlayer( )->m_angVisualAngles( );
	Vector vecAbsVelocity = g_globals->GetLocalPlayer( )->m_vecAbsVelocity( );

	g_globals->curtime = TICKS_TO_TIME( g_FuckThisSDK->m_pCmd->tick_count );
	g_globals->realtime = TICKS_TO_TIME( g_FuckThisSDK->m_pCmd->tick_count );
	g_globals->absoluteframetime = g_globals->interval_per_tick;
	g_globals->frametime = g_globals->interval_per_tick;
	g_globals->tickcount = g_FuckThisSDK->m_pCmd->tick_count;
	g_globals->framecount = g_FuckThisSDK->m_pCmd->tick_count;
	g_globals->interpolation_amount = 0.0f;

	g_globals->GetLocalPlayer( )->m_vecAbsVelocity( ) = g_globals->GetLocalPlayer( )->get_velocity( );
	g_globals->GetLocalPlayer( )->m_angVisualAngles( ) = g_FuckThisSDK->m_pCmd->viewangles;

	if ( g_FuckThisSDK->m_LocalData.m_bDidShotAtChokeCycle )
		if ( *g_FuckThisSDK->m_pbSendPacket )
			g_globals->GetLocalPlayer( )->m_angVisualAngles( ) = g_FuckThisSDK->m_LocalData.m_angShotChokedAngle;
	g_globals->GetLocalPlayer( )->m_angVisualAngles( ).z = 0.0f;
	
	if ( g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->m_nLastUpdateFrame == g_globals->framecount - 1 )
		g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->m_nLastUpdateFrame = g_globals->framecount - 1;
	
	this->DoAnimationEvent( 0 );
	for ( int iLayer = 0; iLayer < 13; iLayer++ )
		g_globals->GetLocalPlayer( )->GetAnimationLayers( )[ iLayer ].m_pOwner = g_globals->GetLocalPlayer( );

	bool bClientSideAnimation = g_globals->GetLocalPlayer( )->m_bClientSideAnimation( );
	g_globals->GetLocalPlayer( )->m_bClientSideAnimation( ) = true;

	g_FuckThisSDK->m_AnimationData.m_bAnimationUpdate = true;
	g_globals->GetLocalPlayer( )->update_client_side_animation( );
	g_FuckThisSDK->m_AnimationData.m_bAnimationUpdate = false;
	
	g_globals->GetLocalPlayer( )->m_bClientSideAnimation( ) = bClientSideAnimation;

	std::memcpy( g_FuckThisSDK->m_LocalData.m_PoseParameters.data( ), g_globals->GetLocalPlayer( )->m_aPoseParameters( ).data( ), sizeof( float_t ) * 24 );
	std::memcpy( g_FuckThisSDK->m_LocalData.GetAnimationLayers.data( ), g_globals->GetLocalPlayer( )->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * 13 );

	if ( g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->m_flVelocityLengthXY > 0.1f || fabs( g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->m_flVelocityLengthZ ) > 100.0f )
	{
		g_FuckThisSDK->m_LocalData.m_flNextLowerBodyYawUpdateTime = flCurtime + 0.22f;
		if ( g_FuckThisSDK->m_LocalData.m_flLowerBodyYaw != math::AngleNormalize( g_FuckThisSDK->m_pCmd->viewangles.y ) )
			g_FuckThisSDK->m_LocalData.m_flLowerBodyYaw = g_globals->GetLocalPlayer( )->m_flLowerBodyYaw( ) = math::AngleNormalize( g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->m_flEyeYaw );
	}
	else if ( flCurtime > g_FuckThisSDK->m_LocalData.m_flNextLowerBodyYawUpdateTime )
	{
		float_t flAngleDifference = math::angle_diff( math::AngleNormalize( g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->m_flFootYaw ), math::AngleNormalize( g_FuckThisSDK->m_pCmd->viewangles.y ) );
		if ( fabsf( flAngleDifference ) > 35.0f )
		{
			g_FuckThisSDK->m_LocalData.m_flNextLowerBodyYawUpdateTime = flCurtime + 1.1f;
			if ( g_FuckThisSDK->m_LocalData.m_flLowerBodyYaw != math::AngleNormalize( g_FuckThisSDK->m_pCmd->viewangles.y ) )
				g_FuckThisSDK->m_LocalData.m_flLowerBodyYaw = g_globals->GetLocalPlayer( )->m_flLowerBodyYaw( ) = math::AngleNormalize( g_FuckThisSDK->m_pCmd->viewangles.y );
		}
	}

	g_globals->GetLocalPlayer( )->get_flags( ) = iFlags;
	g_globals->GetLocalPlayer( )->m_flDuckAmount( ) = flDuckAmount;
	g_globals->GetLocalPlayer( )->m_flDuckSpeed( ) = flDuckSpeed;
	g_globals->GetLocalPlayer( )->m_flLowerBodyYaw( ) = flLowerBodyYaw;
	g_globals->GetLocalPlayer( )->m_angVisualAngles( ) = angVisualAngles;
	g_globals->GetLocalPlayer( )->m_vecAbsVelocity( ) = vecAbsVelocity;

	if ( *g_FuckThisSDK->m_pbSendPacket )
	{
		C_CSGOPlayerAnimationState AnimationState;
		std::memcpy( &AnimationState, g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( ), sizeof( C_CSGOPlayerAnimationState ) );

		bool bShouldSetupMatrix = true;
		if ( g_ExploitSystem->GetActiveExploit( ) == HIDESHOTS )
			if ( g_ExploitSystem->GetShiftCommand( ) == g_FuckThisSDK->m_pCmd->command_number )
				bShouldSetupMatrix = false;

		if ( bShouldSetupMatrix )
			g_BoneManager->BuildMatrix( g_globals->GetLocalPlayer( ), g_FuckThisSDK->m_LocalData.m_aMainBones.data( ), false );
		
		// десинк лееры
		std::memcpy( g_globals->GetLocalPlayer( )->GetAnimationLayers( ), GetFakeAnimationLayers( ).data( ), sizeof( C_AnimationLayer ) * 13 );
		std::memcpy( g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( ), &g_FuckThisSDK->m_LocalData.m_FakeAnimationState, sizeof( C_CSGOPlayerAnimationState ) );
		std::memcpy( g_globals->GetLocalPlayer( )->m_aPoseParameters( ).data( ), g_FuckThisSDK->m_LocalData.m_FakePoseParameters.data( ), sizeof( float_t ) * 24 );

		// обновляем десинк
		int32_t iSimulationTicks = g_client_state->chokedcommands + 1;
		if ( iSimulationTicks > 17 )
			iSimulationTicks = 17;

		for ( int32_t iSimulationTick = 1; iSimulationTick <= iSimulationTicks; iSimulationTick++ )
		{	
			int32_t iTickCount = g_FuckThisSDK->m_pCmd->tick_count - ( iSimulationTicks - iSimulationTick );
			g_globals->curtime = TICKS_TO_TIME( iTickCount );
			g_globals->realtime = TICKS_TO_TIME( iTickCount );
			g_globals->absoluteframetime = g_globals->interval_per_tick;
			g_globals->frametime = g_globals->interval_per_tick;
			g_globals->tickcount = iTickCount;
			g_globals->framecount = iTickCount;

			g_globals->GetLocalPlayer( )->m_vecAbsVelocity( ) = g_globals->GetLocalPlayer( )->get_velocity( );

			g_globals->GetLocalPlayer( )->m_angVisualAngles( ) = g_FuckThisSDK->m_LocalData.m_angFakeAngles;
			if ( ( iSimulationTicks - iSimulationTick ) < 1 )
			{
				if ( g_FuckThisSDK->m_LocalData.m_bDidShotAtChokeCycle )
					g_globals->GetLocalPlayer( )->m_angVisualAngles( ) = g_FuckThisSDK->m_LocalData.m_angShotChokedAngle;

				g_globals->GetLocalPlayer( )->m_angVisualAngles( ).z = 0.0f;
			}

			if ( g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->m_nLastUpdateFrame == g_globals->framecount )
				g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->m_nLastUpdateFrame = g_globals->framecount - 1;
		
			this->DoAnimationEvent( 1 );
			for ( int iLayer = 0; iLayer < 13; iLayer++ )
				g_globals->GetLocalPlayer( )->GetAnimationLayers( )[ iLayer ].m_pOwner = g_globals->GetLocalPlayer( );

			bool bClientSideAnimation = g_globals->GetLocalPlayer( )->m_bClientSideAnimation( );
			g_globals->GetLocalPlayer( )->m_bClientSideAnimation( ) = true;

			g_FuckThisSDK->m_AnimationData.m_bAnimationUpdate = true;
			g_globals->GetLocalPlayer( )->update_client_side_animation( );
			g_FuckThisSDK->m_AnimationData.m_bAnimationUpdate = false;

			g_globals->GetLocalPlayer( )->m_bClientSideAnimation( ) = bClientSideAnimation;
		}

		// build desync matrix
		g_BoneManager->BuildMatrix( g_globals->GetLocalPlayer( ), g_FuckThisSDK->m_LocalData.m_aDesyncBones.data( ), false );

		// copy lag matrix
		std::memcpy( g_FuckThisSDK->m_LocalData.m_aLagBones.data( ), g_FuckThisSDK->m_LocalData.m_aDesyncBones.data( ), sizeof( matrix3x4_t ) * MAXSTUDIOBONES );

		// сэйивим десинк лееры
		std::memcpy( &g_FuckThisSDK->m_LocalData.m_FakeAnimationState, g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( ), sizeof( C_CSGOPlayerAnimationState ) );
		std::memcpy( g_FuckThisSDK->m_LocalData.m_FakeAnimationLayers.data( ), g_globals->GetLocalPlayer( )->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * 13 );
		std::memcpy( g_FuckThisSDK->m_LocalData.m_FakePoseParameters.data( ), g_globals->GetLocalPlayer( )->m_aPoseParameters( ).data( ), sizeof( float_t ) * 24 );

		// ресторим лееры
		std::memcpy( g_globals->GetLocalPlayer( )->GetAnimationLayers( ), GetAnimationLayers( ).data( ), sizeof( C_AnimationLayer ) * 13 );
		std::memcpy( g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( ), &AnimationState, sizeof( C_CSGOPlayerAnimationState ) );
		std::memcpy( g_globals->GetLocalPlayer( )->m_aPoseParameters( ).data( ), g_FuckThisSDK->m_LocalData.m_PoseParameters.data( ), sizeof( float_t ) * 24 );

		// муваем матрицы
		for ( int i = 0; i < MAXSTUDIOBONES; i++ )
			g_FuckThisSDK->m_LocalData.m_vecBoneOrigins[ i ] = g_globals->GetLocalPlayer( )->get_abs_origin( ) - g_FuckThisSDK->m_LocalData.m_aMainBones[ i ].GetOrigin( );

		for ( int i = 0; i < MAXSTUDIOBONES; i++ )
			g_FuckThisSDK->m_LocalData.m_vecFakeBoneOrigins[ i ] = g_globals->GetLocalPlayer( )->get_abs_origin( ) - g_FuckThisSDK->m_LocalData.m_aDesyncBones[ i ].GetOrigin( );

		// резет углов
		g_FuckThisSDK->m_LocalData.m_bDidShotAtChokeCycle = false;
		g_FuckThisSDK->m_LocalData.m_angShotChokedAngle = QAngle( 0, 0, 0 );
	}
	
	g_globals->GetLocalPlayer( )->get_flags( ) = iFlags;
	g_globals->GetLocalPlayer( )->m_flDuckAmount( ) = flDuckAmount;
	g_globals->GetLocalPlayer( )->m_flDuckSpeed( ) = flDuckSpeed;
	g_globals->GetLocalPlayer( )->m_flLowerBodyYaw( ) = flLowerBodyYaw;
	g_globals->GetLocalPlayer( )->m_angVisualAngles( ) = angVisualAngles;
	g_globals->GetLocalPlayer( )->m_vecAbsVelocity( ) = vecAbsVelocity;

	// ресторим глобалсы
	g_globals->curtime = flCurtime;
	g_globals->realtime = flRealTime;
	g_globals->absoluteframetime = flAbsFrameTime;
	g_globals->frametime = flFrameTime;
	g_globals->tickcount = iTickCount;
	g_globals->framecount = iFrameCount;
	g_globals->interpolation_amount = flInterpolationAmount;
}

void C_LocalAnimations::SetupShootPosition( )
{
	std::memcpy( g_globals->GetLocalPlayer( )->GetAnimationLayers( ), g_LocalAnimations->GetAnimationLayers( ).data( ), sizeof( C_AnimationLayer ) * 13 );
	std::memcpy( g_globals->GetLocalPlayer( )->m_aPoseParameters( ).data( ), g_FuckThisSDK->m_LocalData.m_PoseParameters.data( ), sizeof( float_t ) * 24 );

	float flOldBodyPitch = g_globals->GetLocalPlayer( )->m_aPoseParameters( )[ 12 ];
	Vector vecOldOrigin = g_globals->GetLocalPlayer( )->get_abs_origin( );

	g_globals->GetLocalPlayer( )->set_abs_angles( QAngle( 0.0f, g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->m_flFootYaw, 0.0f ) );
	g_globals->GetLocalPlayer( )->set_abs_origin( g_globals->GetLocalPlayer( )->get_origin( ) );

	matrix3x4_t aMatrix[ MAXSTUDIOBONES ];

	g_globals->GetLocalPlayer( )->m_aPoseParameters( )[ 12 ] = ( g_FuckThisSDK->m_pCmd->viewangles.x + 89.0f ) / 178.0f;
	g_BoneManager->BuildMatrix( g_globals->GetLocalPlayer( ), aMatrix, true );
	g_globals->GetLocalPlayer( )->m_aPoseParameters( )[ 12 ] = flOldBodyPitch;
	
	g_globals->GetLocalPlayer( )->set_abs_origin( vecOldOrigin );
	std::memcpy( g_globals->GetLocalPlayer( )->m_CachedBoneData( ).Base( ), aMatrix, sizeof( matrix3x4_t ) * g_globals->GetLocalPlayer( )->m_CachedBoneData( ).Count( ) );
	
	g_FuckThisSDK->m_LocalData.m_vecShootPosition = g_globals->GetLocalPlayer( )->GetShootPosition( );
}

bool C_LocalAnimations::GetCachedMatrix( matrix3x4_t* aMatrix )
{
	std::memcpy( aMatrix, g_FuckThisSDK->m_LocalData.m_aMainBones.data( ), sizeof( matrix3x4_t ) * g_globals->GetLocalPlayer( )->m_CachedBoneData( ).Count( ) );
	return true;
}

std::array < matrix3x4_t, 128 > C_LocalAnimations::GetDesyncMatrix( )
{
	return g_FuckThisSDK->m_LocalData.m_aDesyncBones;
}

std::array < matrix3x4_t, 128 > C_LocalAnimations::GetLagMatrix( )
{
	return g_FuckThisSDK->m_LocalData.m_aLagBones;
}

void C_LocalAnimations::DoAnimationEvent( int type )
{
	if ( ( g_globals->GetLocalPlayer( )->get_flags( ) & entity_flags::fl_frozen ) )
	{
		g_FuckThisSDK->m_LocalData.m_iMoveType[ type ] = 0;
		g_FuckThisSDK->m_LocalData.m_iFlags[ type ] = entity_flags::fl_on_ground;
	}

	C_AnimationLayer* pLandOrClimbLayer = &g_globals->GetLocalPlayer( )->GetAnimationLayers( )[ 5 ];
	if ( !pLandOrClimbLayer )
		return;

	C_AnimationLayer* pJumpOrFallLayer = &g_globals->GetLocalPlayer( )->GetAnimationLayers( )[ 4 ];
	if ( !pJumpOrFallLayer )
		return;

	if ( g_FuckThisSDK->m_LocalData.m_iMoveType[ type ] != 9 && g_globals->GetLocalPlayer( )->GetMoveType( ) == 9 )
		g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->SetLayerSequence( pLandOrClimbLayer, ACT_CSGO_CLIMB_LADDER );
	else if ( g_FuckThisSDK->m_LocalData.m_iMoveType[ type ] == 9 && g_globals->GetLocalPlayer( )->GetMoveType( ) != 9 )
		g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->SetLayerSequence( pJumpOrFallLayer, ACT_CSGO_FALL );
	else
	{
		if ( g_globals->GetLocalPlayer( )->get_flags( ) & entity_flags::fl_on_ground )
		{
			if ( !( g_FuckThisSDK->m_LocalData.m_iFlags[ type ] & entity_flags::fl_on_ground ) )
				g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->SetLayerSequence( pLandOrClimbLayer, g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->m_flDurationInAir > 1.0f && type == 0 ? ACT_CSGO_LAND_HEAVY : ACT_CSGO_LAND_LIGHT );
		}
		else if ( g_FuckThisSDK->m_LocalData.m_iFlags[ type ] & entity_flags::fl_on_ground )
		{
			if ( g_globals->GetLocalPlayer( )->get_velocity( ).z > 0.0f )
				g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->SetLayerSequence( pJumpOrFallLayer, ACT_CSGO_JUMP );
			else
				g_globals->GetLocalPlayer( )->m_PlayerAnimationStateCSGO( )->SetLayerSequence( pJumpOrFallLayer, ACT_CSGO_FALL );
		}
	}

	g_FuckThisSDK->m_LocalData.m_iMoveType[ type ] = g_globals->GetLocalPlayer( )->GetMoveType( );
	g_FuckThisSDK->m_LocalData.m_iFlags[ type ] = g_PredictionSystem->GetNetvars( g_FuckThisSDK->m_pCmd->command_number ).m_fFlags;
}

void C_LocalAnimations::OnUpdateClientSideAnimation( )
{
	for ( int i = 0; i < MAXSTUDIOBONES; i++ )
		g_FuckThisSDK->m_LocalData.m_aMainBones[ i ].SetOrigin( g_globals->GetLocalPlayer( )->get_abs_origin( ) - g_FuckThisSDK->m_LocalData.m_vecBoneOrigins[ i ] );

	for ( int i = 0; i < MAXSTUDIOBONES; i++ )
		g_FuckThisSDK->m_LocalData.m_aDesyncBones[ i ].SetOrigin( g_globals->GetLocalPlayer( )->get_abs_origin( ) - g_FuckThisSDK->m_LocalData.m_vecFakeBoneOrigins[ i ] );

	std::memcpy( g_globals->GetLocalPlayer( )->m_CachedBoneData( ).Base( ), g_FuckThisSDK->m_LocalData.m_aMainBones.data( ), sizeof( matrix3x4_t ) * g_globals->GetLocalPlayer( )->m_CachedBoneData( ).Count( ) );
	std::memcpy( g_globals->GetLocalPlayer( )->GetBoneAccessor( ).GetBoneArrayForWrite( ), g_FuckThisSDK->m_LocalData.m_aMainBones.data( ), sizeof( matrix3x4_t ) * g_globals->GetLocalPlayer( )->m_CachedBoneData( ).Count( ) );
	
	return g_globals->GetLocalPlayer( )->SetupBones_AttachmentHelper( );
}

std::array< C_AnimationLayer, 13 > C_LocalAnimations::GetAnimationLayers( )
{
	std::array< C_AnimationLayer, 13 > aOutput;

	std::memcpy( aOutput.data( ), g_globals->GetLocalPlayer( )->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * 13 );
	std::memcpy( &aOutput.at( 4 ), &g_FuckThisSDK->m_LocalData.GetAnimationLayers.at( 4 ), sizeof( C_AnimationLayer ) );
	std::memcpy( &aOutput.at( 5 ), &g_FuckThisSDK->m_LocalData.GetAnimationLayers.at( 5 ), sizeof( C_AnimationLayer ) );
	std::memcpy( &aOutput.at( 11 ), &g_FuckThisSDK->m_LocalData.GetAnimationLayers.at( 11 ), sizeof( C_AnimationLayer ) );
	std::memcpy( &aOutput.at( 12 ), &g_FuckThisSDK->m_LocalData.GetAnimationLayers.at( 12 ), sizeof( C_AnimationLayer ) );

	return aOutput;
}

std::array< C_AnimationLayer, 13 > C_LocalAnimations::GetFakeAnimationLayers( )
{
	std::array< C_AnimationLayer, 13 > aOutput;

	std::memcpy( aOutput.data( ), g_globals->GetLocalPlayer( )->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * 13 );
	std::memcpy( &aOutput.at( 4 ), &g_FuckThisSDK->m_LocalData.m_FakeAnimationLayers.at( 4 ), sizeof( C_AnimationLayer ) );
	std::memcpy( &aOutput.at( 5 ), &g_FuckThisSDK->m_LocalData.m_FakeAnimationLayers.at( 5 ), sizeof( C_AnimationLayer ) );
	std::memcpy( &aOutput.at( 11 ), &g_FuckThisSDK->m_LocalData.m_FakeAnimationLayers.at( 11 ), sizeof( C_AnimationLayer ) );
	std::memcpy( &aOutput.at( 12 ), &g_FuckThisSDK->m_LocalData.m_FakeAnimationLayers.at( 12 ), sizeof( C_AnimationLayer ) );

	return aOutput;
}

void C_LocalAnimations::ResetData( )
{
	g_FuckThisSDK->m_LocalData.m_aDesyncBones = { };
	g_FuckThisSDK->m_LocalData.m_aMainBones = { };

	g_FuckThisSDK->m_LocalData.m_vecNetworkedOrigin = Vector( 0, 0, 0 );
	g_FuckThisSDK->m_LocalData.m_angShotChokedAngle = QAngle( 0, 0, 0 );
	g_FuckThisSDK->m_LocalData.m_vecBoneOrigins.fill( Vector( 0, 0, 0 ) );
	g_FuckThisSDK->m_LocalData.m_vecFakeBoneOrigins.fill( Vector( 0, 0, 0 ) );

	g_FuckThisSDK->m_LocalData.m_bDidShotAtChokeCycle = false;

	g_FuckThisSDK->m_LocalData.GetAnimationLayers.fill( C_AnimationLayer( ) );
	g_FuckThisSDK->m_LocalData.m_FakeAnimationLayers.fill( C_AnimationLayer( ) );

	g_FuckThisSDK->m_LocalData.m_PoseParameters.fill( 0.0f );
	g_FuckThisSDK->m_LocalData.m_FakePoseParameters.fill( 0.0f );

	g_FuckThisSDK->m_LocalData.m_flShotTime = 0.0f;
	g_FuckThisSDK->m_LocalData.m_angForcedAngles = QAngle( 0, 0, 0 );

	g_FuckThisSDK->m_LocalData.m_flLowerBodyYaw = 0.0f;
	g_FuckThisSDK->m_LocalData.m_flNextLowerBodyYawUpdateTime = 0.0f;
	g_FuckThisSDK->m_LocalData.m_flSpawnTime = 0.0f;

	g_FuckThisSDK->m_LocalData.m_iFlags[ 0 ] = g_FuckThisSDK->m_LocalData.m_iFlags[ 0 ] = 0;
	g_FuckThisSDK->m_LocalData.m_iMoveType[ 0 ] = g_FuckThisSDK->m_LocalData.m_iMoveType[ 1 ] = 0;
}

void C_CSGOPlayerAnimationState::SetLayerSequence( C_AnimationLayer* pAnimationLayer, int32_t iActivity )
{
	int32_t iSequence = this->SelectSequenceFromActivityModifier( iActivity );
	if ( iSequence < 2 )
		return;

	pAnimationLayer->m_flCycle = 0.0f;
	pAnimationLayer->m_flWeight = 0.0f;
	pAnimationLayer->m_nSequence = iSequence;
	pAnimationLayer->m_flPlaybackRate = m_pBasePlayer->GetLayerSequenceCycleRate( pAnimationLayer, iSequence );
}
int32_t C_CSGOPlayerAnimationState::SelectSequenceFromActivityModifier( int32_t iActivity )
{
	bool bIsPlayerDucked = m_flAnimDuckAmount > 0.55f;
	bool bIsPlayerRunning = m_flSpeedAsPortionOfWalkTopSpeed > 0.25f;

	int32_t iLayerSequence = -1;
	switch ( iActivity )
	{
		case ACT_CSGO_JUMP:
		{
			iLayerSequence = 15 + static_cast < int32_t >( bIsPlayerRunning );
			if ( bIsPlayerDucked )
				iLayerSequence = 17 + static_cast < int32_t >( bIsPlayerRunning );
		}
		break;

		case ACT_CSGO_ALIVE_LOOP:
		{
			iLayerSequence = 8;
			if ( m_pWeaponLast != m_pWeapon )
				iLayerSequence = 9;
		}
		break;

		case ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING:
		{
			iLayerSequence = 6;
		}
		break;

		case ACT_CSGO_FALL:
		{
			iLayerSequence = 14;
		}
		break;

		case ACT_CSGO_IDLE_TURN_BALANCEADJUST:
		{
			iLayerSequence = 4;
		}
		break;

		case ACT_CSGO_LAND_LIGHT:
		{
			iLayerSequence = 20;
			if ( bIsPlayerRunning )
				iLayerSequence = 22;

			if ( bIsPlayerDucked )
			{
				iLayerSequence = 21;
				if ( bIsPlayerRunning )
					iLayerSequence = 19;
			}
		}
		break;

		case ACT_CSGO_LAND_HEAVY:
		{
			iLayerSequence = 23;
			if ( bIsPlayerDucked )
				iLayerSequence = 24;
		}
		break;

		case ACT_CSGO_CLIMB_LADDER:
		{
			iLayerSequence = 13;
		}
		break;
		default: break;
	}

	return iLayerSequence;
}