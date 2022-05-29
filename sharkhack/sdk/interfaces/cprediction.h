#pragma once

class C_BasePlayer;
class CMoveData;
class CMoveHelper;

class CPrediction {
public:
	get_virtual_fn( 3, Update( int iServerTick, bool bIsValid, int iAcknowledged, int iOutgoingCmd ), void( __thiscall* )( void*, int, bool, int, int ), iServerTick, bIsValid, iAcknowledged, iOutgoingCmd );
	get_virtual_fn( 20, SetupMove( C_BasePlayer* pPlayer, CUserCmd* pCmd, CMoveHelper* pMoveHelper, CMoveData* MoveData ), void( __thiscall* )( void*, void*, void*, void*, CMoveData* ), pPlayer, pCmd, pMoveHelper, MoveData );
	get_virtual_fn( 21, FinishMove( C_BasePlayer* pPlayer, CUserCmd* pCmd, CMoveData* MoveData ), void( __thiscall* )( void*, void*, void*, CMoveData* ), pPlayer, pCmd, MoveData );
	get_virtual_fn( 19, RunCommand( C_BasePlayer* pPlayer, CUserCmd* pCmd, CMoveHelper* pMoveHelper ),  void( __thiscall* )( void*, void*, void*, CMoveHelper* ), pPlayer, pCmd, pMoveHelper );
};
