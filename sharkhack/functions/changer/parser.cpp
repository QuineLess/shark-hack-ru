#include "parser.h"

void parser::on_init() {
	const auto item_system = reinterpret_cast<uintptr_t*>(memory.get_item_system());

	for (size_t i = 0; i < item_system[170] + 1; i++) {
		auto kit = *reinterpret_cast<CEconPaintKitDefinition**>(item_system[164] + 24 * i + 0x14);
		const auto kit_name = kit->get_skin_name();

		for (size_t j = 0; j < item_system[143]; j++) {
			const auto result = 120 * j + item_system[137] + 24;
			auto name = std::string(*reinterpret_cast<const char**>(result));

			if (name[name.length() - 7] != '_')
				continue;

			name = name.substr(23, name.length() - 30);

			const auto pos = name.find(kit_name);
			if (pos == std::string::npos)
				continue;

			const auto weapon_name = name.substr(0, pos - 1);

			if (name.length() - pos != strlen(kit_name))
				continue;

			auto weapon = memory.get_item_schema()->get_item_definition_by_name(weapon_name.c_str());
			if (!weapon)
				continue;

			config.inventory_weapon_skins[weapon->get_definition_index()].emplace_back(kit);
			config.inventory_skin_info[kit->get_paint_kit()] = kit;
		}
	}

	for (const auto& it : config.inventory_weapon_skins) {
		const auto type = get_hash(weaponids.m_list[it.first]->get_item_type_name());

		if (type == utils::get_const(XORSTR("#CSGO_Type_Knife")) || type == utils::get_const(XORSTR("#Type_Hands")))
			continue;

		std::sort(config.inventory_weapon_skins[it.first].begin(), config.inventory_weapon_skins[it.first].end(), [](auto a, auto b) -> bool {
			return a->get_rarity_value() > b->get_rarity_value();
			});
	}

	for (size_t i = 1; i < item_system[179]; i++) {
		auto sticker = *reinterpret_cast<CEconStickerDefinition**>(item_system[173] + 24 * i + 0x14);

		auto item_name = std::string(sticker->get_hud_name());

		if (item_name.find("SprayKit") != std::string::npos || item_name.find("PatchKit") != std::string::npos)
			continue;

		config.inventory_sticker_info.insert({ sticker->get_sticker_id(), utils::localize_text(sticker->get_sticker_id() == 242 ? "StickerKit_dhw2014_teamdignitas_gold" : item_name.c_str()) });
		config.inventory_sticker_image.insert({ sticker->get_sticker_id(), sticker->get_inventory_image() });
	}

	config.inventory_sticker_info.insert({ 0, "-" });

	for (size_t i = 2; i < item_system[259] + 1; i++) {
		auto music_kit = *reinterpret_cast<CEconMusicDefinition**>(item_system[253] + 24 * i + 0x14);
		config.inventory_music_kits_info[music_kit->get_kit_id()] = music_kit;
	}
}
