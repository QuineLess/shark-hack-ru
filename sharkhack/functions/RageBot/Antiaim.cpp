#include "Antiaim.hpp"
#include "../../utils/math.h"
#include "../Exploits/Exploits.hpp"
#include "../Networking/Networking.hpp"

void C_AntiAim::Instance()
{
	int32_t nCommand = g_FuckThisSDK->m_pCmd->command_number;
	if (!config.m_bAntiAim)
	{
		return;
	}

	m_AntiAimConditions[nCommand % MULTIPLAYER_BACKUP] = this->GetAntiAimConditions();
	if (!m_AntiAimConditions[nCommand % MULTIPLAYER_BACKUP])
	{
		if (!g_FuckThisSDK->m_bFakeDuck)
			*g_FuckThisSDK->m_pbSendPacket = true;

		return;
	}

	float_t flFinalPitch = 0.0f;
	switch (config.m_iPitchMode)
	{
	case 1: flFinalPitch = 89.0f; break;
	case 2: flFinalPitch = -89.0f; break;
	case 3: flFinalPitch = -540.0f; break;
	case 4: flFinalPitch = 540.0f; break;
	default: break;
	}

	if (flFinalPitch)
		if (m_AntiAimConditions[nCommand % MULTIPLAYER_BACKUP] == ANTIAIM_FULL)
			g_FuckThisSDK->m_pCmd->viewangles.x = flFinalPitch;

	if (m_AntiAimConditions[nCommand % MULTIPLAYER_BACKUP])
	{
		float_t flFinalYaw = g_FuckThisSDK->m_pCmd->viewangles.y;
		if (m_AntiAimConditions[nCommand % MULTIPLAYER_BACKUP] == ANTIAIM_FULL)
		{
			flFinalYaw = g_FuckThisSDK->m_pCmd->viewangles.y + 180.0f;
			if (config.m_bAntiAimAtTargets)
			{
				m_iManualSide = 1337;
				flFinalYaw = this->GetTargetYaw();
			}
			else
			{
				if (m_iManualSide == 1337)
					m_iManualSide = 0;

				if (GetAsyncKeyState(config.manual_left_key))
					m_iManualSide = -1;

				if (GetAsyncKeyState(config.manual_back_key))
					m_iManualSide = 0;

				if (GetAsyncKeyState(config.manual_back_key))
					m_iManualSide = 1;

				flFinalYaw = math::AngleNormalize(flFinalYaw + (m_iManualSide < INT_MAX ? 90.0f * m_iManualSide : config.m_iYawAddOffset));
			}
		}

		if (*g_FuckThisSDK->m_pbSendPacket)
		{
			if (utils::key_bind(config.inverter_key, config.inverter_key_mode))
				m_iDesyncSide = 1;
			else
				m_iDesyncSide = -1;

			m_bShouldUseAlternativeSide = false;
			if (config.m_aInverterConditions[0] && (g_globals->GetLocalPlayer()->get_flags() & entity_flags::fl_on_ground) && g_globals->GetLocalPlayer()->get_velocity().Length2D() < 23.40f && g_globals->GetLocalPlayer()->get_velocity().z == 0.0f)
			{
				m_iFinalSide = -m_iFinalSide;
				m_bShouldUseAlternativeSide = true;
			}
			else if (g_globals->GetLocalPlayer()->get_velocity().Length2D() > 23.40f && g_globals->GetLocalPlayer()->get_velocity().z == 0.0f && (g_globals->GetLocalPlayer()->get_flags() & entity_flags::fl_on_ground))
			{
				if (config.m_aInverterConditions[1])
				{
					m_iFinalSide = -m_iFinalSide;
					m_bShouldUseAlternativeSide = true;
				}
			}
			else if (!(g_globals->GetLocalPlayer()->get_flags() & entity_flags::fl_on_ground))
			{
				if (config.m_aInverterConditions[2])
				{
					m_iFinalSide = -m_iFinalSide;
					m_bShouldUseAlternativeSide = true;
				}
			}

			if (!m_bShouldUseAlternativeSide)
				m_iFinalSide = m_iDesyncSide;

			if (m_AntiAimConditions[nCommand % MULTIPLAYER_BACKUP] == ANTIAIM_FULL)
			{
				if (m_bSwitch)
					flFinalYaw += config.m_iJitterAmount;
				else
					flFinalYaw -= config.m_iJitterAmount;
			}

			m_bSwitch = !m_bSwitch;
		}

		g_FuckThisSDK->m_pCmd->viewangles.y = math::AngleNormalize(flFinalYaw + config.m_iYawAddOffset);
	}

	int32_t iDesyncSide = m_iDesyncSide;
	if (m_AntiAimConditions[nCommand % MULTIPLAYER_BACKUP] == ANTIAIM_DESYNC)
		iDesyncSide = -iDesyncSide;

	if (m_bShouldUseAlternativeSide)
		iDesyncSide = m_iFinalSide;

	if (m_AntiAimConditions[nCommand % MULTIPLAYER_BACKUP] == ANTIAIM_DESYNC)
		if (m_AntiAimConditions[(nCommand - 1) % MULTIPLAYER_BACKUP] == ANTIAIM_FULL)
			return;

	if (!*g_FuckThisSDK->m_pbSendPacket)
	{
		float_t flDesyncAmount = 0.0f;
		if (m_iDesyncSide > 0)
			flDesyncAmount = config.m_iRightFakeLimit * 2.0f;
		else
			flDesyncAmount = config.m_iLeftFakeLimit * 2.0f;

		g_FuckThisSDK->m_pCmd->viewangles.y += flDesyncAmount * iDesyncSide;
	}
}

float C_AntiAim::GetTargetYaw()
{
	float_t flBestDistance = FLT_MAX;

	C_BasePlayer* pFinalPlayer = nullptr;
	for (int32_t i = 1; i < 65; i++)
	{
		C_BasePlayer* pPlayer = static_cast <C_BasePlayer*> (g_ent_list->GetClientEntity(i));
		if (!pPlayer || !pPlayer->is_player() || !pPlayer->is_alive() || pPlayer->get_team() == g_globals->GetLocalPlayer()->get_team() || pPlayer->IsDormant())
			continue;

		if (pPlayer->get_flags() & entity_flags::fl_frozen)
			continue;

		float_t flDistanceToPlayer = g_globals->GetLocalPlayer()->get_origin().DistTo(pPlayer->get_origin());
		if (flDistanceToPlayer > flBestDistance)
			continue;

		if (flDistanceToPlayer > 1250.0f)
			continue;

		flBestDistance = flDistanceToPlayer;
		pFinalPlayer = pPlayer;
	}

	if (!pFinalPlayer)
		return g_FuckThisSDK->m_pCmd->viewangles.y;

	return math::CalcAngle(g_globals->GetLocalPlayer()->get_eye_position(), g_globals->GetLocalPlayer()->get_abs_origin()).y + 180.0f;
}

void C_AntiAim::JitterMove()
{
	const float_t flSpeedPerTick = 5.0f / g_Networking->GetTickRate();
	if (!config.m_bJitterMove || (g_globals->GetLocalPlayer()->get_flags() & entity_flags::fl_frozen) || (g_globals->GetLocalPlayer()->get_gun_game_immunity()))
		return;

	if (config.m_bSlowWalk)
		return;

	const float_t flTargetVelocity = ((g_FuckThisSDK->m_pCmd->command_number % g_Networking->GetTickRate()) * flSpeedPerTick) + 95.0f;
	if (flTargetVelocity <= 100.0)
		if (flTargetVelocity >= 95.0)
			m_flAccelerationSpeed = flTargetVelocity;
		else
			m_flAccelerationSpeed = 100.0;

	const float_t flWishVelocity = (m_flAccelerationSpeed / 100.0) * g_globals->GetLocalPlayer()->GetMaxPlayerSpeed();
	if (flWishVelocity <= 0.0f)
		return;

	const float_t flMoveLength = Vector(g_FuckThisSDK->m_pCmd->forwardmove, g_FuckThisSDK->m_pCmd->sidemove, g_FuckThisSDK->m_pCmd->upmove).Length();
	if (flMoveLength < 10.0 || flMoveLength < flWishVelocity)
		return;

	g_FuckThisSDK->m_pCmd->forwardmove = (g_FuckThisSDK->m_pCmd->forwardmove / flMoveLength) * flWishVelocity;
	g_FuckThisSDK->m_pCmd->sidemove = (g_FuckThisSDK->m_pCmd->sidemove / flMoveLength) * flWishVelocity;
	g_FuckThisSDK->m_pCmd->upmove = (g_FuckThisSDK->m_pCmd->upmove / flMoveLength) * flWishVelocity;
}

void C_AntiAim::SlowWalk(bool bForcedWalk)
{
	bool bCanSlowwalk = true;
	if (!config.m_bSlowWalk)
		bCanSlowwalk = false;

	if ((g_globals->GetLocalPlayer()->get_flags() & entity_flags::fl_frozen) || (g_globals->GetLocalPlayer()->get_gun_game_immunity()))
		bCanSlowwalk = false;

	float_t flMovementLength = g_globals->GetLocalPlayer()->get_velocity().Length2D();
	if ((!bCanSlowwalk && !bForcedWalk) || !(g_globals->GetLocalPlayer()->get_flags() & entity_flags::fl_on_ground))
		return;

	float_t flMaxSpeed = g_globals->GetLocalPlayer()->GetMaxPlayerSpeed();
	if (m_bWalkSwitch || bForcedWalk)
		flMaxSpeed *= 0.033999f;
	else
		flMaxSpeed *= 0.022999f;

	m_bWalkSwitch = !m_bWalkSwitch;
	if (*g_FuckThisSDK->m_pbSendPacket && !bForcedWalk)
	{
		if (config.m_bJitterMove)
		{
			if (m_bMoveSwitch)
				g_FuckThisSDK->m_pCmd->buttons |= IN_SPEED;
			else
				g_FuckThisSDK->m_pCmd->buttons &= ~IN_SPEED;
		}

		m_bMoveSwitch = !m_bMoveSwitch;
	}

	float_t flDistanceToMinimalSpeed = flMovementLength / flMaxSpeed;
	if (flDistanceToMinimalSpeed <= 0.0f)
		return;

	g_FuckThisSDK->m_pCmd->forwardmove /= flDistanceToMinimalSpeed;
	g_FuckThisSDK->m_pCmd->sidemove /= flDistanceToMinimalSpeed;
}

void C_AntiAim::LegMovement()
{
	if (!config.m_bAntiAim || !((g_globals->GetLocalPlayer()->get_flags() & entity_flags::fl_on_ground)))
		return;

	int32_t Conditions = this->GetAntiAimConditions();
	if (Conditions != ANTIAIM_FULL)
		return;

	g_FuckThisSDK->m_pCmd->buttons &= ~(IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK);
	if (!config.m_iLegMovement)
	{
		if (g_FuckThisSDK->m_pCmd->forwardmove > 0.0f)
		{
			g_FuckThisSDK->m_pCmd->buttons &= ~IN_FORWARD;
			g_FuckThisSDK->m_pCmd->buttons |= IN_BACK;
		}
		else if (g_FuckThisSDK->m_pCmd->forwardmove < 0.0f)
		{
			g_FuckThisSDK->m_pCmd->buttons |= IN_FORWARD;
			g_FuckThisSDK->m_pCmd->buttons &= ~IN_BACK;
		}

		if (g_FuckThisSDK->m_pCmd->sidemove > 0.0f)
		{
			g_FuckThisSDK->m_pCmd->buttons &= ~IN_MOVERIGHT;
			g_FuckThisSDK->m_pCmd->buttons |= IN_MOVELEFT;
		}
		else if (g_FuckThisSDK->m_pCmd->sidemove < 0.0f)
		{
			g_FuckThisSDK->m_pCmd->buttons |= IN_MOVERIGHT;
			g_FuckThisSDK->m_pCmd->buttons &= ~IN_MOVELEFT;
		}
	}
	else
	{
		if (g_FuckThisSDK->m_pCmd->forwardmove > 0.0f)
		{
			g_FuckThisSDK->m_pCmd->buttons |= IN_FORWARD;
			g_FuckThisSDK->m_pCmd->buttons &= ~IN_BACK;
		}
		else if (g_FuckThisSDK->m_pCmd->forwardmove < 0.0f)
		{
			g_FuckThisSDK->m_pCmd->buttons &= ~IN_FORWARD;
			g_FuckThisSDK->m_pCmd->buttons |= IN_BACK;
		}

		if (g_FuckThisSDK->m_pCmd->sidemove > 0.0f)
		{
			g_FuckThisSDK->m_pCmd->buttons |= IN_MOVERIGHT;
			g_FuckThisSDK->m_pCmd->buttons &= ~IN_MOVELEFT;
		}
		else if (g_FuckThisSDK->m_pCmd->sidemove < 0.0f)
		{
			g_FuckThisSDK->m_pCmd->buttons &= ~IN_MOVERIGHT;
			g_FuckThisSDK->m_pCmd->buttons |= IN_MOVELEFT;
		}
	}
}

void C_AntiAim::Micromovement()
{
	if (!config.m_bAntiAim || g_FuckThisSDK->m_pCmd->buttons & (IN_MOVELEFT | IN_MOVERIGHT | IN_BACK | IN_FORWARD))
		return;

	float_t flVelocityLength = g_globals->GetLocalPlayer()->get_velocity().Length2D();
	if (flVelocityLength > 5.0f)
		return;

	float_t flSpeed = 1.1f;
	if ((g_FuckThisSDK->m_pCmd->buttons & IN_DUCK) || g_FuckThisSDK->m_bFakeDuck)
		flSpeed *= 3.3333334f;

	if (g_FuckThisSDK->m_pCmd->command_number & 1)
		flSpeed = -flSpeed;

	g_FuckThisSDK->m_pCmd->forwardmove = flSpeed;
}

int32_t C_AntiAim::GetAntiAimConditions()
{
	if (!config.m_bAntiAim)
		return 0;

	C_BaseCombatWeapon* pCombatWeapon = g_globals->GetLocalPlayer()->get_weapon_handle().Get();
	if (!pCombatWeapon)
		return 0;

	if ((g_globals->GetLocalPlayer()->get_flags() & entity_flags::fl_frozen))
		return 0;

	if (g_globals->GetLocalPlayer()->get_move_type() == move_type::move_type_ladder)
	{
		if (g_FuckThisSDK->m_pCmd->buttons & IN_BACK)
			return 0;

		if (g_FuckThisSDK->m_pCmd->buttons & IN_FORWARD)
			return 0;
	}

	if (g_globals->GetLocalPlayer()->get_move_type() == move_type::move_type_noclip)
		return 0;

	if (pCombatWeapon->IsGrenade())
	{
		if (pCombatWeapon->get_throw_time() > 0.0f && !pCombatWeapon->get_pin_pulled())
			return 0;
	}
	else  if (g_FuckThisSDK->m_pCmd->buttons & IN_ATTACK)
	{
		bool bIsRevolver = false;
		if (pCombatWeapon)
			if (pCombatWeapon->get_definition_index() == WEAPON_REVOLVER)
				bIsRevolver = true;

		int32_t nShiftAmount = g_ExploitSystem->GetShiftAmount();
		if (nShiftAmount > 9 || bIsRevolver)
			nShiftAmount = 0;

		if (g_globals->GetLocalPlayer()->CanFire(nShiftAmount, bIsRevolver))
			return 0;
	}
	else if (g_FuckThisSDK->m_pCmd->buttons & IN_ATTACK2)
		if (pCombatWeapon->IsKnife())
			return 0;

	if (g_FuckThisSDK->m_pCmd->buttons & IN_USE)
		return 2;

	return 1;
}