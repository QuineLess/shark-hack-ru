#include "legit_anti_aim.h"
#include "../../utils/render.h"

float next_lby = 0.0f;
float side = 1.0f;

void legit_anti_aim::update_lby( C_CSGOPlayerAnimationState* anim_state )
{
	if ( anim_state->m_flVelocityLengthXY > 0.1f || std::fabsf( anim_state->m_flVelocityLengthZ ) )
	{
		next_lby = g_globals->curtime + 0.22f;
	}
	else if ( g_globals->curtime > next_lby )
	{
		if ( std::fabsf( math::angle_diff( anim_state->m_flFootYaw, anim_state->m_flEyeYaw ) ) > 35.0f )
		{
			next_lby = g_globals->curtime + 1.1f;
		}
	}
}

void legit_anti_aim::on_move( C_BasePlayer* local_player, QAngle old_angles, CUserCmd* cmd, bool* send_packet ) {

	if ( !g_engine->IsInGame( ) || !g_engine->IsConnected( ) )
		return;

	if ( !local_player || !local_player->is_alive( ) )
		return;

	if ( cmd->buttons & ( IN_ATTACK | IN_ATTACK2 | IN_USE ) )
		return;

	if ( local_player->get_move_type( ) == move_type::move_type_ladder || local_player->get_move_type( ) == move_type::move_type_noclip || !local_player->is_alive( ) )
		return;

	auto weapon = local_player->get_weapon_handle( ).Get( );

	if ( !weapon )
		return;

	auto weapon_index = weapon->get_definition_index( );

	if ( ( weapon_index == WEAPON_GLOCK || weapon_index == WEAPON_FAMAS ) && weapon->get_next_primary_attack( ) >= g_globals->curtime )
		return;

	auto weapon_data = weapon->get_weapon_data( );

	if ( weapon_data->m_iWeaponType == WEAPONTYPE_GRENADE )
	{
		if ( !weapon->get_pin_pulled( ) )
		{
			float throwTime = weapon->get_throw_time( );
			if ( throwTime > 0.f )
				return;
		}

		if ( ( cmd->buttons & IN_ATTACK ) || ( cmd->buttons & IN_ATTACK2 ) )
		{
			if ( weapon->get_throw_time( ) > 0.f )
				return;
		}
	}

	static bool broke_lby = false;

	if ( utils::is_key_pressed( config.legit_desync_key ) )
	{
		side = -side;
	}


	float minimal_move = 2.0f;
	if ( local_player->get_flags( ) & fl_ducking )
		minimal_move *= 3.f;

	if ( cmd->buttons & IN_WALK )
		minimal_move *= 3.f;

	bool should_move = local_player->get_velocity( ).Length2D( ) <= 0.0f || std::fabsf( local_player->get_velocity( ).z ) <= 100.0f;

	if ( ( cmd->command_number % 2 ) == 1 )
	{
		cmd->viewangles.y += 120.0f * side;
		if ( should_move )
			cmd->sidemove -= minimal_move;
		*send_packet = false;
	}
	else if ( should_move )
	{
		cmd->sidemove += minimal_move;
	}

	math::normalize_angles( cmd->viewangles );
	math::clamp_angles( cmd->viewangles );
	math::fix_movement( cmd, old_angles );
};

void legit_anti_aim::on_draw( C_BasePlayer* local_player ) {

	/*if (!config.legit_desync)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player || !local_player->is_alive())
		return;

	render::render_text(side > 0.0f ? "<" : ">", side > 0.0f ? int(ImGui::GetIO().DisplaySize.x / 2) - 50 : int(ImGui::GetIO().DisplaySize.x / 2) + 50, int(ImGui::GetIO().DisplaySize.y / 2) - 2, Color::White(), g_visuals_font, 20.f, text_effect::text_outline);*/
}