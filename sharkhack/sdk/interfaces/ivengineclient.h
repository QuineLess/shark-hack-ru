#pragma once
#include "vector.h"
#include "qangle.h"
#include "vmatrix.h"
#include "iachievementmgr.h"

typedef struct InputContextHandle_t__ *InputContextHandle_t;
struct client_textmessage_t;
struct model_t;
class SurfInfo;
class IMaterial;
class CSentence;
class CAudioSource;
struct AudioState_t;
class ISpatialQuery;
class IMaterialSystem;
class CPhysCollide;
class IAchievementMgr;
class INetChannelInfo;
class ISPSharedMemory;
class CGamestatsData;
class KeyValues;
class CSteamAPIContext;

struct vplane {
	Vector normal;
	Vector dist;
};

typedef vplane Frustum_t[0x6];

typedef void(*pfnDemoCustomDataCallback)(uint8_t *pData, size_t iSize);
typedef struct player_info_s
{
	__int64         unknown;            //0x0000 
	union
	{
		__int64       steamID64;          //0x0008 - SteamID64
		struct
		{
			__int32     xuid_low;
			__int32     xuid_high;
		};
	};
	char            szName[128];        //0x0010 - Player Name
	int             userId;             //0x0090 - Unique Server Identifier
	char            szSteamID[20];      //0x0094 - STEAM_X:Y:Z
	char            pad_0x00A8[0x10];   //0x00A8
	unsigned long   iSteamID;           //0x00B8 - SteamID 
	char            szFriendsName[128];
	bool            fakeplayer;
	bool            ishltv;
	unsigned int    customfiles[4];
	unsigned char   filesdownloaded;
} player_info_t;

class INetChannelInfo
{
public:

	enum {
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	virtual const char  *GetName(void) const = 0;	// get channel name
	virtual const char  *GetAddress(void) const = 0; // get channel IP address as string
	virtual float		GetTime(void) const = 0;	// current net time
	virtual float		GetTimeConnected(void) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize(void) const = 0;	// netchannel packet history size
	virtual int			GetDataRate(void) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback(void) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut(void) const = 0;	// true if timing out
	virtual bool		IsPlayback(void) const = 0;	// true if demo playback

	virtual float		GetLatency(int flow) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(int flow) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss(int flow) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke(int flow) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAvgData(int flow) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets(int flow) const = 0; // avg packets/sec
	virtual int			GetTotalData(int flow) const = 0;	 // total flow in/out in bytes
	virtual int			GetSequenceNr(int flow) const = 0;	// last send seq number
	virtual bool		IsValidPacket(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(int flow, int frame_number) const = 0; // time when packet was send
	virtual int			GetPacketBytes(int flow, int frame_number, int group) const = 0; // group size of this packet
	virtual bool		GetStreamProgress(int flow, int *received, int *total) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived(void) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void		GetPacketResponseLatency(int flow, int frame_number, int *pnLatencyMsecs, int *pnChoke) const = 0;
	virtual void		GetRemoteFramerate(float *pflFrameTime, float *pflFrameTimeStdDeviation) const = 0;

	virtual float		GetTimeoutSeconds() const = 0;
};

class IVEngineClient
{
public:
	void ClientCmd(const char *cmd_string) {
		CallVFunction<void(__thiscall*)(void*, const char *)>(this, 7)(this, cmd_string);
	}

	bool GetPlayerInfo(int index, const player_info_t& player_info) {
		return CallVFunction<bool(__thiscall*)(void*, int, const player_info_t&)>(this, 8)(this, index, player_info);
	}

	int GetPlayerForUserID(int user_id) {
		return CallVFunction<int(__thiscall*)(void*, int)>(this, 9)(this, user_id);
	}

	bool Con_IsVisible() {
		return CallVFunction<bool(__thiscall*)(void*)>(this, 11)(this);
	}

	int GetLocalPlayer() {
		return CallVFunction<int(__thiscall*)(void*)>(this, 12)(this);
	}

	void GetViewAngles(Vector* angles)  {
		CallVFunction<void(__thiscall*)(void*, Vector*)>(this, 18)(this, angles);
	}

	void SetViewAngles(const Vector& angles) {
		CallVFunction<void(__thiscall*)(void*, const Vector&)>(this, 19)(this, angles);
	}

	int GetMaxClients() {
		return CallVFunction<int(__thiscall*)(void*)>(this, 20)(this);
	}

	bool IsInGame() {
		return CallVFunction<bool(__thiscall*)(void*)>(this, 26)(this);
	}

	bool IsConnected() {
		return CallVFunction<bool(__thiscall*)(void*)>(this, 27)(this);
	}

	const VMatrix& WorldToScreenMatrix() {
		return CallVFunction<const VMatrix&(__thiscall*)(void*)>(this, 37)(this);
	}

	INetChannelInfo* GetNetChannel() {
		return CallVFunction<INetChannelInfo*(__thiscall*)(void*)>(this, 78)(this);
	}

	void ClientCmdUnrestricted(const char* cmd) {
		CallVFunction<void(__thiscall*)(void*, const char*, bool)>(this, 114)(this, cmd, false);
	}

	IAchievementMgr* GetAchievementMgr() {
		return CallVFunction<IAchievementMgr*(__thiscall*)(void*)>(this, 121)(this);
	}

	get_virtual_fn(43, get_bsp_tree_query(), void* (__thiscall*)(void*));
};