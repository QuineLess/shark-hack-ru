#include "lobby.h"

void lobby::stop_matchmaking() {
	utils::run_script(XORSTR("LobbyAPI.StopMatchmaking();"));
}

void lobby::on_init() {
	utils::run_script("function shark_call(times, callback) { for (var i = 1; i <= times; i++) { callback(); } }");

	if (config.lobby_popups_spam) {
		utils::run_script(XORSTR("UiToolkitAPI.CloseAllVisiblePopups();"));
		utils::run_script("shark_call(20, ()=>{ PartyListAPI.SessionCommand(\"Game::HostEndGamePlayAgain\", `run all xuid ${MyPersonaAPI.GetXuid()}`); }); ");
	}

	if (config.lobby_error_spam) {
		std::vector<std::string> errors = { "X_VacBanned", "X_NotVacVerified", "X_PenaltySeconds", "X_InsecureBlocked", "SkillGroupLargeDelta" };
		utils::run_script(std::string("shark_call(1, ()=> {PartyListAPI.SessionCommand(\"Game::ChatReportError\", `run all xuid ${ MyPersonaAPI.GetXuid() } error #SFUI_QMM_ERROR_" + errors[rand() % errors.size()] + "`);});").c_str());
	}

	if (config.lobby_invitations)
		utils::run_script("shark_call(1, ()=>{ PartyBrowserAPI.Refresh(); var lobbies_count = PartyBrowserAPI.GetResultsCount(); for (var lobby_id = 0; lobby_id < lobbies_count; lobby_id++) { var lobby_xuid = PartyBrowserAPI.GetXuidByIndex(lobby_id); var lobby_members_count = PartyBrowserAPI.GetPartyMembersCount(lobby_xuid); for (var lobby_member_id = 0; lobby_member_id < lobby_members_count; lobby_member_id++) { var lobby_member_xuid = PartyBrowserAPI.GetPartyMemberXuid(lobby_xuid, lobby_member_id); FriendsListAPI.ActionInviteFriend(lobby_member_xuid, ''); } } });");
}
