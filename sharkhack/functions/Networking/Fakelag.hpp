#pragma once
#include "../../sdk/interfaces.h"
#include "../../utils/entity.h"

class C_FakeLag
{
public:
	virtual void Instance( );
	virtual void ForceTicksAllowedForProcessing( );
	virtual void ResetData( );
private:
	int32_t m_iForcedChoke = 0;
	
	bool m_bDidForceTicksAllowed = false;
};

inline C_FakeLag* g_FakeLag = new C_FakeLag( );