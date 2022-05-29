#pragma once
#include "vfunction.h"

class IPanel
{
public:
	const char *GetName(unsigned int vguiPanel) {
		return CallVFunction<const char *(__thiscall*)(void*, unsigned int)>(this, 36)(this, vguiPanel);
	}

	const char *GetClassName(unsigned int vguiPanel) {
		return CallVFunction<const char *(__thiscall*)(void*, unsigned int)>(this, 37)(this, vguiPanel);
	}
};
