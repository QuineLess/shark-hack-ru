#pragma once
#include "../../sdk/interfaces.h"
#include "../../utils/entity.h"

class C_BoneManager
{
public:
	virtual void BuildMatrix( C_BasePlayer* pPlayer, matrix3x4_t* aMatrix, bool bSafeMatrix );
};

inline C_BoneManager* g_BoneManager = new C_BoneManager( );