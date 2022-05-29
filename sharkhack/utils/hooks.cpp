//utils
#include "hooks.h"
#include "entity.h"
#include "render.h"
#include "utils.h"
#include "../gui/gui.h"

//functions
#include "../functions/misc/misc.h"
#include "../functions/misc/knifebot.h"
#include "../functions/misc/fake_latency.h"
#include "../functions/visuals/esp.h"
#include "../functions/visuals/sound_esp.h"
#include "../functions/visuals/visuals.h"
#include "../functions/visuals/chams.h"
#include "../functions/visuals/glow.h"
#include "../functions/visuals/radar.h"
#include "../functions/visuals/grenade_prediction.h"
#include "../functions/changer/skins.h"
#include "../functions/aimbot/legitbot.h"
#include "../functions/anti_aim/legit_anti_aim.h"
#include "../functions/backtrack/backtrack.h"
#include "../functions/visuals/preview.h"
#include "../functions/lobby/lobby.h"

#include "../functions/Prediction/EnginePrediction.hpp"
#include "../functions/Networking/Networking.hpp"
#include "../functions/Exploits/Exploits.hpp"
#include "../functions/Networking/Fakelag.hpp"
#include "../functions/RageBot/RageBot.hpp"
#include "../functions/RageBot/Antiaim.hpp"
#include "../functions/RageBot/Autowall.hpp"
#include "../functions/Animations/LagCompensation.hpp"
#include "../functions/Animations/LocalAnimations.hpp"
#include "../functions/Animations/Animations.hpp"
#include "../min_hook/min_hook.h"

QAngle last_real;
uint64_t g_crate;

static auto calculateVmtLength( uintptr_t* vmt ) noexcept
{
	std::size_t length = 0;
	MEMORY_BASIC_INFORMATION memoryInfo;
	while ( VirtualQuery( LPCVOID( vmt[ length ] ), &memoryInfo, sizeof( memoryInfo ) ) && memoryInfo.Protect & ( PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY ) )
		length++;
	return length;
}

bool __fastcall hk_send_net_message( registers, INetMessage* message, bool force_reliable, bool voice ) {

	if ( !message )
		return o_send_net_message( ecx, edx, message, force_reliable, voice );

	if ( g_engine->IsInGame( ) )
	{
		switch ( message->get_type( ) )
		{
			case 9:
			voice = true;
			break;
			case 14:
			return false;
			break;
		}
	}

	return o_send_net_message( ecx, edx, message, force_reliable, voice );
}

int __fastcall hk_send_datagram( INetChannel* net_channel, void* edx, bf_write* datagram ) {

	if ( datagram )
		return o_send_datagram( net_channel, edx, datagram );

	int instate = net_channel->m_nInReliableState;
	int insequencenr = net_channel->m_nInSequenceNr;

	float ammount = ( float ) config.misc_fake_latency_value / 1000.f;

	fake_latency::add_latency_to_net_channel( net_channel, ammount );

	int ret = o_send_datagram( net_channel, edx, datagram );

	net_channel->m_nInReliableState = instate;
	net_channel->m_nInSequenceNr = insequencenr;

	return ret;
}


bool __fastcall hk_set_item_backpack_position( registers, CEconItemView* item, uint32_t position, bool force_unequip, bool allow_overflow ) {

	if ( item->get_inventory( ) == 0 )
	{
		item->get_inventory( ) = 1;
		*item->GetSOCData( )->GetInventory( ) = item->get_inventory( );
		g_player_inventory->so_updated( g_player_inventory->get_owner_id( ), ( c_shared_object* ) item->GetSOCData( ), 4 );
	}

	return o_set_item_backpack_position( ecx, edx, item, position, force_unequip, allow_overflow );
}

bool SetupPacket(int iSequence, bool* pbSendPacket)
{
	CUserCmd* m_pUserCmd = g_input->GetUserCmd(iSequence);
	if (!m_pUserCmd || !m_pUserCmd->command_number)
		return false;

	if (!g_globals->GetLocalPlayer()->is_alive())
		return false;

	g_FuckThisSDK->m_pbSendPacket = pbSendPacket;
	return true;
}

void __fastcall hk_create_move(registers, int sequence, float frametime, bool active, bool& bSendPacket) {

	o_create_move(ecx, edx, sequence, frametime, active);

	CUserCmd* cmd = g_input->GetUserCmd(sequence);

	const auto local_player = reinterpret_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(g_engine->GetLocalPlayer()));

	if (!local_player || !local_player->is_alive() || !SetupPacket(sequence, &bSendPacket))
		return 	o_create_move(ecx, edx, sequence, frametime, active);

	g_FuckThisSDK->m_pCmd = g_input->GetUserCmd(sequence);
	CVerifiedUserCmd* pVerifiedCmd = g_input->GetVerifiedCmd(sequence);

	g_ExploitSystem->SetupCommand();
	g_PredictionSystem->UpdatePacket();
	g_PredictionSystem->SaveNetvars(cmd->command_number);

	if (local_player->get_weapon_handle()->get_gun())
	{
		if (config.legit_autoselectweapon)
		{
			int weapon_settings_select = legitbot::get_weapon_select(local_player);
			if (weapon_settings_select >= 0)
				utils::cur_weapon = weapon_settings_select;
		}
	}

	misc::on_move(local_player, cmd);

	g_AntiAim->SlowWalk();
	g_AntiAim->JitterMove();
	g_AntiAim->Micromovement();
	g_AntiAim->LegMovement();

	g_RageBot->BackupPlayers();
	g_AutoWall->CacheWeaponData();

	g_RageBot->SetupPacket();
	if (!g_ExploitSystem->PerformCommand())
	{
		CVerifiedUserCmd* yamur = g_input->GetVerifiedCmd(sequence);
		if (yamur) {
			yamur->m_cmd = *cmd;
			yamur->m_crc = cmd->GetChecksum();
		}
		return;
	}

	g_RageBot->FakeDuck( );
	g_RageBot->SaveMovementData( );
	g_RageBot->PredictAutoStop( );

	g_PredictionSystem->Instance( ); 
	g_LocalAnimations->SetupShootPosition( );

	misc::clan_tag( );

	misc::edge_jump( local_player, cmd );

	backtrack::on_move( local_player, cmd );
	legitbot::on_move( local_player, cmd );
	knifebot::on_move( local_player, cmd );
	grenade_prediction::on_move( local_player, cmd->buttons );

	g_RageBot->UpdatePeekState( );
	g_FakeLag->Instance( );
	g_RageBot->Instance( );
	g_AntiAim->Instance( );
	g_ExploitSystem->BreakLagCompensation( );
	g_RageBot->ForceMovementData( );

	g_ExploitSystem->Instance( );

	visuals::sniper_crosshair( local_player );

//	if ( config.misc_fake_latency && g_client_state )
//	{
	//	auto net_channel = g_client_state->m_NetChannel;
	//	static bool hooked = false;
	//	if ( !hooked && net_channel )
	//	{
	//		hooked = true;
	//		MH_CreateHook( get_virtual( net_channel, 46 ), &hk_send_datagram, reinterpret_cast< void** >( &o_send_datagram ) );
	//		MH_EnableHook( get_virtual( net_channel, 46 ) );
	//	}
//	}

	g_RageBot->RestorePlayers( );

	math::normalize_angles( cmd->viewangles );
	math::clamp_angles( cmd->viewangles );
	math::FixMovement( cmd );

	pVerifiedCmd->m_cmd = *cmd;
	pVerifiedCmd->m_crc = cmd->GetChecksum();

	if ( *g_FuckThisSDK->m_pbSendPacket )
		g_Networking->PushCommand( cmd->command_number );
	else if ( !( g_client_state->chokedcommands % 4 ) )
		g_Networking->FinishNetwork( );
	g_LocalAnimations->Instance();

	g_PredictionSystem->ResetPacket( );
	return;
}

__declspec(naked) void __stdcall hk_create_move_proxy(int sequence, float frametime, bool active)
{
	__asm
	{
		push ebx
		push esp
		push dword ptr[esp + 20]
		push dword ptr[esp + 0Ch + 8]
		push dword ptr[esp + 10h + 4]
		call hk_create_move
		pop ebx
		retn 0Ch
	}
}
bool __fastcall hk_fire_event( registers, IGameEvent* event ) {
	const auto local_player = reinterpret_cast< C_BasePlayer* >( g_ent_list->GetClientEntity( g_engine->GetLocalPlayer( ) ) );

	changer::on_event( event );
	radar::load_map(utils::find_hud_element("CCSGO_MapOverview"));


	if ( get_hash( event->GetName( ) ) == utils::get_const( XORSTR( "server_spawn" ) ) )
	{
		if ( g_client_state->m_NetChannel )
		{
			MH_CreateHook( get_virtual( g_client_state->m_NetChannel, 40 ), &hk_send_net_message, reinterpret_cast< void** >( &o_send_net_message ) );
			MH_EnableHook( get_virtual( g_client_state->m_NetChannel, 40 ) );
		}
	}

	return o_fire_event( ecx, edx, event );
}

bool __fastcall hk_is_connected( registers ) {

	if ( config.misc_inventory_always && _ReturnAddress( ) == memory.inventory_always )
		return false;

	return o_is_connected( ecx, edx );
}

bool __fastcall hk_is_playing_demo( registers ) {

	if ( config.misc_money_reveal && *static_cast< uintptr_t* >( _ReturnAddress( ) ) == 0x0975C084 && **reinterpret_cast< uintptr_t** >( uintptr_t( _AddressOfReturnAddress( ) ) + 4 ) == 0x0C75C084 )
		return true;

	return o_is_playing_demo( ecx, edx );
}

bool __fastcall hk_broadcast_event( registers, uintptr_t event ) {

	if ( event == *reinterpret_cast< uintptr_t* >( memory.failed_to_open_crate ) )
	{
		if ( g_ui_engine_source2->BroadcastEvent( *reinterpret_cast< uintptr_t* >( memory.item_customization_notification ) ) )
		{
			const auto crate = g_player_inventory->GetInventoryItemByItemID( g_crate );
			const auto crate_series = crate->GetCrateSeries( );
			const auto items_to_drop = g_player_inventory->GetWeaponsForCrate( crate_series );

			std::vector<weapon_drop_info> blue_items;
			std::vector<weapon_drop_info> purple_items;
			std::vector<weapon_drop_info> pink_items;
			std::vector<weapon_drop_info> red_items;

			for ( auto item : items_to_drop )
			{
				if ( item.rarity <= 3 )
					blue_items.emplace_back( item );
				else if ( item.rarity == 4 )
					purple_items.emplace_back( item );
				else if ( item.rarity == 5 )
					pink_items.emplace_back( item );
				else if ( item.rarity == 6 )
					red_items.emplace_back( item );
			}

			const auto value = utils::random_float( 0.f, 100.f );

			auto dropped_item = weapon_drop_info( );

			if ( value <= 77.6f )
				dropped_item = blue_items[ rand( ) % blue_items.size( ) ];
			else if ( value <= 94.6f || pink_items.empty( ) && red_items.empty( ) )
				dropped_item = purple_items[ rand( ) % purple_items.size( ) ];
			else if ( value <= 98.6f || red_items.empty( ) )
				dropped_item = pink_items[ rand( ) % pink_items.size( ) ];
			else
				dropped_item = red_items[ rand( ) % red_items.size( ) ];

			const auto definition_index = dropped_item.item_definition;
			const auto paint_kit = dropped_item.paint_kit;
			const auto rarity = dropped_item.rarity;
			const auto wear = utils::random_float( 0.f, 1.f );

			const auto item = g_player_inventory->CreateEconItem( );

			*item->GetAccountID( ) = ( uint32_t ) g_player_inventory->get_owner_id( ).id;
			*item->GetDefinitionIndex( ) = definition_index;
			*item->GetItemID( ) = g_player_inventory->GetHighestItemID( ) + unique_id++;
			*item->GetInventory( ) = 0;
			*item->GetOriginalID( ) = 0;

			item->SetPaintKit( paint_kit );
			item->SetPaintWear( wear );
			item->SetOrigin( 8 );
			item->SetRarity( rarity );

			g_player_inventory->get_item_base_type_cache( )->add_object( item );
			g_player_inventory->so_created( g_player_inventory->get_owner_id( ), ( c_shared_object* ) item, 4 );
			
			const auto item_view = g_player_inventory->GetInventoryItemByItemID( *item->GetItemID( ) );
			item_view->ClearInventoryImageRgba( );
			config.inventory_item_list.insert( { *item->GetItemID( ), { definition_index, paint_kit, ( int ) wear, 0 } } );

			std::string crate_unlock = "$.DispatchEvent( 'PanoramaComponent_Inventory_ItemCustomizationNotification', '0','crate_unlock','" + std::to_string(*item->GetItemID()) + "');";
			utils::run_script(crate_unlock.c_str());

			const auto key = g_player_inventory->FindKeyToOpen( crate );

			const auto econ_data_crate = crate->GetSOCData( );
			g_player_inventory->RemoveItem( econ_data_crate );
			config.inventory_item_list.erase( *econ_data_crate->GetItemID( ) );

			if ( key )
			{
				const auto econ_data_key = key->GetSOCData( );
				g_player_inventory->RemoveItem( econ_data_key );
				config.inventory_item_list.erase( *econ_data_key->GetItemID( ) );
			}

			return false;
		}

	}

	return o_broadcast_event( ecx, edx, event );
}

bool __fastcall hk_is_using_static_prop_debug_modes( registers ) {
	return ( config.misc_asus_walls_enable || config.misc_world_custom_color_enable ) ? true : o_is_using_static_prop_debug_modes( ecx, edx );
}

float __fastcall hk_get_screen_aspect_ratio( registers, int width, int height ) {

	if ( config.misc_aspect_ratio )
		return config.misc_aspect_ratio_value;

	return o_get_screen_aspect_ratio( ecx, edx, width, height );
}

float __fastcall hk_view_model_fov( registers ) {
	float fov = o_view_model_fov( ecx, edx );
	const auto local_player = reinterpret_cast< C_BasePlayer* >( g_ent_list->GetClientEntity( g_engine->GetLocalPlayer( ) ) );

	misc::view_model_fov( local_player, fov );

	return fov;
}

float __fastcall hk_get_alpha_modulation( registers ) {

	if ( !config.misc_asus_walls_enable )
		return o_get_alpha_modulation( ecx, edx );

	if ( !g_engine->IsInGame( ) || !g_engine->IsConnected( ) )
		return o_get_alpha_modulation( ecx, edx );

	const auto get_material = reinterpret_cast< IMaterial* >( ecx );

	if ( !get_material || get_material->IsErrorMaterial( ) )
		return o_get_alpha_modulation( ecx, edx );

	const auto get_group = get_hash( get_material->GetTextureGroupName( ) );

	if ( get_group != utils::get_const( XORSTR( "World textures" ) ) && get_group != utils::get_const( XORSTR( "StaticProp textures" ) ) )
		return o_get_alpha_modulation( ecx, edx );

	if ( config.misc_asus_walls_value < 1.f )
		config.misc_asus_walls_value = 1.f;

	const auto value = config.misc_asus_walls_enable ? 1.f - 0.01f * config.misc_asus_walls_value : 1.f;

	return value;
}

int __fastcall hk_do_post_screen_effects( registers, int param ) {
	const auto local_player = reinterpret_cast< C_BasePlayer* >( g_ent_list->GetClientEntity( g_engine->GetLocalPlayer( ) ) );

	visuals::no_smoke( local_player );
	visuals::no_flash( local_player );
	visuals::dynamic_light( local_player );
	visuals::fog_controller( );
	glow::on_post_screen( local_player );

	return o_do_post_screen_effects( ecx, edx, param );
}
int __fastcall hk_set_viewmodel_offsets( registers, int something, float x, float y, float z ) {
	const auto local_player = reinterpret_cast< C_BasePlayer* >( g_ent_list->GetClientEntity( g_engine->GetLocalPlayer( ) ) );

	if ( !g_engine->IsInGame( ) || !g_engine->IsConnected( ) )
		return o_set_viewmodel_offsets( ecx, edx, something, x, y, z );

	if ( !local_player || !local_player->is_alive( ) )
		return o_set_viewmodel_offsets( ecx, edx, something, x, y, z );

	if ( local_player->is_scoped( ) )
		return o_set_viewmodel_offsets( ecx, edx, something, x, y, z );

	if ( config.misc_fov_offset )
	{
		x = config.misc_fov_viewmodel_x;
		y = config.misc_fov_viewmodel_y;
		z = config.misc_fov_viewmodel_z;
	}

	return o_set_viewmodel_offsets( ecx, edx, something, x, y, z );
}

void __fastcall hk_set_draw_color( registers, int r, int g, int b, int a ) {

	if ( config.misc_no_scope_border && ( _ReturnAddress( ) == memory.scope_arc || _ReturnAddress( ) == memory.scope_lens ) )
		a = 0;

	o_set_draw_color( ecx, edx, r, g, b, a );
}

void __fastcall hk_override_view( registers, CViewSetup* view ) {
	const auto local_player = reinterpret_cast< C_BasePlayer* >( g_ent_list->GetClientEntity( g_engine->GetLocalPlayer( ) ) );

	misc::override_view( local_player, view );
	misc::third_person(local_player);
	grenade_prediction::override_view( local_player, view );

	o_override_view( ecx, edx, view );
}

void __fastcall hk_lock_cursor( registers ) {

	if ( gui::menu_open )
	{
		g_surface->UnlockCursor( );
		return;
	}

	o_lock_cursor( ecx, edx );
}

void __fastcall hk_paint_traverse( registers, vgui::VPANEL panel, bool force_repaint, bool allow_force ) {
	static auto panel_id = vgui::VPANEL { 0 };
	const auto local_player = reinterpret_cast< C_BasePlayer* >( g_ent_list->GetClientEntity( g_engine->GetLocalPlayer( ) ) );

	o_paint_traverse( ecx, edx, panel, force_repaint, allow_force );

	lobby::on_init();
	misc::disable_post_processing();
	misc::get_match_cookie();


	if ( !panel_id )
	{
		const auto panel_name = g_panel->GetName( panel );
		if ( get_hash( panel_name ) == utils::get_const( XORSTR( "MatSystemTopPanel" ) ) ) // thanks zlatk0!
		{
			panel_id = panel;
		}
	}
	else if ( panel_id == panel )
	{
		static bool b_skip = false;
		b_skip = !b_skip;

		if ( b_skip )
			return;

		render::begin_scene( ); {
			visuals::draw_player_circle( local_player );
			player_esp::on_draw( local_player );
			world_esp::on_draw( local_player );
			preview::on_init( );
			sound_esp::on_draw( local_player );
			visuals::draw_spread_circle( local_player );
			visuals::draw_taser_range( local_player );
			visuals::draw_damage_indicator( local_player );
			visuals::draw_hit_marker( local_player );
			visuals::bomb_timer( );
			legitbot::on_draw( local_player );
			grenade_prediction::on_draw( local_player );
			legit_anti_aim::on_draw( local_player );
		}
		render::end_scene( );
	}
}

void __fastcall hk_suppress_lists( registers, int a2, bool a3 ) {
	const auto local_player = reinterpret_cast< C_BasePlayer* >( g_ent_list->GetClientEntity( g_engine->GetLocalPlayer( ) ) );

	if ( _ReturnAddress( ) == memory.frame_render_start )
	{
		visuals::night_mode( local_player );
		misc::ragdoll( );
		misc::thunder( );
		changer::apply_agents( );
		backtrack::frame_update( local_player );
	}

	if ( _ReturnAddress( ) == memory.frame_render_end )
	{
	}

	if ( _ReturnAddress( ) == memory.frame_net_update_start )
	{
	}

	if ( _ReturnAddress( ) == memory.frame_net_update_end )
	{
		sound_esp::frame_update( local_player );
	}

	o_suppress_lists( ecx, edx, a2, a3 );
}

void __fastcall hk_read_packet_entities( registers, void* ent_info ) {
	const auto local_player = reinterpret_cast< C_BasePlayer* >( g_ent_list->GetClientEntity( g_engine->GetLocalPlayer( ) ) );

	o_read_packet_entities( ecx, edx, ent_info );

	changer::apply_skins( );
	changer::apply_gloves( );
	changer::apply_other( );
	fake_latency::update_incoming_sequences( );
}

void __fastcall hk_draw_model_execute( registers, void* results, DrawModelInfo_t* info, matrix3x4_t* bone_to_world, float* flex_weights, float* flex_delayed_weights, Vector& model_origin, int flags ) {
	const auto local_player = reinterpret_cast< C_BasePlayer* >( g_ent_list->GetClientEntity( g_engine->GetLocalPlayer( ) ) );

	if ( g_studio_render->IsForcedMaterialOverride( ) )
		return o_draw_model_execute( ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags );

	chams::player( local_player, ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags );
	chams::arms( local_player, ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags );
	chams::weapon( local_player, ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags );

	if ( config.chams_enable && draw_model && strcmp( info->studiohdr->szName, "weapons\\w_rif_ak47.mdl" ) )
	{

		static float color_visible[ 3 ] = { 0.f, 1.f, 0.f };
		static float color_invisible[ 3 ] = { 1.f, 0.f, 0.f };
		static float color_more[ 3 ] = { 1.f, 0.3f, 0.f };

		if ( config.chams_type == chams_type::chams_type_flat )
		{
			if ( config.chams_invisible )
			{
				chams::get_material( material_flat, true, config.chams_wireframe, color_invisible[ 0 ], color_invisible[ 1 ], color_invisible[ 2 ], 1.f );
				o_draw_model_execute( ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags );
			}
			chams::get_material( material_flat, false, config.chams_wireframe, color_visible[ 0 ], color_visible[ 1 ], color_visible[ 2 ], 1.f );
		}
		else if ( config.chams_type == chams_type::chams_type_texture )
		{
			if ( config.chams_invisible )
			{
				chams::get_material( material_texture, true, config.chams_wireframe, color_invisible[ 0 ], color_invisible[ 1 ], color_invisible[ 2 ], 1.f );
				o_draw_model_execute( ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags );
			}
			chams::get_material( material_texture, false, config.chams_wireframe, color_visible[ 0 ], color_visible[ 1 ], color_visible[ 2 ], 1.f );
		}
		else if ( config.chams_type == chams_type::chams_type_animated )
		{
			if ( config.chams_invisible )
			{
				chams::get_material( material_animated, true, config.chams_wireframe, color_invisible[ 0 ], color_invisible[ 1 ], color_invisible[ 2 ], 1.f );
				o_draw_model_execute( ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags );
			}
			chams::get_material( material_animated, false, config.chams_wireframe, color_visible[ 0 ], color_visible[ 1 ], color_visible[ 2 ], 1.f );
		}
		else if ( config.chams_type == chams_type::chams_type_pearlescent )
		{
			if ( config.chams_invisible )
			{
				chams::get_material( material_pearlescent, true, config.chams_wireframe, color_invisible[ 0 ], color_invisible[ 1 ], color_invisible[ 2 ], 1.f );
				o_draw_model_execute( ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags );
			}
			chams::get_material( material_pearlescent, false, config.chams_wireframe, color_visible[ 0 ], color_visible[ 1 ], color_visible[ 2 ], 1.f );
			material_pearlescent->FindVar( XORSTR( "$pearlescent" ), nullptr, false )->SetValue( config.chams_pearlescent_value );
		}
		else if ( config.chams_type == chams_type::chams_type_ghost )
		{
			if ( config.chams_invisible )
			{
				chams::get_material( material_ghost, true, config.chams_wireframe, color_invisible[ 0 ], color_invisible[ 1 ], color_invisible[ 2 ], 1.f );
				o_draw_model_execute( ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags );
			}
			chams::get_material( material_ghost, false, config.chams_wireframe, color_visible[ 0 ], color_visible[ 1 ], color_visible[ 2 ], 1.f );
		}
		o_draw_model_execute( ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags );
		if ( config.chams_more )
		{
			if ( config.chams_more_type == chams_more_type::chams_more_type_glow - 1 )
				material = material_glow;
			else if ( config.chams_more_type == chams_more_type::chams_more_type_glow_overlay - 1 )
			{
				material = material_glow_overlay;
				material->FindVar( XORSTR( "$envmapfresnelminmaxexp" ), nullptr, false )->SetVecComponentValue( 3.8f, 2 );
			}
			else if ( config.chams_more_type == chams_more_type::chams_more_type_glow_pulse - 1 )
			{
				material = material_glow_overlay;
				material->FindVar( XORSTR( "$envmapfresnelminmaxexp" ), nullptr, false )->SetVecComponentValue( 9.0f * ( 1.2f - ( 0.5f * std::sin( g_globals->curtime * 5.0f ) * 0.5f + 0.5f ) ), 2 );
			}
			else if ( config.chams_more_type == chams_more_type::chams_more_type_gloss - 1 )
				material = material_gloss;
			else if ( config.chams_more_type == chams_more_type::chams_more_type_crystal - 1 )
				material = material_crystal;
			else if ( config.chams_more_type == chams_more_type::chams_more_type_glass - 1 )
				material = material_glass;

			material->FindVar( XORSTR( "$envmaptint" ), nullptr, false )->SetVectorValue( color_more[ 0 ], color_more[ 1 ], color_more[ 2 ] );
			g_studio_render->ForcedMaterialOverride( material );
			o_draw_model_execute( ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags );
		}
	}

	o_draw_model_execute( ecx, edx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags );

	g_studio_render->ForcedMaterialOverride( nullptr );
}

void __fastcall hk_emit_sound( registers, void* filter, int ent_index, int channel, const char* entry, unsigned int entry_hash, const char* sample, float volume, int seed, float attenuation, int flags, int pitch, const Vector* origin, const Vector* direction, void* vec_origins, bool update_pos, float soundtime, int speakerentity, int unk ) {

	if (config.misc_auto_accept && !strcmp(entry, "UIPanorama.popup_accept_match_beep"))
		utils::run_script("$.DispatchEvent( \"MatchAssistedAccept\" );");

	o_emit_sound( ecx, edx, filter, ent_index, channel, entry, entry_hash, sample, volume, seed, attenuation, flags, pitch, origin, direction, vec_origins, update_pos, soundtime, speakerentity, unk );
}

int32_t __fastcall hkListLeavesInBox(LPVOID pEcx, uint32_t, Vector& vecMin, Vector& vecMax, unsigned short* pList, int32_t iMax)
{
	if (*(uint32_t*)(_ReturnAddress()) != 0x14244489)
		return g_FuckThisSDK->m_Originals.m_ListLeavesInBox(pEcx, vecMin, vecMax, pList, iMax);

	renderable_info_t* pRenderableInfo = *(renderable_info_t**)((uintptr_t)_AddressOfReturnAddress() + 0x14);
	if (!pRenderableInfo || !pRenderableInfo->renderable)
		return g_FuckThisSDK->m_Originals.m_ListLeavesInBox(pEcx, vecMin, vecMax, pList, iMax);

	C_BaseEntity* pBaseEntity = pRenderableInfo->renderable->GetIClientUnknown()->GetBaseEntity();
	if (!pBaseEntity || !pBaseEntity->is_player())
		return g_FuckThisSDK->m_Originals.m_ListLeavesInBox(pEcx, vecMin, vecMax, pList, iMax);

	pRenderableInfo->flags[0] &= ~0x100;
	pRenderableInfo->flags[1] |= 0xC0;

	return g_FuckThisSDK->m_Originals.m_ListLeavesInBox(pEcx, Vector(-16384.0f, -16384.0f, -16384.0f), Vector(16384.0f, 16384.0f, 16384.0f), pList, iMax);
}

void __fastcall hkUpdateClientSideAnimation( LPVOID pEcx, uint32_t )
{
	C_BasePlayer* pPlayer = ( C_BasePlayer* ) ( pEcx );
	if ( !pPlayer || !pPlayer->is_player( ) || pPlayer->EntIndex( ) - 1 > 63 || !pPlayer->is_alive( ) || !g_globals->GetLocalPlayer( ) )
		return g_FuckThisSDK->m_Originals.m_UpdateClientSideAnimation( pEcx );

	if ( pPlayer->get_team( ) == g_globals->GetLocalPlayer( )->get_team( ) )
	{
		if ( pPlayer != g_globals->GetLocalPlayer( ) )
			return g_FuckThisSDK->m_Originals.m_UpdateClientSideAnimation( pEcx );
	}
	else
		pPlayer->set_abs_origin( pPlayer->get_origin( ) );

	if ( !g_FuckThisSDK->m_AnimationData.m_bAnimationUpdate )
	{
		if ( pPlayer == g_globals->GetLocalPlayer( ) )
			return g_LocalAnimations->OnUpdateClientSideAnimation( );

		return g_AnimationSync->OnUpdateClientSideAnimation( ( C_BasePlayer* ) ( pEcx ) );
	}

	return g_FuckThisSDK->m_Originals.m_UpdateClientSideAnimation( pEcx );
}

bool __fastcall hkSetupBones( LPVOID pEcx, uint32_t, matrix3x4_t* aMatrix, int32_t iMaxBones, int32_t iBoneMask, float_t flCurrentTime )
{
	C_BasePlayer* pPlayer = ( C_BasePlayer* ) ( ( uintptr_t ) ( pEcx ) - 0x4 );
	if ( !pPlayer || !pPlayer->is_player( ) || pPlayer->EntIndex( ) - 1 > 63 || !pPlayer->is_alive( ) || !g_globals->GetLocalPlayer( ) && config.legit_enable)
		return g_FuckThisSDK->m_Originals.m_SetupBones( pEcx, aMatrix, iMaxBones, iBoneMask, flCurrentTime );

	if ( pPlayer->get_team( ) == g_globals->GetLocalPlayer( )->get_team( ) )
		if ( pPlayer != g_globals->GetLocalPlayer( ) )
			return g_FuckThisSDK->m_Originals.m_SetupBones( pEcx, aMatrix, iMaxBones, iBoneMask, flCurrentTime );

	if ( g_FuckThisSDK->m_AnimationData.m_bSetupBones )
		return g_FuckThisSDK->m_Originals.m_SetupBones( pEcx, aMatrix, iMaxBones, iBoneMask, flCurrentTime );
	else if ( aMatrix )
	{
		if ( pPlayer == g_globals->GetLocalPlayer( ) )
			return g_LocalAnimations->GetCachedMatrix( aMatrix );
		else
			return g_AnimationSync->GetCachedMatrix( pPlayer, aMatrix );
	}

	return true;
}

class C_CommandContext
{
public:
	bool m_bNeedsProcessing;
	CUserCmd m_Cmd;
	int m_nCommandNumber;
};

void __fastcall hkPhysicsSimulate( LPVOID pEcx, uint32_t )
{
	if ( !g_globals->GetLocalPlayer( ) || !g_globals->GetLocalPlayer( )->is_alive( ) || pEcx != g_globals->GetLocalPlayer( ) )
		return g_FuckThisSDK->m_Originals.m_PhysicsSimulate( pEcx );

	int32_t iSimulationTick = *( int32_t* ) ( ( uintptr_t ) ( pEcx ) + 0x2AC );
	if ( iSimulationTick == g_globals->tickcount )
		return;

	C_CommandContext* pCommandContext = reinterpret_cast < C_CommandContext* >( ( uintptr_t ) ( g_globals->GetLocalPlayer( ) ) + 0x350C );
	if ( !pCommandContext || !pCommandContext->m_bNeedsProcessing )
		return;

	// fix the tickbase
	g_globals->GetLocalPlayer( )->get_tick_base( ) = g_ExploitSystem->GetNetworkTickbase( pCommandContext->m_nCommandNumber );

	// call the orig func
	g_FuckThisSDK->m_Originals.m_PhysicsSimulate( pEcx );

	// store viewmodel
	g_PredictionSystem->SaveViewmodelData( );

	// save netvar data
	return g_Networking->SaveNetvarData( pCommandContext->m_nCommandNumber );
}

void __vectorcall hkCL_Move( float_t flFrametime, bool bIsFinalTick )
{
	g_Networking->UpdateLatency( );
	g_Networking->StartNetwork( );

	if ( !g_ExploitSystem->IsAllowedToRun( ) )
		return;

	g_FuckThisSDK->m_Originals.m_CL_Move( flFrametime, bIsFinalTick );
	return g_ExploitSystem->PerformPackets( );
}

void __fastcall hkDoExtraBoneProcessing( LPVOID pEcx, uint32_t, CStudioHdr* pHdr, Vector* vecVector, Quaternion* pSomething, matrix3x4_t* aMatrix, LPVOID aList, LPVOID pContext )
{
	return;
}

void __fastcall hkPacketStart( LPVOID pEcx, uint32_t, int32_t iSequence, int32_t iCommand )
{
	if ( g_Networking->ShouldProcessPacketStart( iCommand ) )
		return g_FuckThisSDK->m_Originals.m_PacketStart( pEcx, iSequence, iCommand );
}

void __fastcall hkPacketEnd( LPVOID pEcx, uint32_t )
{
	g_Networking->OnPacketEnd( ( CClientState* ) ( pEcx ) );
	return g_FuckThisSDK->m_Originals.m_PacketEnd( pEcx );
}

void __fastcall hk_get_color_modulation( registers, float& r, float& g, float& b ) {
	o_get_color_modulation( ecx, edx, r, g, b );

	if ( !config.misc_world_custom_color_enable )
		return;

	if ( !g_engine->IsInGame( ) || !g_engine->IsConnected( ) )
		return;

	const auto get_material = reinterpret_cast< IMaterial* >( ecx );

	if ( !get_material || get_material->IsErrorMaterial( ) )
		return;

	const auto get_group = get_hash( get_material->GetTextureGroupName( ) );

	if ( get_group != utils::get_const( XORSTR( "World textures" ) ) && get_group != utils::get_const( XORSTR( "StaticProp textures" ) ) )
		return;

	if ( get_group == utils::get_const( XORSTR( "World textures" ) ) )
		r *= config.color_misc_world[ 0 ], g *= config.color_misc_world[ 1 ], b *= config.color_misc_world[ 2 ];
	else if ( get_group == utils::get_const( XORSTR( "StaticProp textures" ) ) )
		r *= config.color_misc_static_props[ 0 ], g *= config.color_misc_static_props[ 1 ], b *= config.color_misc_static_props[ 2 ];
}

const char* __fastcall hk_unknown_call( registers, int a1, int a2 ) {
	const auto ret = o_unknown_call( ecx, edx, a1, a2 );

	if ( _ReturnAddress( ) == reinterpret_cast< void* >( memory.unknown_call_ret_addr ) )
		g_crate = std::atoll( ret );

	return ret;
}

EGCResult __fastcall hk_retrieve_msg( registers, uint32_t* pun_msg_type, void* pub_dest, uint32_t cub_dest, uint32_t* cub_msg_size )
{
	static auto write_packet = [ ] ( std::string packet, void* this_ptr, void* old_ebp, void* pub_dest, uint32_t cub_dest, uint32_t* cub_msg_size ) {
		if ( ( uint32_t ) packet.size( ) <= cub_dest - 8 )
		{
			memcpy( ( void* ) ( ( DWORD ) pub_dest + 8 ), ( void* ) packet.data( ), packet.size( ) );
			*cub_msg_size = packet.size( ) + 8;
		}
		else if ( g_mem_alloc )
		{
			auto mem_ptr = *reinterpret_cast< void** >( ( uint32_t ) this_ptr + ( uint32_t ) 0x18 );
			auto mem_ptr_size = *reinterpret_cast< uint32_t* >( ( uint32_t ) this_ptr + ( uint32_t ) 0x1C );
			auto new_size = ( mem_ptr_size - cub_dest ) + packet.size( ) + 8;

			auto memory = g_mem_alloc->Realloc( mem_ptr, new_size + 4 );

			*reinterpret_cast< void** >( ( uint32_t ) this_ptr + ( uint32_t ) 0x18 ) = memory;
			*reinterpret_cast< uint32_t* >( ( uint32_t ) this_ptr + ( uint32_t ) 0x1C ) = new_size;
			*reinterpret_cast< void** >( ( uint32_t ) old_ebp + ( uint32_t ) -0x14 ) = memory;

			memcpy( reinterpret_cast< void* >( ( uint32_t ) memory + ( uint32_t ) 0x1C ), ( void* ) packet.data( ), packet.size( ) );

			*cub_msg_size = packet.size( ) + 8;
		}
	};

	auto status = o_retrieve_msg( ecx, edx, pun_msg_type, pub_dest, cub_dest, cub_msg_size );

	if ( status == k_EGCResultOK )
	{
		void* this_ptr = nullptr;
		__asm mov this_ptr, ebx;
		auto old_ebp = *reinterpret_cast< void** >( ( uint32_t ) _AddressOfReturnAddress( ) - 4 );

		uint32_t messageType = *pun_msg_type & 0x7FFFFFFF;

		if ( messageType == k_EMsgGCCStrike15_v2_MatchmakingGC2ClientHello )
		{
			writer msg( ( void* ) ( ( DWORD ) pub_dest + 8 ), *cub_msg_size - 8, 19 );

			auto _commendation = msg.has( MatchmakingGC2ClientHello::commendation ) ? msg.get( MatchmakingGC2ClientHello::commendation ).String( ) : std::string( "" );
			writer commendation( _commendation, 4 );
			if ( config.profile_friendly > 0 )
				commendation.replace( field( PlayerCommendationInfo::cmd_friendly, TYPE_UINT32, ( int64_t ) config.profile_friendly ) );
			if ( config.profile_teaching > 0 )
				commendation.replace( field( PlayerCommendationInfo::cmd_teaching, TYPE_UINT32, ( int64_t ) config.profile_teaching ) );
			if ( config.profile_leader > 0 )
				commendation.replace( field( PlayerCommendationInfo::cmd_leader, TYPE_UINT32, ( int64_t ) config.profile_leader ) );
			msg.replace( field( MatchmakingGC2ClientHello::commendation, TYPE_STRING, commendation.serialize( ) ) );

			auto _ranking = msg.has( MatchmakingGC2ClientHello::ranking ) ? msg.get( MatchmakingGC2ClientHello::ranking ).String( ) : std::string( "" );
			writer ranking( _ranking, 6 );
			if ( config.profile_rank_id > 0 )
				ranking.replace( field( PlayerRankingInfo::rank_id, TYPE_UINT32, ( int64_t ) config.profile_rank_id ) );
			if ( config.profile_wins > 0 )
				ranking.replace( field( PlayerRankingInfo::wins, TYPE_UINT32, ( int64_t ) config.profile_wins ) );
			msg.replace( field( MatchmakingGC2ClientHello::ranking, TYPE_STRING, ranking.serialize( ) ) );

			if ( config.profile_player_level > 0 )
				msg.replace( field( MatchmakingGC2ClientHello::player_level, TYPE_INT32, ( int64_t ) config.profile_player_level ) );
			if ( config.profile_player_level_xp > 0 )
				msg.replace( field( MatchmakingGC2ClientHello::player_xp, TYPE_INT32, ( int64_t ) config.profile_player_level_xp ) );

			if ( config.profile_player_ban != 0 && config.profile_player_ban_time != 0 )
			{
				if ( config.profile_player_ban == 1 )
					msg.replace( field( MatchmakingGC2ClientHello::penalty_reason, TYPE_INT32, ( int64_t ) 2 ) );
				else if ( config.profile_player_ban == 2 )
					msg.replace( field( MatchmakingGC2ClientHello::penalty_reason, TYPE_INT32, ( int64_t ) 8 ) );
				else if ( config.profile_player_ban == 3 )
					msg.replace( field( MatchmakingGC2ClientHello::penalty_reason, TYPE_INT32, ( int64_t ) 12 ) );

				msg.replace( field( MatchmakingGC2ClientHello::penalty_seconds, TYPE_INT32, ( int64_t ) config.profile_player_ban_time ) );
			}

			auto packet = msg.serialize( );
			write_packet( packet, this_ptr,

						  old_ebp, pub_dest, cub_dest, cub_msg_size );
		}
		else if ( messageType == k_EMsgGCCStrike15_v2_ClientGCRankUpdate )
		{
			writer msg( ( void* ) ( ( DWORD ) pub_dest + 8 ), *cub_msg_size - 8, 19 );

			auto _ranking = msg.has( ClientGCRankUpdate::ranking ) ? msg.get( ClientGCRankUpdate::ranking ).String( ) : std::string( "" );
			writer ranking( _ranking, 6 );

			switch ( ranking.get( PlayerRankingInfo::rank_type_id ).Int32( ) )
			{
				case 7: {
					if ( config.profile_rank_id_wingmans > 0 )
						ranking.replace( field( PlayerRankingInfo::rank_id, TYPE_UINT32, ( int64_t ) config.profile_rank_id_wingmans ) );
					if ( config.profile_wins_wingmans > 0 )
						ranking.replace( field( PlayerRankingInfo::wins, TYPE_UINT32, ( int64_t ) config.profile_wins_wingmans ) );
					msg.replace( field( ClientGCRankUpdate::ranking, TYPE_STRING, ranking.serialize( ) ) );
					break;
				}
				case 10: {
					if ( config.profile_rank_id_dz > 0 )
						ranking.replace( field( PlayerRankingInfo::rank_id, TYPE_UINT32, ( int64_t ) config.profile_rank_id_dz ) );
					if ( config.profile_wins_dz > 0 )
						ranking.replace( field( PlayerRankingInfo::wins, TYPE_UINT32, ( int64_t ) config.profile_wins_dz ) );
					msg.replace( field( ClientGCRankUpdate::ranking, TYPE_STRING, ranking.serialize( ) ) );
					break;
				}
			}

			auto packet = msg.serialize( );
			write_packet( packet, this_ptr, old_ebp, pub_dest, cub_dest, cub_msg_size );
		}
	}

	return status;
}

void* __stdcall hk_read_packet( ) {

	if ( !config.misc_overwatch_reveal )
		return o_read_packet( );

	g_demo_player->set_overwatch_state( true );

	auto ret = o_read_packet( );

	g_demo_player->set_overwatch_state( false );

	return ret;
}

void __stdcall hk_hud_update( bool active ) {

	if ( config.misc_overwatch_reveal && g_demo_player->is_playing_demo( ) )
	{
		g_demo_player->set_overwatch_state( true );
		o_hud_update( active );
		g_demo_player->set_overwatch_state( false );
	}
	else
		o_hud_update( active );
}

long __stdcall hk_reset( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* presentation_parameters ) {

	ImGui_ImplDX9_InvalidateDeviceObjects( );

	auto result = o_reset( device, presentation_parameters );

	if ( result >= 0 )
		ImGui_ImplDX9_CreateDeviceObjects( );

	return result;
}

long __stdcall hk_end_scene( IDirect3DDevice9* device ) {
	const auto local_player = reinterpret_cast< C_BasePlayer* >( g_ent_list->GetClientEntity( g_engine->GetLocalPlayer( ) ) );

	static uintptr_t gameoverlay_return_address;
	auto return_address = reinterpret_cast< uintptr_t >( _ReturnAddress( ) );

	if ( config.settings_obs_spoof )
	{
		if ( !gameoverlay_return_address )
		{
			static MEMORY_BASIC_INFORMATION module_info;
			static char file_name[ MAX_PATH ];

			VirtualQuery( _ReturnAddress( ), &module_info, sizeof( MEMORY_BASIC_INFORMATION ) );
			GetModuleFileName( reinterpret_cast< HMODULE >( module_info.AllocationBase ), file_name, MAX_PATH );

			if ( strstr( file_name, "gameoverlay" ) )
				gameoverlay_return_address = return_address;
		}

		if ( gameoverlay_return_address != return_address )
			return o_end_scene( device );
	}

	ImGui_ImplDX9_NewFrame( );
	ImGui_ImplWin32_NewFrame( );
	ImGui::NewFrame( );

	gui::render( );
	radar::render( local_player );
	misc::spectator_list( );

	ImGui::Render( render::render_scene( ) );
	ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );

	return o_end_scene( device );
}

int __stdcall hk_unknown_file_sys(void* image) {
	DWORD this_pointer;
	__asm mov this_pointer, ebx;

	if (image && _ReturnAddress() == memory.unknown_file_sys_ret_addr && *(DWORD*)image == 542327876) {
		auto size = *(DWORD*)(this_pointer + 0x50);
		D3DXCreateTextureFromFileInMemory(g_device, image, size, &radar::map_image);
	}

	return o_unknown_file_sys(image);
}

#define FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MIN 4.0f
#define FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MAX 10.0f

void __fastcall hkModifyEyePosition( LPVOID pEcx, uint32_t, Vector& vecInputEyePos )
{
	C_CSGOPlayerAnimationState* m_AnimationState = ( C_CSGOPlayerAnimationState* ) ( pEcx );
	if ( !m_AnimationState->m_pBasePlayer )
		return;

	if ( !m_AnimationState->m_bLanding && m_AnimationState->m_flAnimDuckAmount == 0.0f )
	{
		m_AnimationState->m_bSmoothHeightValid = false;
		return;
	}

	Vector vecHeadPos = Vector
	(
		m_AnimationState->m_pBasePlayer->m_CachedBoneData( )[ 8 ][ 0 ][ 3 ],
		m_AnimationState->m_pBasePlayer->m_CachedBoneData( )[ 8 ][ 1 ][ 3 ],
		m_AnimationState->m_pBasePlayer->m_CachedBoneData( )[ 8 ][ 2 ][ 3 ] + 1.7f
	);

	if ( vecHeadPos.z > vecInputEyePos.z )
		return;

	float_t flLerp = math::RemapValClamped( abs( vecInputEyePos.z - vecHeadPos.z ),
											FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MIN,
											FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MAX,
											0.0f, 1.0f );

	vecInputEyePos.z = math::Lerp( flLerp, vecInputEyePos.z, vecHeadPos.z );
}

enum ClientFrameStage_t : int
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END
};

void __fastcall hkProcessMovement( LPVOID pEcx, uint32_t, C_BasePlayer* pPlayer, CMoveData* pMoveData )
{
	return g_FuckThisSDK->m_Originals.m_ProcessMovement( pEcx, pPlayer, pMoveData );
}

void __fastcall hkFrameStageNotify( LPVOID pEcx, uint32_t, int nStage )
{
	if ( !g_globals->GetLocalPlayer( ) )
	{
		g_LagCompensation->ResetData( );
		g_Networking->ResetData( );
		g_LocalAnimations->ResetData( );
		g_PredictionSystem->ResetData( );
		g_RageBot->ResetData( );
		g_FakeLag->ResetData( );

		return g_FuckThisSDK->m_Originals.m_FrameStageNotify( pEcx, nStage );
	}

	if ( nStage == FRAME_NET_UPDATE_END )
	{
		g_LagCompensation->Instance( );
		g_AnimationSync->Instance( );
		g_RageBot->OnNetworkUpdate( );
	}

	if ( nStage == FRAME_RENDER_START )
		g_Networking->ProcessInterpolation( false );

	g_FuckThisSDK->m_Originals.m_FrameStageNotify( pEcx, nStage );

	if ( nStage == FRAME_RENDER_START )
		g_Networking->ProcessInterpolation( true );

	if ( nStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START )
		g_PredictionSystem->AdjustViewmodelData( );
}

bool __fastcall hkIsHLTV( LPVOID pEcx, uint32_t )
{
	static auto m_SetupVelocity_Call = ( void* )( utils::find_signature( "client.dll", "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0" ) );
	if ( reinterpret_cast < DWORD > ( _ReturnAddress( ) ) == reinterpret_cast < DWORD > ( m_SetupVelocity_Call ) )
		return true;

	static auto m_AccumulateLayers_Call = ( void* ) ( utils::find_signature( "client.dll", "84 C0 75 0D F6 87" ) );
	if ( reinterpret_cast < DWORD > ( _ReturnAddress( ) ) == reinterpret_cast < DWORD > ( m_AccumulateLayers_Call ) )
		return true;

	return g_FuckThisSDK->m_Originals.m_IsHLTV( pEcx );
}

bool __fastcall hkIsPaused( LPVOID pEcx, uint32_t )
{
	static auto m_IsPaused_Call = ( void* ) ( utils::find_signature( "client.dll", "0F B6 0D ? ? ? ? 84 C0 0F 44" ) );
	if ( reinterpret_cast < DWORD > ( _ReturnAddress( ) ) == reinterpret_cast < DWORD > ( m_IsPaused_Call ) )
		return true;

	return g_FuckThisSDK->m_Originals.m_IsPaused( pEcx );
}

bool __fastcall hkInPrediction( LPVOID pEcx, uint32_t )
{
	static auto m_InPrediction_Call = ( void* ) ( utils::find_signature( "client.dll", "84 C0 74 0A F3 0F 10 05 ? ? ? ? EB 05" ) );
	if ( reinterpret_cast < DWORD >( _ReturnAddress( ) ) == reinterpret_cast < DWORD >( m_InPrediction_Call ) )
		return false;

	return g_FuckThisSDK->m_Originals.m_InPrediction( pEcx );
}
static void WriteUsercmd( bf_write* pBuffer, CUserCmd* pToCmd, CUserCmd* pFromCmd )
{
	static auto WriteCmd = ( void* )( utils::find_signature( "client.dll", "55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D" ) );
	__asm
	{
		mov     ecx, pBuffer
		mov     edx, pToCmd
		push    pFromCmd
		call    WriteCmd
		add     esp, 4
	}
}

bool __fastcall hkWriteUsercmdDeltaToBuffer( LPVOID pEcx, uint32_t, int32_t iSlot, bf_write* pBuffer, int32_t iFrom, int32_t iTo, bool bNewCmd )
{
	if ( g_ExploitSystem->GetForcedShiftAmount( ) < 1 || g_ExploitSystem->GetShiftMode( ) != MODE::SHIFT_BUFFER )
		return g_FuckThisSDK->m_Originals.m_WriteUsercmdDeltaToBuffer( pEcx, iSlot, pBuffer, iFrom, iTo, bNewCmd );

	int32_t iExtraCommands = g_ExploitSystem->GetForcedShiftAmount( );
	g_ExploitSystem->ResetShiftAmount( );

	int32_t* pNumBackupCommands = ( int32_t* ) ( ( uintptr_t ) ( pBuffer ) - 0x30 );
	int32_t* pNumNewCommands = ( int32_t* ) ( ( uintptr_t ) ( pBuffer ) - 0x2C );

	int32_t iNewCommands = *pNumNewCommands;
	int32_t iNextCommand = g_client_state->chokedcommands + g_client_state->m_nLastOutgoingCommand + 1;

	*pNumBackupCommands = 0;

	for ( iTo = iNextCommand - iNewCommands + 1; iTo <= iNextCommand; iTo++ )
	{
		if ( !g_FuckThisSDK->m_Originals.m_WriteUsercmdDeltaToBuffer( pEcx, iSlot, pBuffer, iFrom, iTo, true ) )
			return false;

		iFrom = iTo;
	}

	*pNumNewCommands = iNewCommands + iExtraCommands;

	CUserCmd* pCmd = g_input->GetUserCmd( iSlot, iFrom );
	if ( !pCmd )
		return true;

	CUserCmd ToCmd = *pCmd;
	CUserCmd FromCmd = *pCmd;

	ToCmd.command_number++;
	ToCmd.tick_count += 200;

	for ( int32_t i = 0; i < iExtraCommands; i++ )
	{
		WriteUsercmd( pBuffer, &ToCmd, &FromCmd );

		FromCmd = ToCmd;

		ToCmd.tick_count++;
		ToCmd.command_number++;
	}

	return true;
}
void hooks::on_init( ) {

	LPVOID pFrameStageNotify			= ( void* ) utils::find_signature( "client.dll", ( "55 8B EC 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0 A2" ) );
	LPVOID pPacketStart					= ( void* ) ( ( DWORD )( utils::find_signature( "engine.dll", ( "56 8B F1 E8 ? ? ? ? 8B 8E ? ? ? ? 3B 8E ? ? ? ?" ) ) ) - 0x20 );
	LPVOID pPacketEnd					= ( void* ) utils::find_signature( "engine.dll", ( "56 8B F1 E8 ? ? ? ? 8B 8E ? ? ? ? 3B 8E ? ? ? ?" ) );
	LPVOID pModifyEyePosition			= ( void* ) utils::find_signature( "client.dll", ( "55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 14 83 7F 60" ) );
	LPVOID pDoExtraBoneProcessing		= ( void* ) utils::find_signature( "client.dll", ( "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C" ) );
	LPVOID pSetupBones					= ( void* ) utils::find_signature( "client.dll", ( "55 8B EC 83 E4 F0 B8 D8" ) );
	LPVOID pUpdateClientSideAnimation	= ( void* ) utils::find_signature( "client.dll", ( "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74" ) );
	LPVOID pPhysicsSimulate				= ( void* ) utils::find_signature( "client.dll", ( "56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 23" ) );
	LPVOID pCLMove						= ( void* ) utils::find_signature( "engine.dll", ( "55 8B EC 81 EC 64 01 00 00 53 56 8A F9" ) );

	MH_Initialize( );



	const char* fart[]{ "client.dll", "engine.dll", "server.dll", "studiorender.dll", "materialsystem.dll", "shaderapidx9.dll", "vstdlib.dll", "vguimatsurface.dll" };
	long long amongus = 0x69690004C201B0;
	for (auto sex : fart) WriteProcessMemory(GetCurrentProcess(), (LPVOID)utils::find_signature(sex, "55 8B EC 56 8B F1 33 C0 57 8B 7D 08"), &amongus, 8, 0);



	MH_CreateHook( pFrameStageNotify, &hkFrameStageNotify, reinterpret_cast < void** > ( &g_FuckThisSDK->m_Originals.m_FrameStageNotify ) );
	MH_CreateHook( pPacketStart, &hkPacketStart, reinterpret_cast < void** > ( &g_FuckThisSDK->m_Originals.m_PacketStart ) );
	MH_CreateHook( pPacketEnd, &hkPacketEnd, reinterpret_cast < void** > ( &g_FuckThisSDK->m_Originals.m_PacketEnd ) );
	MH_CreateHook( pModifyEyePosition, &hkModifyEyePosition, reinterpret_cast < void** > ( &g_FuckThisSDK->m_Originals.m_ModifyEyePosition ) );
	MH_CreateHook( pDoExtraBoneProcessing, &hkDoExtraBoneProcessing, reinterpret_cast < void** > ( &g_FuckThisSDK->m_Originals.m_DoExtraBoneProcessing ) );
	MH_CreateHook( pSetupBones, &hkSetupBones, reinterpret_cast < void** > ( &g_FuckThisSDK->m_Originals.m_SetupBones ) );
	MH_CreateHook( pUpdateClientSideAnimation, &hkUpdateClientSideAnimation, reinterpret_cast < void** > ( &g_FuckThisSDK->m_Originals.m_UpdateClientSideAnimation ) );
	MH_CreateHook( pCLMove, &hkCL_Move, reinterpret_cast < void** > ( &g_FuckThisSDK->m_Originals.m_CL_Move ) );
	MH_CreateHook( pPhysicsSimulate, &hkPhysicsSimulate, reinterpret_cast < void** > ( &g_FuckThisSDK->m_Originals.m_PhysicsSimulate ) );
	MH_CreateHook(get_virtual(g_engine->get_bsp_tree_query(), 6), &hkListLeavesInBox, reinterpret_cast <void**> (&g_FuckThisSDK->m_Originals.m_ListLeavesInBox));

	MH_CreateHook( get_virtual( g_demo_player, 17 ), &hk_read_packet, reinterpret_cast< void** >( &o_read_packet ) );
	MH_CreateHook( get_virtual( g_game_movement, 1 ), &hkProcessMovement, reinterpret_cast< void** >( &g_FuckThisSDK->m_Originals.m_ProcessMovement ) );
	MH_CreateHook( get_virtual( g_prediction, 14 ), &hkInPrediction, reinterpret_cast< void** >( &g_FuckThisSDK->m_Originals.m_InPrediction ) );
	MH_CreateHook( get_virtual( g_engine, 90 ), &hkIsPaused, reinterpret_cast< void** >( &g_FuckThisSDK->m_Originals.m_IsPaused ) );
	MH_CreateHook( get_virtual( g_engine, 93 ), &hkIsHLTV, reinterpret_cast< void** >( &g_FuckThisSDK->m_Originals.m_IsHLTV ) );

	MH_CreateHook(get_virtual(g_file_sys, 89), &hk_unknown_file_sys, reinterpret_cast<void**>(&o_unknown_file_sys));

	MH_CreateHook( get_virtual( g_client, 11 ), &hk_hud_update, reinterpret_cast< void** >( &o_hud_update ) );
	MH_CreateHook(get_virtual(g_client, 22), &hk_create_move_proxy, reinterpret_cast<void**>(&o_create_move));
	MH_CreateHook( get_virtual( g_client, 24 ), &hkWriteUsercmdDeltaToBuffer, reinterpret_cast< void** >( &g_FuckThisSDK->m_Originals.m_WriteUsercmdDeltaToBuffer ) );

	MH_CreateHook( get_virtual( g_panel, 41 ), &hk_paint_traverse, reinterpret_cast< void** >( &o_paint_traverse ) );

	MH_CreateHook( get_virtual( g_game_events, 9 ), &hk_fire_event, reinterpret_cast< void** >( &o_fire_event ) );

	MH_CreateHook( get_virtual( g_panorama_marshall_helper, 7 ), &hk_unknown_call, reinterpret_cast< void** >( &o_unknown_call ) );

	MH_CreateHook( get_virtual( g_ui_engine_source2, 54 ), &hk_broadcast_event, reinterpret_cast< void** >( &o_broadcast_event ) );

	MH_CreateHook( get_virtual( g_steam_game_coordinator, 2 ), &hk_retrieve_msg, reinterpret_cast< void** >( &o_retrieve_msg ) );

	MH_CreateHook( get_virtual( g_spatial_partition, 16 ), &hk_suppress_lists, reinterpret_cast< void** >( &o_suppress_lists ) );

	MH_CreateHook( get_virtual( g_sound, 5 ), &hk_emit_sound, reinterpret_cast< void** >( &o_emit_sound ) );

	MH_CreateHook( get_virtual( g_studio_render, 29 ), &hk_draw_model_execute, reinterpret_cast< void** >( &o_draw_model_execute ) );

	MH_CreateHook( get_virtual( g_inventory_manager, 26 ), &hk_set_item_backpack_position, reinterpret_cast< void** >( &o_set_item_backpack_position ) );

	MH_CreateHook( get_virtual( memory.material, 44 ), &hk_get_alpha_modulation, reinterpret_cast< void** >( &o_get_alpha_modulation ) );
	MH_CreateHook( get_virtual( memory.material, 45 ), &hk_get_color_modulation, reinterpret_cast< void** >( &o_get_color_modulation ) );

	MH_CreateHook( get_virtual( reinterpret_cast< void* >( reinterpret_cast< int >( g_client_state ) + 0x8 ), 64 ), &hk_read_packet_entities, reinterpret_cast< void** >( &o_read_packet_entities ) );

	MH_CreateHook( get_virtual( g_engine, 27 ), &hk_is_connected, reinterpret_cast< void** >( &o_is_connected ) );
	MH_CreateHook( get_virtual( g_engine, 82 ), &hk_is_playing_demo, reinterpret_cast< void** >( &o_is_playing_demo ) );
	MH_CreateHook( get_virtual( g_engine, 101 ), &hk_get_screen_aspect_ratio, reinterpret_cast< void** >( &o_get_screen_aspect_ratio ) );

	MH_CreateHook( get_virtual( g_surface, 15 ), &hk_set_draw_color, reinterpret_cast< void** >( &o_set_draw_color ) );
	MH_CreateHook( get_virtual( g_surface, 67 ), &hk_lock_cursor, reinterpret_cast< void** >( &o_lock_cursor ) );

	MH_CreateHook( get_virtual( g_client_mode, 18 ), &hk_override_view, reinterpret_cast< void** >( &o_override_view ) );
	MH_CreateHook( get_virtual( g_client_mode, 35 ), &hk_view_model_fov, reinterpret_cast< void** >( &o_view_model_fov ) );
	MH_CreateHook( get_virtual( g_client_mode, 44 ), &hk_do_post_screen_effects, reinterpret_cast< void** >( &o_do_post_screen_effects ) );

	MH_CreateHook( get_virtual( g_device, 16 ), &hk_reset, reinterpret_cast< void** >( &o_reset ) );
	MH_CreateHook( get_virtual( g_device, 42 ), &hk_end_scene, reinterpret_cast< void** >( &o_end_scene ) );

	MH_CreateHook( memory.set_viewmodel_offsets, &hk_set_viewmodel_offsets, reinterpret_cast< void** >( &o_set_viewmodel_offsets ) );
	MH_CreateHook( memory.is_using_static_prop_debug_modes, &hk_is_using_static_prop_debug_modes, reinterpret_cast< void** >( &o_is_using_static_prop_debug_modes ) );

	MH_EnableHook( MH_ALL_HOOKS );
}