#pragma once

struct c_shared_object {
	get_virtual_fn(1, get_type_id(), int32_t(__thiscall*)(void*));
};

struct c_shared_object_type_cache {
	get_virtual_fn(1, add_object(void* object), void(__thiscall*)(void*, void*), object);
	get_virtual_fn(3, remove_object(void* object), void(__thiscall*)(void*, void*), object);

	std::vector<CEconItem*> GetEconItems() {
		std::vector<CEconItem*> ret;

		auto size = *reinterpret_cast<size_t*>(this + 0x18);
		auto data = *reinterpret_cast<uintptr_t**>(this + 0x4);

		for (size_t i = 0; i < size; i++)
			ret.push_back(reinterpret_cast<CEconItem*>(data[i]));

		return ret;
	}
};

struct c_client_shared_object_cache {
	c_shared_object_type_cache* find_base_type_cache(int32_t class_id) {
		static auto fn_create_base_type_cache = reinterpret_cast<c_shared_object_type_cache * (__thiscall*)(void*, int32_t)>(memory.fn_create_base_type_cache);
		return fn_create_base_type_cache(this, class_id);
	}
};

struct weapon_drop_info {
	int item_definition;
	int paint_kit;
	int rarity;
};


struct soid_s {
	uint64_t id;
	uint32_t type;
	uint32_t padding;
};

class CPlayerInventory {
public:
	get_virtual_fn(0, so_created(soid_s owner, c_shared_object* object, int32_t event), void(__thiscall*)(void*, soid_s, c_shared_object*, int32_t), owner, object, event);
	get_virtual_fn(1, so_updated(soid_s owner, c_shared_object* object, int32_t event), void(__thiscall*)(void*, soid_s, c_shared_object*, int32_t), owner, object, event);
	get_virtual_fn(2, so_destroyed(soid_s owner, c_shared_object* object, int32_t event), void(__thiscall*)(void*, soid_s, c_shared_object*, int32_t), owner, object, event);
	get_virtual_fn(8, get_item_in_loadout(int32_t team, int32_t slot), CEconItemView* (__thiscall*)(void*, int32_t, int32_t), team, slot);

	get_static_offset(soid_s, get_owner_id, 0x8);

	void RemoveItem(CEconItem* item) {
		so_destroyed(get_owner_id(), (c_shared_object*)item, 4);
		get_item_base_type_cache()->remove_object(item);
	}

	c_client_shared_object_cache* get_soc() {
		return *reinterpret_cast<c_client_shared_object_cache**>(std::uintptr_t(this) + 0x90);
	}

	c_shared_object_type_cache* get_item_base_type_cache() {
		const auto soc = get_soc();

		if (!soc)
			return nullptr;

		return soc->find_base_type_cache(1);
	}

	CEconItem* CreateEconItem() {
		static auto fnCreateSharedObjectSubclass_EconItem_ = reinterpret_cast<CEconItem * (__stdcall*)()>(*reinterpret_cast<uintptr_t*>(memory.create_econ_item));
		return fnCreateSharedObjectSubclass_EconItem_();
	}

	CEconItemView* GetInventoryItemByItemID(const uint64_t ItemID) {
		static auto fn = reinterpret_cast<CEconItemView * (__thiscall*)(void*, uint64_t)>(memory.get_inventory_item_by_item_id);
		return fn(this, ItemID);
	}

	uint64_t GetHighestItemID() {
		uint64_t max_id = 1;
		for (auto&& i : get_item_base_type_cache()->GetEconItems()) {
			auto is_default_item = (uint32_t)(*i->GetItemID() >> 32) == 0xF0000000;
			if (!is_default_item)
				if (*i->GetItemID() > max_id)
					max_id = *i->GetItemID();
		}
		return max_id;
	}

	void RecursiveAddLootToLootList(uintptr_t v2, std::vector<weapon_drop_info>& drop) {
		auto size = *(uintptr_t*)((*(int(__thiscall**)(int))(*(uintptr_t*)v2 + 4))(v2) + 12);
		uintptr_t v9 = 0;
		uintptr_t v8 = 0;

		do
		{

			auto v4 = (uintptr_t*)(*(int(__thiscall**)(int))(*(uintptr_t*)v2 + 4))(v2);
			auto v5 = v9 + *v4;

			if (*(BYTE*)(v5 + 24))
			{
				static auto get_loot_list_interface_by_index_fn = reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t, uintptr_t)>(memory.get_loot_list_interface_by_index);

				auto v7 = get_loot_list_interface_by_index_fn(reinterpret_cast<uintptr_t>(memory.get_item_system()) + 4, *(uintptr_t*)v5);
				RecursiveAddLootToLootList(v7, drop);
			}
			else
			{
				auto paintkit = *reinterpret_cast<int*>(v5 + 0x4);
				auto itemdef = *reinterpret_cast<int*>(v5);
				if (itemdef != 0)
				{
					auto set_rarity = std::string(*reinterpret_cast<const char**>(v2 + 0x18));

					auto rarity = 0;

					if (set_rarity.find("_common") != std::string::npos)
						rarity = 1;
					else if (set_rarity.find("_uncommon") != std::string::npos)
						rarity = 2;
					else if (set_rarity.find("_rare") != std::string::npos)
						rarity = 3;
					else if (set_rarity.find("_mythical") != std::string::npos)
						rarity = 4;
					else if (set_rarity.find("_legendary") != std::string::npos)
						rarity = 5;
					else if (set_rarity.find("_ancient") != std::string::npos)
						rarity = 6;
					else if (set_rarity.find("_immortal") != std::string::npos)
						rarity = 7;

					drop.push_back(weapon_drop_info{ itemdef, paintkit, rarity });
				}
			}

			v9 += 28;
			++v8;
		} while (v8 < size);
	}

	std::vector<weapon_drop_info> GetWeaponsForCrate(const char* name) {
		std::vector<weapon_drop_info> drop;
		static auto get_loot_list_by_name_fn = reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t, const char*, signed int)>(memory.get_loot_list_by_name);
		auto v2 = get_loot_list_by_name_fn(reinterpret_cast<uintptr_t>(memory.get_item_system()) + 4, name, 0);
		RecursiveAddLootToLootList(v2, drop);
		return drop;
	}

	CUtlVector<CEconItemView*>* GetInventoryItems() {
		return reinterpret_cast<CUtlVector<CEconItemView*>*>(this + 0x2C);
	}

	CEconItemView* FindKeyToOpen(CEconItemView* crate) {
		for (int i = 0; i < this->GetInventoryItems()->m_Size; i++)
		{
			auto prob_key = (this->GetInventoryItems()->m_Memory.m_pMemory[i]);
			if (prob_key)
			{
				if (prob_key->ToolCanApplyTo(crate))
					return prob_key;
			}
		}

		return nullptr;
	}
};
