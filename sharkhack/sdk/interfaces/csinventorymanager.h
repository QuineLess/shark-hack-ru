#pragma once
struct CSInventoryManager {
	bool EquipItemInLoadout(int team, int slot, uint64_t item_id) {
		static auto fn = reinterpret_cast<bool(__thiscall*)(void*, int, int, uint64_t, bool)>(memory.equip_item_in_loadout);
		return fn(this, team, slot, item_id, true);
	}
};