#pragma once

class IServerGameDLL
{
public:
	virtual bool			ReplayInit(CreateInterfaceFn fnReplayFactory) = 0;
	virtual bool			GameInit(void) = 0;
	virtual bool			LevelInit(char const *pMapName, char const *pMapEntities, char const *pOldLevel, char const *pLandmarkName, bool loadGame, bool background) = 0;
	virtual void			ServerActivate(void *pEdictList, int edictCount, int clientMax) = 0;
	virtual void			GameFrame(bool simulating) = 0;
	virtual void			PreClientUpdate(bool simulating) = 0;
	virtual void			LevelShutdown(void) = 0;
	virtual void			GameShutdown(void) = 0;
	virtual void			DLLShutdown(void) = 0;
	virtual float			GetTickInterval(void) const = 0;
	virtual ServerClass*	GetAllServerClasses(void) = 0;
	virtual const char     *GetGameDescription(void) = 0;
	virtual void			CreateNetworkStringTables(void) = 0;
};
