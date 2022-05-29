#pragma once

#include <cstdint>

// Created with ReClass.NET by KN4CK3R
#pragma pack(push, 1)
class INetChannel
{
public:
	char pad_0000[20];           //0x0000
	bool m_bProcessingMessages;  //0x0014
	bool m_bShouldDelete;        //0x0015
	char pad_0016[2];            //0x0016
	int32_t m_nOutSequenceNr;    //0x0018 last send outgoing sequence number
	int32_t m_nInSequenceNr;     //0x001C last received incoming sequnec number
	int32_t m_nOutSequenceNrAck; //0x0020 last received acknowledge outgoing sequnce number
	int32_t m_nOutReliableState; //0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int32_t m_nInReliableState;  //0x0028 state of incoming reliable data
	int32_t m_nChokedPackets;    //0x002C number of choked packets
	char pad_0030[1044];         //0x0030

	get_virtual_fn( 46, SendDatagram( void* Data), int( __thiscall* )( void*, void* ), Data );
}; //Size: 0x0444

class CClockDriftMgr
{
public:
	float m_ClockOffsets[16];   //0x0000
	uint32_t m_iCurClockOffset; //0x0044
	uint32_t m_nServerTick;     //0x0048
	uint32_t m_nClientTick;     //0x004C
}; //Size: 0x0050

// This struct is most likely wrong
// Only fields that I know to be valid are:
// - m_NetChannel
// - m_nCurrentSequence
// - m_nDeltaTick
// - m_nMaxClients
// - viewangles
class CClientState {
public:
	char pad_0000[ 156 ];
	INetChannel* m_NetChannel;
	int m_nChallengeNr;
	char pad_00A4[ 100 ];
	int m_nSignonState;
	int signon_pads[ 2 ];
	float m_flNextCmdTime;
	int m_nServerCount;
	int m_nCurrentSequence;
	int musor_pads[ 2 ];
	CClockDriftMgr m_ClockDriftMgr;
	int m_nDeltaTick;
	bool m_bPaused;
	char paused_align[ 3 ];
	int m_nViewEntity;
	int m_nPlayerSlot;
	int bruh;
	char m_szLevelName[ 260 ];
	char m_szLevelNameShort[ 80 ];
	char m_szGroupName[ 80 ];
	char pad_032[ 92 ];
	int m_nMaxClients;
	char pad_0314[ 18828 ];
	float m_nLastServerTickTime;
	bool m_bInSimulation;
	char pad_4C9D[ 3 ];
	int m_nOldTickCount;
	float m_flTickReminder;
	float m_flFrametime;
	int m_nLastOutgoingCommand;
	int chokedcommands;
	int m_nLastCommandAck;
	int m_nPacketEndTickUpdate;
	int m_nCommandAck;
	int m_nSoundSequence;
	char pad_4CCD[ 76 ];
	QAngle viewangles;
	int pads[ 54 ];
	void* m_pEvents;

	void SetReservationCookie(unsigned long long cookie) {
		 CallVFunction<void(__thiscall*)(void*, unsigned long long)>(this, 63)(this, cookie);
	}

}; //Size: 0x4D1C 

#pragma pack(pop) 

class INetMessage {
public:
	virtual ~INetMessage() {};
	virtual void set_net_channel(INetChannel* netchan) = 0;
	virtual void set_reliable(bool state) = 0;
	virtual bool process(void) = 0;
	virtual bool read_from_buffer(bf_read& buffer) = 0;
	virtual bool write_to_buffer(bf_write& buffer) = 0;
	virtual bool is_reliable(void) const = 0;
	virtual int get_type(void) const = 0;
	virtual int get_group(void) const = 0;
	virtual const char* get_name(void) const = 0;
	virtual INetChannel* get_net_channel(void) const = 0;
};