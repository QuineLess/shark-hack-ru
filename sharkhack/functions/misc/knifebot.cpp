#include "knifebot.h"

int best_index = -1;
float best_distance = -1;
C_BasePlayer* best_entity;

bool knifebot::get_best_target(C_BasePlayer* local_player) {
	float p_best_distance = 75;

	for (int i = 0; i < g_engine->GetMaxClients(); i++) {
		const auto entity = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(i));

		if (!entity)
			continue;

		if (entity == local_player)
			continue;

		if (entity->IsDormant() || entity->GetClientClass()->m_ClassID != classids::get(XORSTR("CCSPlayer")))
			continue;

		if (!entity->is_alive())
			continue;

		if (entity->is_teammate(local_player))
			continue;

		if (!local_player->is_visible(entity, HITBOX_CHEST))
			continue;

		float currect_distance = local_player->get_origin().DistTo(entity->get_origin());

		if (currect_distance < p_best_distance) {
			p_best_distance = currect_distance;
			best_index = i;
			best_entity = entity;
		}
	}

	best_distance = p_best_distance;

	return best_index != -1;
}

void knifebot::on_move(C_BasePlayer* local_player, CUserCmd* cmd) {

	if (!config.misc_knife_bot)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player || !local_player->is_alive())
		return;

	auto weapon = local_player->get_weapon_handle();

	if (!weapon)
		return;

	if (get_hash(weaponids.m_list[weapon->get_definition_index()]->get_item_type_name()) != utils::get_const(XORSTR("#CSGO_Type_Knife")))
		return;

	if (!get_best_target(local_player))
		return;

	best_index = -1;

	bool attack = false;

	Vector angles = math::calculate_angle(local_player->get_origin(), best_entity->get_origin());
	int health = best_entity->get_health();

	if (best_entity->get_armor()) {
		if (health <= 55 && health > 34)
			attack = true;
	}
	else {
		if (health <= 65 && health > 40)
			attack = true;
	}

	attack = attack && best_distance < 60;
	attack ? cmd->buttons |= IN_ATTACK2 : cmd->buttons |= IN_ATTACK;

	float server_time = local_player->get_tick_base() * g_globals->interval_per_tick;
	float next_shot = (attack ? weapon->get_next_secondary_attack() : weapon->get_next_primary_attack()) - server_time;

	if (!(next_shot > 0) && (cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2))
		cmd->viewangles = angles;
}

