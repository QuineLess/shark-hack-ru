#include "interfaces.h"

class InterfaceReg {
private:
	using InstantiateInterfaceFn = void*(*)();
public:
	InstantiateInterfaceFn m_CreateFn;
	const char *m_pName;
	InterfaceReg *m_pNext;
};

template<typename T>
static T *get_interface(const char *module, const char *name) {
	static unsigned int CreateInterfaceFn = reinterpret_cast<unsigned int>(GetProcAddress(GetModuleHandleA(module), XORSTR("CreateInterface")));

	if (!CreateInterfaceFn)
		return nullptr;

	T *inter = nullptr;

	int len = strlen(name);

	unsigned int target = (unsigned int)(CreateInterfaceFn)+*(unsigned int*)((unsigned int)(CreateInterfaceFn)+0x5) + 0x9;
	InterfaceReg *interface_reg_list = **reinterpret_cast<InterfaceReg***>(target + 0x6);

	for (InterfaceReg *cur = interface_reg_list; cur; cur = cur->m_pNext) {
		if (!strncmp(cur->m_pName, name, len) && std::atoi(cur->m_pName + len) > 0)
			inter = reinterpret_cast<T*>(cur->m_CreateFn());
	}
	return inter;
}

void interfaces::on_init() {
	g_client = get_interface<IBaseClientDLL>(XORSTR("client.dll"), XORSTR("VClient"));
	g_panel = get_interface<IPanel>(XORSTR("vgui2.dll"), XORSTR("VGUI_Panel"));
	g_ent_list = get_interface<IClientEntityList>(XORSTR("client.dll"), XORSTR("VClientEntityList"));
	g_engine = get_interface<IVEngineClient>(XORSTR("engine.dll"), XORSTR("VEngineClient"));
	g_surface = get_interface<ISurface>(XORSTR("vguimatsurface.dll"), XORSTR("VGUI_Surface"));
	g_server = get_interface<IServerGameDLL>(XORSTR("server.dll"), XORSTR("ServerGameDLL"));
	g_sound = get_interface<IEngineSound>(XORSTR("engine.dll"), XORSTR("IEngineSoundClient"));
	g_spatial_partition = get_interface<uintptr_t>(XORSTR("engine.dll"), XORSTR("SpatialPartition"));
	g_mat_system = get_interface<IMaterialSystem>(XORSTR("materialsystem.dll"), XORSTR("VMaterialSystem"));
	g_cvar = get_interface<ICvar>(XORSTR("vstdlib.dll"), XORSTR("VEngineCvar"));
	g_localize = get_interface<ILocalize>(XORSTR("localize.dll"), XORSTR("Localize_"));
	g_mdl_info = get_interface<IVModelInfo>(XORSTR("engine.dll"), XORSTR("VModelInfoClient"));
	g_game_events = get_interface<IGameEventManager2>(XORSTR("engine.dll"), XORSTR("GAMEEVENTSMANAGER"));
	g_base_file_sys = get_interface<IBaseFileSystem>(XORSTR("filesystem_stdio.dll"), XORSTR("VBaseFileSystem"));
	g_studio_render = get_interface<IStudioRender>(XORSTR("studiorender.dll"), XORSTR("VStudioRender"));
	g_effects = get_interface<CEffects>(XORSTR("engine.dll"), XORSTR("VEngineEffects"));
	g_engine_trace = get_interface<IEngineTrace>(XORSTR("engine.dll"), XORSTR("EngineTraceClient"));
	g_game_movement = get_interface<CGameMovement>(XORSTR("client.dll"), XORSTR("GameMovement"));
	g_prediction = get_interface<CPrediction>(XORSTR("client.dll"), XORSTR("VClientPrediction"));
	g_physprops = get_interface<IPhysicsSurfaceProps>(XORSTR("vphysics.dll"), XORSTR("VPhysicsSurfaceProps"));
	g_file_sys = get_interface<IFileSystem>(XORSTR("filesystem_stdio.dll"), XORSTR("VFileSystem"));
	g_game_types = get_interface<IGameTypes>(XORSTR("matchmaking.dll"), XORSTR("VENGINE_GAMETYPES_VERSION"));
	g_match_framework = get_interface<IMatchFramework>(XORSTR("matchmaking.dll"), XORSTR("MATCHFRAMEWORK_"));
	g_debug_overlay = get_interface<IVDebugOverlay>(XORSTR("engine.dll"), XORSTR("VDebugOverlay"));
	g_input_sys = get_interface<IInputSystem>(XORSTR("inputsystem.dll"), XORSTR("InputSystemVersion"));
	g_model_render = get_interface<IVModelRender>(XORSTR("engine.dll"), XORSTR("VEngineModel"));
	g_render_view = get_interface<IRenderView>(XORSTR("engine.dll"), XORSTR("VEngineRenderView"));
	g_panorama_engine = get_interface<IPanoramaUIEngine>(XORSTR("panorama.dll"), XORSTR("PanoramaUIEngine"));
	g_input_system = get_interface<i_input_system>(XORSTR("inputsystem.dll"), XORSTR("InputSystemVersion"));

	g_globals = **reinterpret_cast<CGlobalVarsBase***>((*(DWORD**)g_client)[0] + 0x1F);
	g_client_mode = **reinterpret_cast<IClientMode***>((*(DWORD**)g_client)[10] + 0x5);

	g_device = **reinterpret_cast<IDirect3DDevice9***>(memory.g_device);
	g_player_inventory = **reinterpret_cast<CPlayerInventory***>(memory.g_player_inventory);
	g_inventory_manager = *reinterpret_cast<CSInventoryManager**>(memory.g_inventory_manager);
	g_move_data = **reinterpret_cast<CMoveData***>(memory.g_move_data);
	g_move_helper = **reinterpret_cast<CMoveHelper***>(memory.g_move_helper);
	g_prediction_random_seed = *reinterpret_cast<uintptr_t**>(memory.g_prediction_random_seed);
	g_weapon_system = *reinterpret_cast<IWeaponSystem**>(memory.g_weapon_system);
	g_glow_manager = *reinterpret_cast<CGlowObjectManager**>(memory.g_glow_manager);
	g_view_render_beams = *reinterpret_cast<IViewRenderBeams**>(memory.g_view_render_beams);
	g_player_resource = *reinterpret_cast<CSPlayerResource***>(memory.g_player_resource);
	g_demo_player = **reinterpret_cast<IDemoPlayer***>(memory.g_demo_player);
	g_panorama_marshall_helper = *reinterpret_cast<uintptr_t**>(memory.g_panorama_marshall_helper);
	g_ui_engine_source2 = **reinterpret_cast<CUIEngineSource2***>(memory.g_ui_engine_source2);
	g_client_state = **reinterpret_cast<CClientState***>(memory.g_client_state);
	g_input = *reinterpret_cast<CInput**>(memory.g_input);

	auto g_steam_client = reinterpret_cast<ISteamClient*(__cdecl*)(void)>(GetProcAddress(GetModuleHandleA(XORSTR("steam_api.dll")), XORSTR("SteamClient")))();
	g_steam_game_coordinator = reinterpret_cast<ISteamGameCoordinator*>(g_steam_client->GetISteamGenericInterface((void*)1, (void*)1, XORSTR("SteamGameCoordinator001")));

	g_random_seed = reinterpret_cast<std::uintptr_t>(GetProcAddress(GetModuleHandleA(XORSTR("vstdlib.dll")), XORSTR("RandomSeed")));
	g_random_float = reinterpret_cast<std::uintptr_t>(GetProcAddress(GetModuleHandleA(XORSTR("vstdlib.dll")), XORSTR("RandomFloat")));
	g_mem_alloc = *reinterpret_cast<IMemAlloc**>(GetProcAddress(GetModuleHandleA(XORSTR("tier0.dll")), XORSTR("g_pMemAlloc")));

}