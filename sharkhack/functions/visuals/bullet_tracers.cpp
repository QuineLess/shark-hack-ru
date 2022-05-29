#include"bullet_tracers.h"

void bullet_tracers::on_events(IGameEvent* event) {

	if (!config.visuals_bullet_tracers)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	auto index = g_engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));

	if (index != g_engine->GetLocalPlayer())
		return;

	const auto local_player = static_cast<C_BasePlayer*>(g_ent_list->GetClientEntity(index));

	if (!local_player)
		return;

	Vector position(event->GetFloat(XORSTR("x")), event->GetFloat(XORSTR("y")), event->GetFloat(XORSTR("z")));

	Ray_t ray;
	trace_t trace;
	CTraceFilter filter;

	Vector eye_position = local_player->get_eye_position();

	ray.Init(eye_position, position);
	filter.pSkip = local_player;

	g_engine_trace->TraceRay(ray, MASK_SHOT, &filter, &trace);

	logs.push_back(trace_info(eye_position, position, g_globals->curtime, Color::White()));

	for (size_t i = 0; i < logs.size(); i++) {
		auto current = logs.at(i);

		current.color = Color(config.color_visuals_bullet_tracers);
		BeamInfo_t beam_info;

		beam_info.m_nType = TE_BEAMPOINTS;
		beam_info.m_pszModelName = XORSTR("sprites/physbeam.vmt");
		beam_info.m_nModelIndex = -1;
		beam_info.m_flHaloScale = 0.0f;
		beam_info.m_flLife = config.visuals_bullet_tracers_life;
		beam_info.m_flWidth = config.visuals_bullet_tracers_width;
		beam_info.m_flEndWidth = config.visuals_bullet_tracers_width;
		beam_info.m_flFadeLength = 0.0f;
		beam_info.m_flAmplitude = 2.0f;
		beam_info.m_flBrightness = 255.f;
		beam_info.m_flSpeed = 0.2f;
		beam_info.m_nStartFrame = 0;
		beam_info.m_flFrameRate = 0.f;
		beam_info.m_flRed = (float)current.color.r();
		beam_info.m_flGreen = (float)current.color.g();
		beam_info.m_flBlue = (float)current.color.b();
		beam_info.m_nSegments = 2;
		beam_info.m_bRenderable = true;
		beam_info.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;
		beam_info.m_vecStart = eye_position;
		beam_info.m_vecEnd = current.dst;

		Beam_t* beam = g_view_render_beams->CreateBeamPoints(beam_info);

		if (beam)
			g_view_render_beams->DrawBeam(beam);

		logs.erase(logs.begin() + i);
	}
}

