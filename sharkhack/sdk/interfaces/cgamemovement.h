#pragma once

class C_BasePlayer;
class CMoveData
{
public:
    bool        m_bFirstRunOfFunctions : 1;
    bool        m_bGameCodeMovedPlayer : 1;
    int         m_nPlayerHandle;
    int         m_nImpulseCommand;
    Vector      m_vecViewAngles;
    Vector      m_vecAbsViewAngles;
    int         m_nButtons;
    int         m_nOldButtons;
    float       m_flForwardMove;
    float       m_flSideMove;
    float       m_flUpMove;
    float       m_flMaxSpeed;
    float       m_flClientMaxSpeed;
    Vector      m_vecVelocity;
    Vector      m_vecAngles;
    Vector      m_vecOldAngles;
    float       m_flOutStepHeight;
    Vector      m_vecOutWishVel;
    Vector      m_vecOutJumpVel;
    Vector      m_vecConstraintCenter;
    float       m_flConstraintRadius;
    float       m_flConstraintWidth;
    float       m_flConstraintSpeedFactor;
    float       m_aUnknown[ 5 ];
    Vector      m_vecAbsOrigin;
};

class CGameMovement {
public:
    get_virtual_fn( 1, ProcessMovement( C_BasePlayer* pPlayer, CMoveData* pMoveData ), void( __thiscall* )( void*, C_BasePlayer*, CMoveData* ), pPlayer, pMoveData );
    get_virtual_fn( 2, Reset( ), void( __thiscall* )( void* ) );
    get_virtual_fn( 3, StartTrackPredictionErrors( C_BasePlayer* pPlayer ), void( __thiscall* )( void*, C_BasePlayer* ), pPlayer );
    get_virtual_fn( 4, FinishTrackPredictionErrors( C_BasePlayer* pPlayer ), void( __thiscall* )( void*, C_BasePlayer* ), pPlayer );

};
