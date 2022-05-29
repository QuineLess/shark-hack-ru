#pragma once

#define k_EMsgGCCStrike15_v2_MatchmakingGC2ClientReserve 9107
#define k_EMsgGCClientWelcome 4004
#define k_EMsgGCClientHello 4006
#define k_EMsgGCAdjustItemEquippedState 1059
#define k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello 9109
#define k_EMsgGCCStrike15_v2_MatchmakingGC2ClientHello 9110
#define k_EMsgGCCStrike15_v2_ClientGCRankUpdate  9194

// ProfileChanger

struct MatchmakingGC2ClientHello {
	static const uint32_t commendation = 8;
	static const uint32_t ranking = 7;
	static const uint32_t player_level = 17;
	static const uint32_t player_xp = 18;
	static const uint32_t penalty_seconds = 4;
	static const uint32_t penalty_reason = 5;
};

struct CCSUsrMsg_SayText2 {
	static const uint32_t ent_idx = 1;
	static const uint32_t chat = 2;
	static const uint32_t msg_name = 3;
	static const uint32_t params = 4;
	static const uint32_t textallchat = 5;
};

struct ClientGCRankUpdate {
	static const uint32_t ranking = 1;
};

struct PlayerCommendationInfo {
	static const uint32_t cmd_friendly = 1;
	static const uint32_t cmd_teaching = 2;
	static const uint32_t cmd_leader = 4;
};
struct PlayerRankingInfo {
	static const uint32_t rank_id = 2;
	static const uint32_t wins = 3;
	static const uint32_t rank_type_id = 6;
};

