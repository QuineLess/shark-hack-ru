#pragma once

#include "utils.h"
#include "xor_str.h"

#define get_signature(name, module_name, signature, offset) uint8_t* name = utils::find_signature(module_name, signature) + offset

struct CCStrike15ItemSchema;
struct CCStrike15ItemSystem;

struct memory_t {
	get_signature(g_device, XORSTR("shaderapidx9.dll"), XORSTR("A1 ? ? ? ? 50 8B 08 FF 51 0C"), 0x1);
	get_signature(frame_net_update_end, XORSTR("client.dll"), XORSTR("5F 5E 5D C2 04 00 83 3D"), NULL);
	get_signature(frame_net_update_start, XORSTR("client.dll"), XORSTR("FF 50 40 5F 5E 5D C2 04 00 80"), 0x3);
	get_signature(frame_render_start, XORSTR("client.dll"), XORSTR("FF 50 40 8B 1D"), 0x3);
	get_signature(frame_render_end, XORSTR("client.dll"), XORSTR("FF 50 40 E8 ? ? ? ? 8B 0D"), 0x3);
	get_signature(scope_arc, XORSTR("client.dll"), XORSTR("6A 00 FF 50 3C 8B 0D ? ? ? ? FF B7"), 0x5);
	get_signature(scope_lens, XORSTR("client.dll"), XORSTR("FF 50 3C 8B 4C 24 20"), 0x3);
	get_signature(line_goes_through_smoke, XORSTR("client.dll"), XORSTR("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"), NULL);
	get_signature(smoke_count, XORSTR("client.dll"), XORSTR("8B 15 ? ? ? ? 0F 57 C0 56"), 0x2);
	get_signature(item_schema_map_offset, XORSTR("client.dll"), XORSTR("8B 87 ? ? ? ? 83 7C 88 ? ? 7C 0A 8B 44 88 04 5F 5E 5D C2 04 00 8B 87"), 0x2);
	get_signature(item_system, XORSTR("client.dll"), XORSTR("A1 ? ? ? ? 85 C0 75 53"), NULL);
	get_signature(get_static_data, XORSTR("client.dll"), XORSTR("55 8B EC 51 53 8B D9 8B ? ? ? ? ? 56 57 8B ? ? ? ? ? 85 FF 74 16"), NULL);
	get_signature(get_soc_data, XORSTR("client.dll"), XORSTR("55 8B EC 83 E4 F0 83 EC 18 56 8B F1 57 8B 86"), NULL);
	get_signature(clear_inventory_image_rgba, XORSTR("client.dll"), XORSTR("55 8B EC 81 EC ? ? ? ? 57 8B F9 C7 47"), NULL);
	get_signature(create_econ_item , XORSTR("client.dll"), XORSTR("55 8B EC 83 EC 1C 8D 45 E4 C7 45"), 0x14);
	get_signature(remove_item, XORSTR("client.dll"), XORSTR("55 8B EC 83 E4 F8 51 53 56 57 FF 75 0C 8B F1"), NULL);
	get_signature(find_so_cache, XORSTR("client.dll"), XORSTR("55 8B EC 83 E4 F8 83 EC 1C 0F 10 45 08"), NULL);
	get_signature(create_base_type_cache, XORSTR("client.dll"), XORSTR("55 8B EC 51 53 56 8B D9 8D 45 08"), NULL);
	get_signature(client_system, XORSTR("client.dll"), XORSTR("8B 0D ? ? ? ? 6A 00 83 EC 10"), 0x2);
	get_signature(get_inventory_item_by_item_id, XORSTR("client.dll"), XORSTR("55 8B EC 8B 55 08 83 EC 10 8B C2"), NULL);
	get_signature(equip_item_in_loadout, XORSTR("client.dll"), XORSTR("55 8B EC 83 E4 F8 83 EC 24 83 3D ? ? ? ? ? 53 56 57 8B F9"), NULL);
	get_signature(g_player_inventory, XORSTR("client.dll"), XORSTR("8B 0D ? ? ? ? 85 C9 74 18 51"), 0x2);
	get_signature(g_inventory_manager, XORSTR("client.dll"), XORSTR("C7 05 ? ? ? ? ? ? ? ? BF ? ? ? ? BE"), 0x2);
	get_signature(set_dynamic_attribute_value, XORSTR("client.dll"), XORSTR("55 8B EC 83 E4 F8 83 EC 3C 53 8B 5D 08 56 57 6A 00"), NULL);
	get_signature(get_econ_item_view, XORSTR("client.dll"), XORSTR("8B 81 ? ? ? ? 81 C1 ? ? ? ? FF 50 04 83 C0 40 C3"), NULL);
	get_signature(equip_wearable, XORSTR("client.dll"), XORSTR("55 8B EC 83 EC 10 53 8B 5D 08 57 8B F9"), NULL);
	get_signature(initialize_attibutes, "client.dll", "55 8B EC 83 E4 F8 83 EC 0C 53 56 8B F1 8B 86", NULL);
	get_signature(set_abs_origin, XORSTR("client.dll"), XORSTR("55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8"), NULL);
	get_signature(set_abs_angles, XORSTR("client.dll"), XORSTR("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"), NULL);
	get_signature(material, XORSTR("materialsystem.dll"), XORSTR("C7 06 ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? 89 4D E8"), 0x2);
	get_signature(custom_econ_name, XORSTR("client.dll"), XORSTR("E8 ? ? ? ? 8B 46 78 C1 E8 0A A8 01 74 13 8B 46 34"), NULL);
	get_signature(custom_sky_box, XORSTR("engine.dll"), XORSTR("55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"), NULL);
	get_signature(set_viewmodel_offsets, XORSTR("client.dll"), XORSTR("55 8B EC 8B 45 08 F3 0F 7E 45"), NULL);
	get_signature(is_using_static_prop_debug_modes, XORSTR("engine.dll"), XORSTR("8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? EB ? 8B 01 FF 50 ? 83 F8 ? 0F 85 ? ? ? ? 8B 0D"), NULL);
	get_signature(g_move_data, XORSTR("client.dll"), XORSTR("A1 ? ? ? ? F3 0F 59 CD"), 0x1);
	get_signature(g_move_helper, XORSTR("client.dll"), XORSTR("8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01"), 0x2);
	get_signature(g_prediction_random_seed, XORSTR("client.dll"), XORSTR("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04"), 0x2);
	get_signature(g_weapon_system, XORSTR("client.dll"), XORSTR("8B 35 ? ? ? ? FF 10 0F B7 C0"), 0x2);
	get_signature(g_glow_manager, XORSTR("client.dll"), XORSTR("0F 11 05 ? ? ? ? 83 C8 01"), 0x3);
	get_signature(get_accept, XORSTR("client.dll"), XORSTR("55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"), NULL);
	get_signature(get_line_goes_through_smoke, XORSTR("client.dll"), XORSTR("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"), NULL);
	get_signature(inventory_always, XORSTR("client.dll"), XORSTR("84 C0 75 05 B0 01 5F"), NULL);
	get_signature(unknown_file_sys_ret_addr, XORSTR("panorama.dll"), XORSTR("C7 43 ? ? ? ? ? 83 7C 24 ? ? 7C 25 8B 54 24 48"), NULL);
	get_signature(element_fn, XORSTR("client.dll"), XORSTR("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39"), NULL);
	get_signature(element_this, XORSTR("client.dll"), XORSTR("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08"), 0x1);
	get_signature(g_view_render_beams, XORSTR("client.dll"), XORSTR("B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9"), 0x1);
	get_signature(set_clantag, XORSTR("engine.dll"), XORSTR("53 56 57 8B DA 8B F9 FF 15"), NULL);
	get_signature(g_player_resource, XORSTR("client.dll"), XORSTR("A1 ? ? ? ? 89 44 24 60 85"), 0x1);
	get_signature(g_demo_player, XORSTR("engine.dll"), XORSTR("8B 0D ? ? ? ? 8B 01 8B 40 1C FF D0 84 C0 74 0A"), 0x2);
	get_signature(kv_find_key, XORSTR("client.dll"), XORSTR("55 8B EC 83 EC 1C 53 8B D9 85 DB"), NULL);
	get_signature(kv_set_string, XORSTR("client.dll"), XORSTR("55 8B EC A1 ? ? ? ? 53 56 57 8B F9 8B 08 8B 01"), NULL);
	get_signature(kv_init_key_values, XORSTR("client.dll"), "55 8B EC 51 33 C0 C7 45 ? ? ? ? ? 56 8B F1 81 26 ? ? ? ? C6 46 03 ? 89 46 10 89 46 18 89 46 14 89 46 1C 89 46 04 89 46 08 89 46 0C FF 15 ? ? ? ? 6A 01 FF 75 08 8D 4D FC 8B 10 51 8B C8 FF 52 24 8B 0E 33 4D FC 81 E1 ? ? ? ? 31 0E 88 46 03", NULL);
	get_signature(kv_set_uint64, XORSTR("client.dll"), XORSTR("55 8B EC 56 6A 01 FF 75 08"), NULL);
	get_signature(kv_get_string, XORSTR("client.dll"), XORSTR("55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 8B 5D 08"), NULL);
	get_signature(kv_get_int, XORSTR("client.dll"), XORSTR("55 8B EC 6A ? FF 75 08 E8 ? ? ? ? 85 C0 74 45"), NULL);
	get_signature(kv_from_string, XORSTR("client.dll"), XORSTR("55 8B EC 81 EC ? ? ? ? 85 D2 53 56 57"), NULL);
	get_signature(unknown_call_ret_addr, XORSTR("client.dll"), XORSTR("85 C0 0F 84 ? ? ? ? 8B C8 E8 ? ? ? ? 52 50 E8 ? ? ? ? 83 C4 08 89 44 24 14 85 C0 0F 84 ? ? ? ? 8B 0D"), NULL);
	get_signature(failed_to_open_crate, XORSTR("client.dll"), XORSTR("68 ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0 84 C0 74 1A 8B 35 ? ? ? ? 8B D3 33 C9 8B 3E E8 ? ? ? ? 50 8B CE FF 97 ? ? ? ? 5F 5E B0 01 5B 8B E5 5D C2 04 00"), 0x1);
	get_signature(item_customization_notification, XORSTR("client.dll"), XORSTR("68 ? ? ? ? 8B 80 ? ? ? ? FF D0 84 C0 74 28"), 0x1);
	get_signature(get_typed_attribute_value, XORSTR("client.dll"), XORSTR("E8 ? ? ? ? 0F B6 45 10"), NULL);
	get_signature(get_crate_series_unk, XORSTR("client.dll"), XORSTR("55 8B EC 8B 45 08 56 57 8B 30 8B 41 10 83 F8 FF 74 1E 8B 79 04 8D 0C 40 8B 54 CF 10 3B D6 7E 05"), NULL);
	get_signature(tool_can_apply_to, XORSTR("client.dll"), XORSTR("55 8B EC 83 EC 18 53 56 8B F1 57 8B FA"), NULL);
	get_signature(get_loot_list_by_name, XORSTR("client.dll"), XORSTR("55 8B EC 8B 55 08 81 EC ? ? ? ? 56 57"), NULL);
	get_signature(get_loot_list_interface_by_index, XORSTR("client.dll"), XORSTR("55 8B EC 8B 55 08 56 8B F1 85 D2 78 48"), NULL);
	get_signature(g_panorama_marshall_helper, XORSTR("client.dll"), XORSTR("68 ? ? ? ? 8B C8 E8 ? ? ? ? 8D 4D F4 FF 15 ? ? ? ? 8B CF FF 15 ? ? ? ? 5F 5E 8B E5 5D C3"), 0x1);
	get_signature(g_ui_engine_source2, XORSTR("client.dll"), XORSTR("8B 0D ? ? ? ? 53 56 57 8B 01 8B DA"), 0x2);
	get_signature(unlock_all_achievements, XORSTR("client.dll"), XORSTR("8B 7D 0C 83 FF 01 0F 8D"), NULL);
	get_signature(has_ongoing_match, XORSTR("client.dll"), XORSTR("A1 ? ? ? ? 8B 4D 08 D1 E8"), 0x1);
	get_signature(action_reconnect_to_ongoing_match, XORSTR("client.dll"), XORSTR("A1 ? ? ? ? D1 E8 A8 01 74 4D"), 0x1);
	get_signature(update_animation_state, XORSTR("client.dll"), XORSTR("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"), NULL);
	get_signature(reset_animation_state, XORSTR("client.dll"), XORSTR("56 6A 01 68 ? ? ? ? 8B F1"), NULL);
	get_signature(create_animation_state, XORSTR("client.dll"), XORSTR("55 8B EC 56 8B F1 B9 ? ? ? ? C7 46"), NULL);
	get_signature(o_override_post_processing, XORSTR("client.dll"), XORSTR("80 3D ? ? ? ? ? 53 56 57 0F 85"), 0x2);
	get_signature(is_breakable, XORSTR("client.dll"), XORSTR("55 8B EC 51 56 8B F1 85 F6 74 68"), NULL);
	get_signature(g_client_state, XORSTR("engine.dll"), XORSTR("A1 ? ? ? ? 8B 80 ? ? ? ? C3"), 0x1);
	get_signature(g_input, XORSTR("client.dll"), XORSTR("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10"), 0x1);
	get_signature(fn_create_base_type_cache, XORSTR("client.dll"), XORSTR("55 8B EC 51 53 56 8B D9 8D 45 08"), NULL);
	get_signature(setup_bones_for_attachment_queries, XORSTR("client.dll"), XORSTR("55 8B EC 83 EC 14 83 3D ? ? ? ? ? 53"), NULL);
	get_signature(set_merge_mdl, XORSTR("client.dll"), XORSTR("55 8B EC 57 8B F9 8B 0D ? ? ? ? 85 C9 75"), NULL);
	get_signature(create_model, XORSTR("client.dll"), XORSTR("53 8B D9 56 57 8D 4B 04 C7 03 ? ? ? ? E8 ? ? ? ? 6A"), NULL);
	get_signature(list_leaves, XORSTR("client.dll"), XORSTR("56 52 FF 50 18"), 0x5);

	CCStrike15ItemSystem * get_item_system();
	CCStrike15ItemSchema * get_item_schema();
};

inline memory_t memory;
