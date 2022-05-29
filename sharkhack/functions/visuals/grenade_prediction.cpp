#include "grenade_prediction.h"

std::vector<Vector> path;
std::vector<std::pair<Vector, QAngle>> other_collisions;

int type = 0;
int act = 0;

void trace_hull(Vector& src, Vector& end, trace_t& tr) {
	static const Vector hull[2] = { Vector(-2.0f, -2.0f, -2.0f), Vector(2.0f, 2.0f, 2.0f) };

	CTraceFilter filter;
	filter.SetIgnoreClass((char*)XORSTR("BaseCSGrenadeProjectile"));
	filter.pSkip = g_ent_list->GetClientEntity(g_engine->GetLocalPlayer());

	Ray_t ray;
	ray.Init(src, end, hull[0], hull[1]);

	g_engine_trace->TraceRay(ray, 0x200400B, &filter, &tr);
}

void setup(C_BasePlayer* local_player, Vector& vec_src, Vector& vec_throw, Vector viewangles) {
	Vector ang_throw = viewangles;
	float pitch = ang_throw.x;

	if (pitch <= 90.0f) {
		if (pitch < -90.0f) {
			pitch += 360.0f;
		}
	}
	else
		pitch -= 360.0f;

	float a = pitch - (90.0f - fabs(pitch)) * 10.0f / 90.0f;
	ang_throw.x = a;

	float vel = 750.0f * 0.9f;

	static const float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	float b = power[act];
	b = b * 0.7f;
	b = b + 0.3f;
	vel *= b;

	Vector forward;
	math::angle_vectors(ang_throw, &forward);

	vec_src = local_player->get_eye_position();
	float off = (power[act] * 12.0f) - 12.0f;
	vec_src.z += off;

	trace_t tr;
	Vector vec_dest = vec_src;
	vec_dest += forward * 22.0f;

	trace_hull(vec_src, vec_dest, tr);

	Vector vec_back = forward;
	vec_back *= 6.0f;
	vec_src = tr.endpos;
	vec_src -= vec_back;

	vec_throw = local_player->get_velocity();
	vec_throw *= 1.25f;
	vec_throw += forward * vel;
}

bool check_detonate(const Vector& vec_throw, const trace_t& trace, int tick, float interval) {
	switch (type) {
	case WEAPON_SMOKEGRENADE:
	case WEAPON_DECOY:
		if (vec_throw.Length2D() < 0.1f) {
			int det_tick_mod = static_cast<int>(0.2f / interval);
			return !(tick%det_tick_mod);
		}
		return false;
	case WEAPON_MOLOTOV:
	case WEAPON_INCGRENADE:
		if (trace.fraction != 1.0f && trace.plane.normal.z > 0.7f)
			return true;
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
		return static_cast<float>(tick)*interval > 1.5f && !(tick%static_cast<int>(0.2f / interval));
	default:
		assert(false);
		return false;
	}
}

void add_gravity_move(Vector& move, Vector& vel, float frametime, bool onground) {
	Vector basevel(0.0f, 0.0f, 0.0f);

	move.x = (vel.x + basevel.x) * frametime;
	move.y = (vel.y + basevel.y) * frametime;

	if (onground)
		move.z = (vel.z + basevel.z) * frametime;
	else {
		float gravity = g_cvar->FindVar(XORSTR("sv_gravity"))->GetFloat() * 0.4f;
		float newZ = vel.z - (gravity * frametime);

		move.z = ((vel.z + newZ) / 2.0f + basevel.z) * frametime;
		vel.z = newZ;
	}
}

void push_entity(Vector& src, const Vector& move, trace_t& tr) {
	Vector vec_abs_end = src;
	vec_abs_end += move;

	trace_hull(src, vec_abs_end, tr);
}

int physics_clip_velocity(const Vector& in, const Vector& normal, Vector& out, float overbounce) {
	static const float stop_epsilon = 0.1f;
	float    backoff;
	float    change;
	float    angle;
	int        i, blocked;

	blocked = 0;
	angle = normal[2];

	if (angle > 0)
		blocked |= 1;

	if (!angle)
		blocked |= 2;

	backoff = in.Dot(normal) * overbounce;

	for (i = 0; i < 3; i++) {
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -stop_epsilon && out[i] < stop_epsilon)
			out[i] = 0;
	}
	return blocked;
}

void resolve_fly_collision_custom(trace_t& trace, Vector& vec_velocity, float interval) {
	float surface_elasticity = 1.0;
	float grenade_elasticity = 0.45f;
	float total_elasticity = grenade_elasticity * surface_elasticity;

	if (total_elasticity > 0.9f)
		total_elasticity = 0.9f;

	if (total_elasticity < 0.0f)
		total_elasticity = 0.0f;

	Vector vec_abs_velocity;
	physics_clip_velocity(vec_velocity, trace.plane.normal, vec_abs_velocity, 2.0f);
	vec_abs_velocity *= total_elasticity;

	float speed_sqr = vec_abs_velocity.LengthSqr();
	static const float flMinSpeedSqr = 20.0f * 20.0f;

	if (speed_sqr < flMinSpeedSqr) {
		vec_abs_velocity.x = 0.0f;
		vec_abs_velocity.y = 0.0f;
		vec_abs_velocity.z = 0.0f;
	}

	if (trace.plane.normal.z > 0.7f) {
		vec_velocity = vec_abs_velocity;
		vec_abs_velocity *= ((1.0f - trace.fraction) * interval);
		push_entity(trace.endpos, vec_abs_velocity, trace);
	}
	else
		vec_velocity = vec_abs_velocity;
}

int step(Vector& vec_src, Vector& vec_throw, int tick, float interval) {

	Vector move;
	add_gravity_move(move, vec_throw, interval, false);

	trace_t trace;
	push_entity(vec_src, move, trace);

	int result = 0;

	if (check_detonate(vec_throw, trace, tick, interval))
		result |= 1;

	if (trace.fraction != 1.0f) {
		result |= 2;
		resolve_fly_collision_custom(trace, vec_throw, interval);
	}

	if ((result & 1) || vec_throw == Vector(0, 0, 0) || trace.fraction != 1.0f)
	{
		QAngle angles;
		math::vector_angles((trace.endpos - trace.startpos).Normalized(), angles);
		other_collisions.push_back(std::make_pair(trace.endpos, angles));
	}

	vec_src = trace.endpos;
	return result;
}

void simulate(C_BasePlayer* local_player, CViewSetup* p_setup) {
	Vector vec_src, vec_throw;
	Vector angles; 
	g_engine->GetViewAngles(&angles);

	setup(local_player, vec_src, vec_throw, angles);

	float interval = g_globals->interval_per_tick;

	int logstep = static_cast<int>(0.05f / interval);
	int logtimer = 0;

	path.clear();
	other_collisions.clear();

	for (unsigned int i = 0; i < path.max_size() - 1; ++i) {
		if (!logtimer)
			path.push_back(vec_src);

		int s = step(vec_src, vec_throw, i, interval);
		if ((s & 1)) break;

		if ((s & 2) || logtimer >= logstep) logtimer = 0;
		else ++logtimer;
	}
	path.push_back(vec_src);
}

void grenade_prediction::on_move(C_BasePlayer* local_player, int buttons) {

	if (!config.visuals_grenade_prediction)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player || !local_player->is_alive())
		return;

	bool in_attack = buttons & IN_ATTACK;
	bool in_attack2 = buttons & IN_ATTACK2;

	act = (in_attack && in_attack2) ? act_lob : (in_attack2) ? act_drop : (in_attack) ? act_throw : act_none;
}

void grenade_prediction::override_view(C_BasePlayer* local_player, CViewSetup* setup) {

	if (!config.visuals_grenade_prediction)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player || !local_player->is_alive())
		return;

	const auto weapon = reinterpret_cast<C_BaseCombatWeapon*>(g_ent_list->GetClientEntityFromHandle(local_player->get_weapon_handle()));

	if (weapon && get_hash(weaponids.m_list[weapon->get_definition_index()]->get_item_type_name()) == utils::get_const(XORSTR("#CSGO_Type_Grenade")) && act != act_none) {
		type = weapon->get_definition_index();
		simulate(local_player, setup);
	}
	else
		type = 0;
}

void grenade_prediction::on_draw(C_BasePlayer* local_player) {

	if (!config.visuals_grenade_prediction)
		return;

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	if (!local_player || !local_player->is_alive())
		return;

	if (type && path.size() > 1){
		Vector nade_start, nade_end;
		Vector prev = path[0];

		for (auto it = path.begin(), end = path.end(); it != end; ++it) {
			if (math::world_to_screen(prev, nade_start) && math::world_to_screen(*it, nade_end)) {
				render::render_line_float(nade_start.x, nade_start.y, nade_end.x, nade_end.y, Color::Black(), 2.5f);
				render::render_line_float(nade_start.x, nade_start.y, nade_end.x, nade_end.y, Color(config.color_visuals_grenade_prediction), 2.0f);
			}
			prev = *it;
		}
	}
}
