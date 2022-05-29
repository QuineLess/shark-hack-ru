#pragma once
#include "../sdk/interfaces.h"

#include <d3d9types.h>
#include <intrin.h>

#define registers uintptr_t ecx, uintptr_t edx

inline std::add_pointer_t<void __fastcall(registers, int, float, bool)> o_create_move;
inline std::add_pointer_t<bool __fastcall(registers, CEconItemView*, uint32_t, bool, bool)>o_set_item_backpack_position;
inline std::add_pointer_t<void __fastcall(registers, void*)> o_read_packet_entities;
inline std::add_pointer_t<bool __fastcall(registers)> o_is_connected;
inline std::add_pointer_t<bool __fastcall(registers)> o_is_playing_demo;
inline std::add_pointer_t<bool __fastcall(registers)> o_is_using_static_prop_debug_modes;
inline std::add_pointer_t<float __fastcall(registers, int, int)> o_get_screen_aspect_ratio;
inline std::add_pointer_t<float __fastcall(registers)> o_view_model_fov;
inline std::add_pointer_t<int __fastcall(registers, int)> o_do_post_screen_effects;
inline std::add_pointer_t<int __stdcall(void*)> o_unknown_file_sys;
inline std::add_pointer_t<int __fastcall(registers, int, float, float, float)> o_set_viewmodel_offsets;
inline std::add_pointer_t<void __fastcall(registers, int, int, int, int)> o_set_draw_color;
inline std::add_pointer_t<void __fastcall(registers, CViewSetup*)> o_override_view;
inline std::add_pointer_t<void __fastcall(registers)> o_lock_cursor;
inline std::add_pointer_t<long __stdcall(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> o_reset;
inline std::add_pointer_t<long __stdcall(IDirect3DDevice9*)> o_end_scene;
inline std::add_pointer_t<float __fastcall(registers)> o_get_alpha_modulation;
inline std::add_pointer_t<void __fastcall(registers, float&, float&, float&)> o_get_color_modulation;
inline std::add_pointer_t<void __fastcall(registers, void*, DrawModelInfo_t*, matrix3x4_t*, float*, float*, Vector&, int)> o_draw_model_execute;
inline std::add_pointer_t<void __fastcall(registers, void*, int, int, const char*, unsigned int, const char*, float, int, float, int, int, const Vector*, const Vector*, void*, bool, float, int, int)> o_emit_sound;
inline std::add_pointer_t<int32_t __stdcall(void*, const Vector&, const Vector&, uint16_t*, int32_t)> o_list_leaves_in_box;
inline std::add_pointer_t<void __fastcall(registers, int, bool)> o_suppress_lists;
inline std::add_pointer_t<EGCResult __fastcall(registers, uint32_t*, void*, uint32_t, uint32_t*)> o_retrieve_msg;
inline std::add_pointer_t<bool __fastcall(registers, uintptr_t)> o_broadcast_event;
inline std::add_pointer_t<const char* __fastcall(registers, int, int)> o_unknown_call;
inline std::add_pointer_t<void __fastcall(registers, vgui::VPANEL, bool, bool)> o_paint_traverse;
inline std::add_pointer_t<bool __fastcall(registers, IGameEvent*)> o_fire_event;
inline std::add_pointer_t<void __stdcall(bool)> o_hud_update;
inline std::add_pointer_t<void* __stdcall()> o_read_packet;
inline std::add_pointer_t<bool __fastcall(registers, INetMessage*, bool, bool)> o_send_net_message;
inline std::add_pointer_t<int __fastcall(INetChannel*, void*, bf_write*)> o_send_datagram;

namespace hooks {
	void on_init();
}