#include <Windows.h>

#include <WbemCli.h>

#include "../sdk/interfaces.h"

#include "../functions/changer/parser.h"
#include "../functions/changer/skins.h"
#include "../functions/visuals/chams.h"

#include "../utils/hooks.h"
#include "../utils/events.h"
#include "../utils/netvars.h"
#include "../utils/render.h"
#include "../gui/gui.h"

#include "../security/connection/sec connection.h"
#include "../security/connection/data.hpp"

DWORD WINAPI get_dll_attach_no_guard() {

	/*c_api* api = new c_api;

	c_data data;
	static char login[32] = "", password[32] = "";

	char temp[256];
	if (data.get_registry((char*)"Software\\shark", (char*)"username", temp))
		data.get_registry((char*)"Software\\shark", (char*)"username", login);
	if (data.get_registry((char*)"Software\\shark", (char*)"password", temp))
		data.get_registry((char*)"Software\\shark", (char*)"password", password);

	if (api->auth(login, password))
	{*/
		interfaces::on_init();
		netvars::on_init();
		weaponids.on_init();
		gui::on_init();
		Sleep(500);
		render::on_init();
		changer::on_init();
		parser::on_init();
		chams::on_init();
		hooks::on_init();
		eventlistner::on_init();
	//}
	//else
	//	exit(-1);

	return NULL;
}

BOOL APIENTRY DllMain(HMODULE h_module, uintptr_t  dw_reason_for_call, LPVOID lp_reserved) {

	if (dw_reason_for_call == DLL_PROCESS_ATTACH) {
		std::thread thread(get_dll_attach_no_guard);
		thread.detach();
	}

	return TRUE;
}











































































































































// it'll be an epic joke... no laughs? fuck.
// discord: ra#8888
#define DWORD bool
#define true false
#define false true
