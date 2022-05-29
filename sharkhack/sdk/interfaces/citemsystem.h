#pragma once

struct CEconItemDefinition {
	get_virtual_fn(0, get_definition_index(), short(__thiscall*)(void*));
	get_virtual_fn(2, get_item_base_name(), const char* (__thiscall*)(void*));
	get_virtual_fn(3, get_item_type_name(), const char* (__thiscall*)(void*));
	get_virtual_fn(4, get_item_desc(), const char* (__thiscall*)(void*));
	get_virtual_fn(5, get_inventory_image(), const char* (__thiscall*)(void*));
	get_virtual_fn(6, get_base_player_display_model(), const char* (__thiscall*)(void*));
	get_virtual_fn(7, get_world_display_model(), const char* (__thiscall*)(void*));
	get_virtual_fn(12, get_rarity(), char(__thiscall*)(void*));
	get_virtual_fn(44, get_num_supported_sticker_slots(), int(__thiscall*)(void*));
	get_static_offset(const char*, get_weapon_name, 0x1DC);
	get_static_offset(int, get_loadout_slot, 0x268);
};

struct CEconPaintKitDefinition {
	get_static_offset(int, get_paint_kit, NULL);
	get_static_offset(int, get_rarity_value, 0x78);
	get_static_offset(const char*, get_skin_name, 0x4);
	get_static_offset(const char*, get_hud_name, 0x24);
};

struct CEconStickerDefinition {
	get_static_offset(int, get_sticker_id, NULL);
	get_static_offset(const char*, get_hud_name, 0x28);
	get_static_offset(const char*, get_inventory_image, 0x58);
};

struct CEconMusicDefinition {
	get_static_offset(int, get_kit_id, NULL);
	get_static_offset(const char*, get_hud_name, 0x8);
	get_static_offset(const char*, get_inventory_image, 0x14);
};

struct CCStrike15ItemSchema {
	get_virtual_fn(3, get_item_definition_count(), int(__thiscall*)(void*));
	get_virtual_fn(42, get_item_definition_by_name(const char* name), CEconItemDefinition* (__thiscall*)(void*, const char*), name);

	uint8_t* get_item_definition_by_map() {
		return *reinterpret_cast<uint8_t**>(uintptr_t(this) + *reinterpret_cast<uint8_t**>(memory.item_schema_map_offset) - 0x4);
	}

	CEconItemDefinition* get_item_definition_by_map_index(int i) {

		if (i < 0 || i >= get_item_definition_count())
			return nullptr;

		return *reinterpret_cast<CEconItemDefinition**>(get_item_definition_by_map() + 0x4 * (i * 0x3) + sizeof(void*));
	}
};

struct CCStrike15ItemSystem {
	get_virtual_fn(0, get_item_schema_interface(), CCStrike15ItemSchema* (__thiscall*)(void*));
};