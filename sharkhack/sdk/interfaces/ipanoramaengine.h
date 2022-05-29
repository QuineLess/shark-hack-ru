#pragma once

struct IUIPanel {
	get_virtual_fn(9, get_panel_id(), const char* (__thiscall*)(void*));
	get_virtual_fn(25, get_parent(), IUIPanel* (__thiscall*)(void*));
};

struct IUIEngine {
	get_virtual_fn(36, is_valid_panel_ptr(IUIPanel* lol), bool(__thiscall*)(void*, IUIPanel*), lol);
	get_virtual_fn(56, get_last_target_panel(), IUIPanel* (__thiscall*)(void*));
	get_virtual_fn(113, run_script(void* panel, char const* entire_js_code, char const* path_to_xml_context, int _1, int _2, bool already_compiled, bool fe), void* (__thiscall*)(void*, void*, char const*, char const*, int, int, bool, bool), panel, entire_js_code, path_to_xml_context, _1, _1, already_compiled, fe);
};


struct IPanoramaUIEngine {
	get_virtual_fn(11, access_ui_engine(), IUIEngine* (__thiscall*)(void*));
};