#pragma once

#include "iglobalvars.h"
#include "clientclass.h"
#include "iappsystem.h"
#include "vfunction.h"

class IBaseClientDLL
{
public:
	virtual int              Connect(CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals) = 0;
	virtual int              Disconnect(void) = 0;
	virtual int              Init(CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals) = 0;
	virtual void             PostInit() = 0;
	virtual void             Shutdown(void) = 0;
	virtual void             LevelInitPreEntity(char const* pMapName) = 0;
	virtual void             LevelInitPostEntity() = 0;
	virtual void             LevelShutdown(void) = 0;
	virtual ClientClass*     GetAllClasses(void) = 0;
	bool DispatchUserMessage(int type, unsigned int a3, unsigned int length, const void *msg_data) {
		return CallVFunction<bool(__thiscall*)(bool, int, unsigned int, unsigned int, const void *)>(this, 38)(this, type, a3, length, msg_data);
	}
};