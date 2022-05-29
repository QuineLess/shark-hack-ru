#pragma once

class CUIEngineSource2
{
public:
	bool BroadcastEvent(uintptr_t pEvent)
	{
		typedef bool(__thiscall* tOriginal)(void*, uintptr_t);
		return CallVFunction<tOriginal>(this, 54)(this, pEvent);
	}

	bool DispatchEvent(uintptr_t pEvent)
	{
		typedef bool(__thiscall* tOriginal)(void*, uintptr_t);
		return CallVFunction<tOriginal>(this, 52)(this, pEvent);
	}
};