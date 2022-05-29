#pragma once
#include "../interfaces.h"

class IMatchEventsSubscription;

class IPlayerLocal
{
public:
	int GetXUIDLow()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + 0x8);
	}

	int GetXUIDHigh()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + 0xC);
	}

	const char* GetName()
	{
		typedef const char* (__thiscall* sss)(void*);
		return CallVFunction<sss>(this, 2)(this);
	}
};

class IPlayerManager
{
public:
	IPlayerLocal* GetLocalPlayer(int un) {
		typedef IPlayerLocal* (__thiscall* tTo)(void*, int);
		return CallVFunction<tTo>(this, 1)(this, un);
	}
};

class IMatchSystem
{
public:
	IPlayerManager* GetPlayerManager()
	{
		typedef IPlayerManager* (__thiscall* tKapis)(void*);
		return CallVFunction<tKapis>(this, 0)(this);
	}
};

class IMatchSession
{
public:
	virtual KeyValues* get_session_system_data() = 0; // 0
	virtual KeyValues* get_session_settings() = 0; // 1
	virtual void update_session_settings(KeyValues* settings) = 0; // 2
	virtual void command(KeyValues* command) = 0; // 3
};

class IMatchSessionOnlineHost
{
public:
	KeyValues* GetSessionSettings()
	{
		typedef KeyValues* (__thiscall* tGetSessionSettings)(void*);
		return CallVFunction<tGetSessionSettings>(this, 1)(this);
	}
	void UpdateSessionSettings(KeyValues* packet)
	{
		typedef void(__thiscall* tUpdateSessionSettings)(void*, KeyValues*);
		CallVFunction<tUpdateSessionSettings>(this, 2)(this, packet);
	}

	void Command(KeyValues* cmd)
	{
		typedef void(__thiscall* OriginalFn)(void*, KeyValues*);

		CallVFunction<OriginalFn>(this, 3)(this, cmd);
	}
};

class IMatchFramework : public IMatchSession
{
public:
	IMatchEventsSubscription* GetEventsSubscription()
	{
		typedef IMatchEventsSubscription* (__thiscall* tGetEventsSubscription)(void*);
		return CallVFunction<tGetEventsSubscription>(this, 11)(this);
	}

	IMatchSessionOnlineHost* GetMatchSession()
	{
		typedef IMatchSessionOnlineHost* (__thiscall* tGetMatchSession)(void*);
		return CallVFunction<tGetMatchSession>(this, 13)(this);
	}

	IMatchSystem* GetMatchSystem()
	{
		typedef IMatchSystem* (__thiscall* tGetMSys)(void*);
		return CallVFunction<tGetMSys>(this, 15)(this);
	}
};
