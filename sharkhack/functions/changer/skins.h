#pragma once

#include "../../utils/entity.h"

static int32_t unique_id = 1;

#define SEQUENCE_DEFAULT_DRAW 0
#define SEQUENCE_DEFAULT_IDLE1 1
#define SEQUENCE_DEFAULT_IDLE2 2
#define SEQUENCE_DEFAULT_LIGHT_MISS1 3
#define SEQUENCE_DEFAULT_LIGHT_MISS2 4
#define SEQUENCE_DEFAULT_HEAVY_MISS1 9
#define SEQUENCE_DEFAULT_HEAVY_HIT1 10
#define SEQUENCE_DEFAULT_HEAVY_BACKSTAB 11
#define SEQUENCE_DEFAULT_LOOKAT01 12

#define SEQUENCE_BUTTERFLY_DRAW 0
#define SEQUENCE_BUTTERFLY_DRAW2 1
#define SEQUENCE_BUTTERFLY_LOOKAT01 13
#define SEQUENCE_BUTTERFLY_LOOKAT03 15

#define SEQUENCE_FALCHION_IDLE1 1
#define SEQUENCE_FALCHION_HEAVY_MISS1 8
#define SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP 9
#define SEQUENCE_FALCHION_LOOKAT01 12
#define SEQUENCE_FALCHION_LOOKAT02 13

#define SEQUENCE_DAGGERS_IDLE1 1
#define SEQUENCE_DAGGERS_LIGHT_MISS1 2
#define SEQUENCE_DAGGERS_LIGHT_MISS5 6
#define SEQUENCE_DAGGERS_HEAVY_MISS2 11
#define SEQUENCE_DAGGERS_HEAVY_MISS1 12

#define SEQUENCE_BOWIE_IDLE1 1

#define SEQUENCE_KNIFES_DRAW 0
#define SEQUENCE_KNIFES_DRAW2 1

#define SEQUENCE_KNIFES_LOOKAT01 13
#define SEQUENCE_KNIFES_LOOKAT02 14
#define SEQUENCE_KNIFES_LOOKAT03 15

namespace changer {
	void on_init();

	void apply_other();
	void apply_agents();
	void apply_skins();
	void apply_gloves();

	void hud_icons(C_BasePlayer* local_player, CPlayerInventory* local_inventory, IGameEvent* event);
	void on_event(IGameEvent* event);

	static int get_rarity(const int item_rarity, const int kit_rarity) noexcept
	{
		const auto maximum_rarity = (kit_rarity == 7) + 6;
		auto rarity = item_rarity + kit_rarity - 1;
		if (rarity >= 0)
		{
			if (rarity > maximum_rarity)
				rarity = maximum_rarity;
		}
		else
			rarity = 0;

		return rarity;
	}
}
