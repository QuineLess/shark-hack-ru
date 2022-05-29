#include "skins.h"

CreateClientClassFn get_wearable_create_fn() {
	auto classes = g_client->GetAllClasses();
	while (get_hash(classes->m_pNetworkName) != utils::get_const(XORSTR("CEconWearable")))
		classes = classes->m_pNext;

	return classes->m_pCreateFn;
}

RecvVarProxyFn sequence_proxy_fn = NULL;

#define model(s) !strcmp(get_model, s)

int get_new_animation(const char* get_model, const int sequence) {

	if (model(XORSTR("models/weapons/v_knife_butterfly.mdl"))) {
		switch (sequence) {
		case SEQUENCE_DEFAULT_DRAW:
			return utils::random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return utils::random(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
		default:
			return sequence + 1;
		}
	}

	if (model(XORSTR("models/weapons/v_knife_falchion_advanced.mdl"))) {
		switch (sequence) {
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_FALCHION_IDLE1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return utils::random(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return utils::random(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence - 1;
		}
	}

	if (model(XORSTR("models/weapons/v_knife_push.mdl"))) {
		switch (sequence) {
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_DAGGERS_IDLE1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2:
			return utils::random(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return utils::random(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01:
			return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence + 2;
		}
	}

	if (model(XORSTR("models/weapons/v_knife_survival_bowie.mdl"))) {
		switch (sequence) {
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_BOWIE_IDLE1;
		default:
			return sequence - 1;
		}
	}

	if (model(XORSTR("models/weapons/v_knife_ursus.mdl")) || model(XORSTR("models/weapons/v_knife_cord.mdl")) ||
		model(XORSTR("models/weapons/v_knife_canis.mdl")) || model(XORSTR("models/weapons/v_knife_outdoor.mdl")) ||
		model(XORSTR("models/weapons/v_knife_skeleton.mdl"))) {
		switch (sequence) {
		case SEQUENCE_DEFAULT_DRAW:
			return utils::random(SEQUENCE_KNIFES_DRAW, SEQUENCE_KNIFES_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return utils::random(SEQUENCE_KNIFES_LOOKAT01, SEQUENCE_KNIFES_LOOKAT02);
		default:
			return sequence + 1;
		}
	}

	if (model(XORSTR("models/weapons/v_knife_stiletto.mdl"))) {
		switch (sequence) {
		case SEQUENCE_DEFAULT_LOOKAT01:
			return utils::random(12, 13);
		}
	}

	if (model(XORSTR("models/weapons/v_knife_widowmaker.mdl"))) {
		switch (sequence) {
		case SEQUENCE_DEFAULT_LOOKAT01:
			return utils::random(SEQUENCE_KNIFES_LOOKAT02, SEQUENCE_KNIFES_LOOKAT03);
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
			return sequence - 1;
		}
	}

	return sequence;
}

void remapping(CRecvProxyData * data, C_BaseViewModel * entity, C_BasePlayer* local_player) {

	const auto owner = reinterpret_cast<C_BaseEntity*>(g_ent_list->GetClientEntityFromHandle(entity->get_owner()));
	if (!owner)
		return;

	const auto view_model_weapon = reinterpret_cast<C_BaseCombatWeapon*>(g_ent_list->GetClientEntityFromHandle(entity->get_weapon()));
	if (!view_model_weapon)
		return;

	const auto type = get_hash(weaponids.m_list[view_model_weapon->get_definition_index()]->get_item_type_name());

	if (type != utils::get_const(XORSTR("#CSGO_Type_Knife")))
		return;

	auto & sequence = data->m_Value.m_Int;
	sequence = get_new_animation(weaponids.m_list[view_model_weapon->get_definition_index()]->get_base_player_display_model(), sequence);
}

void sequence_fix(const CRecvProxyData * const_data, void * structure, void * out) {
	auto local_player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(g_engine->GetLocalPlayer()));
	const auto data = const_cast<CRecvProxyData *>(const_data);
	const auto view_model = static_cast<C_BaseViewModel *>(structure);

	if (!local_player || !local_player->is_alive())
		return sequence_proxy_fn(data, structure, out);

	remapping(data, view_model, local_player);

	sequence_proxy_fn(data, structure, out);
}

void changer::on_init() {
	for (auto class_list = g_client->GetAllClasses(); class_list; class_list = class_list->m_pNext) {

		if (get_hash(class_list->m_pNetworkName) != utils::get_const(XORSTR("CBaseViewModel")))
			continue;

		const auto table = class_list->m_pRecvTable;

		for (auto i = 0; i < table->m_nProps; i++) {
			const auto prop = &table->m_pProps[i];
			if (!prop || get_hash(prop->m_pVarName) != utils::get_const(XORSTR("m_nSequence")))
				continue;

			sequence_proxy_fn = prop->m_ProxyFn;
			prop->m_ProxyFn = sequence_fix;

			break;
		}
	}
}

void changer::apply_other() {
	const auto local_player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(g_engine->GetLocalPlayer()));

	if (!local_player) 
		return;

	const auto equipped_medal = g_player_inventory->get_item_in_loadout(0, 55);
	*(*g_player_resource)->get_active_coin_rank(local_player->EntIndex()) = (equipped_medal && equipped_medal->GetSOCData()) ? *equipped_medal->GetSOCData()->GetDefinitionIndex() : 0;

	const auto equipped_music_kit = g_player_inventory->get_item_in_loadout(0, 54);
	*(*g_player_resource)->get_music_id(local_player->EntIndex()) = (equipped_music_kit && equipped_music_kit->GetSOCData()) ? int(equipped_music_kit->GetSOCData()->GetAttributes()[0].data[0]) : 0;
}

void changer::apply_agents() {

	const auto local_player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(g_engine->GetLocalPlayer()));

	if (!local_player)
		return;

	const auto econ_item_view = g_player_inventory->get_item_in_loadout(local_player->get_team(), 38);

	if (!econ_item_view)
		return;

	const auto soc_data = econ_item_view->GetSOCData();

	if (!soc_data)
		return;

	const auto def_index = weaponids.m_list[*soc_data->GetDefinitionIndex()];

	if (!def_index)
		return;

	if (!(get_hash(def_index->get_item_type_name()) == utils::get_const("#Type_CustomPlayer")))
		return;

	const auto model = def_index->get_base_player_display_model();

	if (!model)
		return;

	const auto index = g_mdl_info->get_model_index(model);

	local_player->set_model_index(index);

	if (const auto ragdoll = reinterpret_cast<C_BaseCombatWeapon*>(g_ent_list->GetClientEntityFromHandle(local_player->get_ragdoll())))
		ragdoll->set_model_index(index);

}

void changer::apply_skins() {

	const auto local_player_id = g_engine->GetLocalPlayer();
	const auto local_player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(local_player_id));

	if (!local_player)
		return;

	auto local_info = player_info_t();
	g_engine->GetPlayerInfo(local_player_id, local_info);

	const auto weapons = local_player->get_weapons();

	for (auto index = 0; weapons[index]; index++) {

		const auto weapon = reinterpret_cast<C_BaseCombatWeapon*>(g_ent_list->GetClientEntityFromHandle(weapons[index]));
		if (!weapon)
			continue;

		if (local_info.xuid_low != weapon->get_owner_xuid_low())
			continue;

		if (local_info.xuid_high != weapon->get_owner_xuid_high())
			continue;

		const auto econ_item_view = weapon->get_econ_item_view();
		if (!econ_item_view)
			continue;

		const auto weapon_definition = econ_item_view->GetStaticData();
		if (!weapon_definition)
			continue;

		const auto equipped_position = weapon_definition->get_loadout_slot();

		const auto econ_skin_data = g_player_inventory->get_item_in_loadout(weapon->get_team(), equipped_position);

		if (!econ_skin_data)
			continue;

		const auto static_data = econ_skin_data->GetStaticData();
		if (!static_data)
			continue;

		const auto econ_data = econ_skin_data->GetSOCData();
		if (!econ_data)
			continue;

		if (get_hash(weaponids.m_list[weapon->get_definition_index()]->get_item_type_name()) == utils::get_const(XORSTR("#CSGO_Type_Knife"))) {
			weapon->get_definition_index() = *econ_data->GetDefinitionIndex();
			weapon->set_model_index(g_mdl_info->get_model_index(weaponids.m_list[*econ_data->GetDefinitionIndex()]->get_base_player_display_model()));
		}
		else if (static_data->get_definition_index() != weapon->get_definition_index())
			continue;

		const auto id = *econ_data->GetItemID();

		const uint64_t mask = UINT_MAX;
		const uint32_t low_id = mask & id;
		const uint32_t high_id = id >> 32;

		weapon->get_item_id_low() = low_id;
		weapon->get_item_id_high() = high_id;
		weapon->get_account_id() = local_info.xuid_low;
	}

	const auto view_model = reinterpret_cast<C_BaseViewModel*>(g_ent_list->GetClientEntityFromHandle(local_player->get_view_model()));
	if (!view_model)
		return;

	const auto view_model_weapon = reinterpret_cast<C_BaseCombatWeapon*>(g_ent_list->GetClientEntityFromHandle(view_model->get_weapon()));
	if (!view_model_weapon)
		return;

	if (get_hash(weaponids.m_list[view_model_weapon->get_definition_index()]->get_item_type_name()) != utils::get_const(XORSTR("#CSGO_Type_Knife")))
		return;

	const auto econ_item_view = view_model_weapon->get_econ_item_view();
	if (!econ_item_view)
		return;

	const auto econ_data = econ_item_view->GetSOCData();
	if (!econ_data)
		return;

	const auto override_model_index = g_mdl_info->get_model_index(weaponids.m_list[view_model_weapon->get_definition_index()]->get_base_player_display_model());
	view_model->get_model_index() = override_model_index;
	view_model->get_view_model_index() = override_model_index;

	const auto world_model = reinterpret_cast<C_BaseWeaponWorldModel*>(g_ent_list->GetClientEntityFromHandle(view_model_weapon->get_weapon_world_model()));

	if (!world_model)
		return;

	world_model->set_model_index(override_model_index + 1);
	world_model->get_model_index() = override_model_index + 1;
}

void changer::apply_gloves() {

	const auto local_player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(g_engine->GetLocalPlayer()));

	if (!local_player)
		return;

	auto info = player_info_t();

	if (!g_engine->GetPlayerInfo(local_player->EntIndex(), info))
		return;

	const auto econ_item_view = g_player_inventory->get_item_in_loadout(local_player->get_team(), 41);

	if (!econ_item_view)
		return;

	const auto soc_data = econ_item_view->GetSOCData();

	if (!soc_data)
		return;

	const auto static_data = econ_item_view->GetStaticData();

	if (!static_data)
		return;

	const auto wearables = local_player->get_wearables();

	if (!wearables)
		return;

	auto id = *soc_data->GetItemID();
	constexpr std::uint64_t mask = (std::numeric_limits<std::uint32_t>::max)();
	const std::uint32_t low_id = mask & id;
	const std::uint32_t high_id = id >> 32;

	auto wearable = reinterpret_cast<C_BaseCombatWeapon*>(g_ent_list->GetClientEntityFromHandle(wearables[0]));

	if (wearable) {
		if (static_cast<std::uint32_t>(wearable->get_item_id_high()) != high_id || static_cast<std::uint32_t>(wearable->get_item_id_low()) != low_id) {
			wearable->SetDestroyedOnRecreateEntities();
			wearable->Release();
			wearable = nullptr;
		}
	}

	if (wearable)
		return;

	const auto entry = g_ent_list->GetHighestEntityIndex() + 1;
	const auto serial = rand() % 0x1000;

	static auto create_wearable_fn = get_wearable_create_fn();
	create_wearable_fn(entry, serial);
	wearable = static_cast<C_BaseCombatWeapon*>(g_ent_list->GetClientEntity(entry));

	if (wearable) {
		wearables[0] = entry | serial << 16;

		wearable->get_item_id_low() = low_id;
		wearable->get_item_id_high() = high_id;
		wearable->get_definition_index() = *soc_data->GetDefinitionIndex();
		wearable->get_account_id() = static_cast<int32_t>(info.steamID64);
		wearable->is_initialized() = true;
		wearable->set_model_index(g_mdl_info->get_model_index(static_data->get_world_display_model()));
		wearable->equip_wearable(local_player);
		local_player->get_n_body() = 1;
		wearable->initialize_attibutes();
	}
}

void changer::hud_icons(C_BasePlayer* local_player, CPlayerInventory* local_inventory, IGameEvent* event) {

	if (event->GetInt("attacker") != local_player->get_user_id())
		return;

	if (const auto weapon = std::string_view{ event->GetString("weapon") }; weapon != "knife" && weapon != "knife_t")
		return;

	const auto econ_item_view = local_inventory->get_item_in_loadout(static_cast<int32_t>(local_player->get_team()), NULL);

	if (!econ_item_view)
		return;

	const auto soc_data = econ_item_view->GetSOCData();

	if (!soc_data)
		return;

	if (const auto def = weaponids.m_list[*soc_data->GetDefinitionIndex()])
		if (const auto weapon_name = def->get_weapon_name(); weapon_name && std::string_view{ weapon_name }._Starts_with("weapon_"))
			event->SetString("weapon", weapon_name + 7);
}

void changer::on_event(IGameEvent* event) {

	const auto local_player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(g_engine->GetLocalPlayer()));

	if (!local_player)
		return;

	if (get_hash(event->GetName()) == utils::get_const("player_death")) 
		hud_icons(local_player, g_player_inventory, event);
}