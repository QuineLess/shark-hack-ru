#pragma once

enum EGCResult {
	k_EGCResultOK = 0,
	k_EGCResultNoMessage = 1,           // There is no message in the queue
	k_EGCResultBufferTooSmall = 2,      // The buffer is too small for the requested message
	k_EGCResultNotLoggedOn = 3,         // The client is not logged onto Steam
	k_EGCResultInvalidMessage = 4,      // Something was wrong with the message being sent with SendMessage
};

class ISteamClient
{
public:
	void* GetISteamGenericInterface(void* hSteamUser, void* hSteamPipe, const char* pchVersion) {
		return CallVFunction<void* (__thiscall*)(void*, void*, void*, const char*)>(this, 12)(this, hSteamUser, hSteamPipe, pchVersion);
	}
};

class ISteamGameCoordinator
{
public:
	EGCResult GCSendMessage(int unMsgType, const void* pubData, int cubData) {
		return CallVFunction<EGCResult(__thiscall*)(void*, int, const void*, int)>(this, 0)(this, unMsgType, pubData, cubData);
	}

	EGCResult GCRetrieveMessage(int* punMsgType, void* pubDest, int cubDest, int* pcubMsgSize) {
		return CallVFunction<EGCResult(__thiscall*)(void*, int*, void*, int, int*)>(this, 2)(this, punMsgType, pubDest, cubDest, pcubMsgSize);
	}
};