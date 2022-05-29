#pragma once
#include "iclientnetworkable.h"
#include "iclientrenderable.h"
#include "iclientunknown.h"
#include "iclientthinkable.h"

struct SpatializationInfo_t;

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual void Release(void) = 0;
};