#include "weaponids.h"

weaponids_t weaponids = {};

void weaponids_t::on_init() {
	const auto item_system = reinterpret_cast<uintptr_t*>(memory.get_item_system());

	for (size_t i = 0; i < item_system[58]; i++) {
		auto def = *reinterpret_cast<CEconItemDefinition**>(item_system[53] + 4 * (3 * i) + 4);
		const auto type = get_hash(def->get_item_type_name());

		if (type != utils::get_const(XORSTR("#CSGO_Type_Pistol")) && type != utils::get_const(XORSTR("#CSGO_Type_Rifle")) && type != utils::get_const(XORSTR("#CSGO_Type_SniperRifle")) && type != utils::get_const(XORSTR("#CSGO_Type_Machinegun")) && 
			type != utils::get_const(XORSTR("#CSGO_Type_SMG")) && type != utils::get_const(XORSTR("#CSGO_Type_Shotgun")) && type != utils::get_const(XORSTR("#CSGO_Type_Knife")) && type != utils::get_const(XORSTR("#CSGO_Type_Grenade"))
			&& type != utils::get_const(XORSTR("#CSGO_Type_Equipment")) && type != utils::get_const(XORSTR("#CSGO_Type_C4")) && type != utils::get_const(XORSTR("#Type_Hands")) && type != utils::get_const(XORSTR("#CSGO_Type_Collectible"))
			&& type != utils::get_const(XORSTR("#Type_CustomPlayer")) && type != utils::get_const(XORSTR("#CSGO_Tool_Sticker")) && type != utils::get_const(XORSTR("#CSGO_Type_WeaponCase")) && type != utils::get_const(XORSTR("#CSGO_Tool_WeaponCase_KeyTag"))
			&& type != utils::get_const(XORSTR("#CSGO_Type_MusicKit")))
			continue;

		m_list[def->get_definition_index()] = def;
		m_types[type].emplace_back(def);
	}
}