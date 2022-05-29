#include "math.h"
#include "../imgui/imgui.h"

static const float invtwopi = 0.1591549f;
static const float twopi = 6.283185f;
static const float threehalfpi = 4.7123889f;
static const float pi = 3.141593f;
static const float halfpi = 1.570796f;
static const __m128 signmask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));

static const __declspec(align(16)) float null[4] = { 0.f, 0.f, 0.f, 0.f };
static const __declspec(align(16)) float _pi2[4] = { 1.5707963267948966192f, 1.5707963267948966192f, 1.5707963267948966192f, 1.5707963267948966192f };
static const __declspec(align(16)) float _pi[4] = { 3.141592653589793238f, 3.141592653589793238f, 3.141592653589793238f, 3.141592653589793238f };

typedef __declspec(align(16)) union
{
	float f[4];
	__m128 v;
} m128;

__forceinline __m128 sqrt_ps(const __m128 squared)
{
	return _mm_sqrt_ps(squared);
}
void math::FixMovement( void* pCmd )
{
	CUserCmd* pUserCmd = static_cast < CUserCmd* > ( pCmd );
	if ( !pUserCmd || !pUserCmd->command_number )
		return;

	Vector view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	auto angViewAngles = pUserCmd->viewangles;
	angViewAngles.Normalize( );

	Vector wish;
	g_engine->GetViewAngles( &wish );

	angle_vectors( wish, view_fwd, view_right, view_up );
	angle_vectors( angViewAngles, cmd_fwd, cmd_right, cmd_up );

	const float v8 = sqrtf( ( view_fwd.x * view_fwd.x ) + ( view_fwd.y * view_fwd.y ) );
	const float v10 = sqrtf( ( view_right.x * view_right.x ) + ( view_right.y * view_right.y ) );
	const float v12 = sqrtf( view_up.z * view_up.z );

	const Vector norm_view_fwd( ( 1.f / v8 ) * view_fwd.x, ( 1.f / v8 ) * view_fwd.y, 0.f );
	const Vector norm_view_right( ( 1.f / v10 ) * view_right.x, ( 1.f / v10 ) * view_right.y, 0.f );
	const Vector norm_view_up( 0.f, 0.f, ( 1.f / v12 ) * view_up.z );

	const float v14 = sqrtf( ( cmd_fwd.x * cmd_fwd.x ) + ( cmd_fwd.y * cmd_fwd.y ) );
	const float v16 = sqrtf( ( cmd_right.x * cmd_right.x ) + ( cmd_right.y * cmd_right.y ) );
	const float v18 = sqrtf( cmd_up.z * cmd_up.z );

	const Vector norm_cmd_fwd( ( 1.f / v14 ) * cmd_fwd.x, ( 1.f / v14 ) * cmd_fwd.y, 0.f );
	const Vector norm_cmd_right( ( 1.f / v16 ) * cmd_right.x, ( 1.f / v16 ) * cmd_right.y, 0.f );
	const Vector norm_cmd_up( 0.f, 0.f, ( 1.f / v18 ) * cmd_up.z );

	const float v22 = norm_view_fwd.x * pUserCmd->forwardmove;
	const float v26 = norm_view_fwd.y * pUserCmd->forwardmove;
	const float v28 = norm_view_fwd.z * pUserCmd->forwardmove;
	const float v24 = norm_view_right.x * pUserCmd->sidemove;
	const float v23 = norm_view_right.y * pUserCmd->sidemove;
	const float v25 = norm_view_right.z * pUserCmd->sidemove;
	const float v30 = norm_view_up.x * pUserCmd->upmove;
	const float v27 = norm_view_up.z * pUserCmd->upmove;
	const float v29 = norm_view_up.y * pUserCmd->upmove;

	pUserCmd->forwardmove = ( ( ( ( norm_cmd_fwd.x * v24 ) + ( norm_cmd_fwd.y * v23 ) ) + ( norm_cmd_fwd.z * v25 ) )
								  + ( ( ( norm_cmd_fwd.x * v22 ) + ( norm_cmd_fwd.y * v26 ) ) + ( norm_cmd_fwd.z * v28 ) ) )
		+ ( ( ( norm_cmd_fwd.y * v30 ) + ( norm_cmd_fwd.x * v29 ) ) + ( norm_cmd_fwd.z * v27 ) );
	pUserCmd->sidemove = ( ( ( ( norm_cmd_right.x * v24 ) + ( norm_cmd_right.y * v23 ) ) + ( norm_cmd_right.z * v25 ) )
							   + ( ( ( norm_cmd_right.x * v22 ) + ( norm_cmd_right.y * v26 ) ) + ( norm_cmd_right.z * v28 ) ) )
		+ ( ( ( norm_cmd_right.x * v29 ) + ( norm_cmd_right.y * v30 ) ) + ( norm_cmd_right.z * v27 ) );
	pUserCmd->upmove = ( ( ( ( norm_cmd_up.x * v23 ) + ( norm_cmd_up.y * v24 ) ) + ( norm_cmd_up.z * v25 ) )
							 + ( ( ( norm_cmd_up.x * v26 ) + ( norm_cmd_up.y * v22 ) ) + ( norm_cmd_up.z * v28 ) ) )
		+ ( ( ( norm_cmd_up.x * v30 ) + ( norm_cmd_up.y * v29 ) ) + ( norm_cmd_up.z * v27 ) );

	pUserCmd->forwardmove = std::clamp( pUserCmd->forwardmove, -450.f, 450.f );
	pUserCmd->sidemove = std::clamp( pUserCmd->sidemove, -450.f, 450.f );
	pUserCmd->upmove = std::clamp( pUserCmd->upmove, -320.f, 320.f );
}
__forceinline __m128 cos_52s_ps(const __m128 x)
{
	const auto c1 = _mm_set1_ps(0.9999932946f);
	const auto c2 = _mm_set1_ps(-0.4999124376f);
	const auto c3 = _mm_set1_ps(0.0414877472f);
	const auto c4 = _mm_set1_ps(-0.0012712095f);
	const auto x2 = _mm_mul_ps(x, x);
	return _mm_add_ps(c1, _mm_mul_ps(x2, _mm_add_ps(c2, _mm_mul_ps(x2, _mm_add_ps(c3, _mm_mul_ps(c4, x2))))));
}

__forceinline void sincos_ps(__m128 angle, __m128 *sin, __m128 *cos) {
	const auto anglesign = _mm_or_ps(_mm_set1_ps(1.f), _mm_and_ps(signmask, angle));
	angle = _mm_andnot_ps(signmask, angle);
	angle = _mm_sub_ps(angle, _mm_mul_ps(_mm_cvtepi32_ps(_mm_cvttps_epi32(_mm_mul_ps(angle, _mm_set1_ps(invtwopi)))), _mm_set1_ps(twopi)));

	auto cosangle = angle;
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(halfpi)), _mm_xor_ps(cosangle, _mm_sub_ps(_mm_set1_ps(pi), angle))));
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(pi)), signmask));
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(threehalfpi)), _mm_xor_ps(cosangle, _mm_sub_ps(_mm_set1_ps(twopi), angle))));

	auto result = cos_52s_ps(cosangle);
	result = _mm_xor_ps(result, _mm_and_ps(_mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(halfpi)), _mm_cmplt_ps(angle, _mm_set1_ps(threehalfpi))), signmask));
	*cos = result;

	const auto sinmultiplier = _mm_mul_ps(anglesign, _mm_or_ps(_mm_set1_ps(1.f), _mm_and_ps(_mm_cmpgt_ps(angle, _mm_set1_ps(pi)), signmask)));
	*sin = _mm_mul_ps(sinmultiplier, sqrt_ps(_mm_sub_ps(_mm_set1_ps(1.f), _mm_mul_ps(result, result))));
}


bool math::screen_transform(const Vector& in, Vector& out) {
	static auto& w2s_matrix = g_engine->WorldToScreenMatrix();

	out.x = w2s_matrix.m[0][0] * in.x + w2s_matrix.m[0][1] * in.y + w2s_matrix.m[0][2] * in.z + w2s_matrix.m[0][3];
	out.y = w2s_matrix.m[1][0] * in.x + w2s_matrix.m[1][1] * in.y + w2s_matrix.m[1][2] * in.z + w2s_matrix.m[1][3];
	out.z = 0.0f;

	float w = w2s_matrix.m[3][0] * in.x + w2s_matrix.m[3][1] * in.y + w2s_matrix.m[3][2] * in.z + w2s_matrix.m[3][3];

	if (w < 0.001f) {
		out.x *= 100000;
		out.y *= 100000;
		return false;
	}

	out.x /= w;
	out.y /= w;

	return true;
}

bool math::world_to_screen(const Vector& in, Vector& out) {
	if (screen_transform(in, out)) {
		Vector2D screen_size{ ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y };

		out.x = (screen_size.x / 2.0f) + (out.x * screen_size.x) / 2.0f;
		out.y = (screen_size.y / 2.0f) - (out.y * screen_size.y) / 2.0f;

		return true;
	}
	return false;
}

void math::vector_transform(const Vector& in1, const matrix3x4_t& in2, Vector& out) {
	out[0] = in1.Dot(in2[0]) + in2[0][3];
	out[1] = in1.Dot(in2[1]) + in2[1][3];
	out[2] = in1.Dot(in2[2]) + in2[2][3];
}

void math::i_transform(const Vector& in1, const matrix3x4_t& in2, Vector& out) {
	out.x = (in1.x - in2[0][3]) * in2[0][0] + (in1.y - in2[1][3]) * in2[1][0] + (in1.z - in2[2][3]) * in2[2][0];
	out.y = (in1.x - in2[0][3]) * in2[0][1] + (in1.y - in2[1][3]) * in2[1][1] + (in1.z - in2[2][3]) * in2[2][1];
	out.z = (in1.x - in2[0][3]) * in2[0][2] + (in1.y - in2[1][3]) * in2[1][2] + (in1.z - in2[2][3]) * in2[2][2];
}

Vector vector_rotate(const Vector& in1, const matrix3x4_t& in2) {
	return Vector(in1.Dot(in2[0]), in1.Dot(in2[1]), in1.Dot(in2[2]));
}

void math::angle_matrix(const QAngle& ang_view, matrix3x4_t& mat_output, const Vector& vec_origin) {
	float_t sp, sy, sr, cp, cy, cr;

	sin_cos(DEG2RAD(ang_view.x), &sp, &cp);
	sin_cos(DEG2RAD(ang_view.y), &sy, &cy);
	sin_cos(DEG2RAD(ang_view.z), &sr, &cr);

	const float_t crcy = cr * cy;
	const float_t crsy = cr * sy;
	const float_t srcy = sr * cy;
	const float_t srsy = sr * sy;

	mat_output.Init(Vector(cp * cy, cp * sy, -sp), Vector(sp * srcy - crsy, sp * srsy + crcy, sr * cp), Vector(sp * crcy + srsy, sp * crsy - srcy, cr * cp), vec_origin);
}

matrix3x4_t math::angle_matrix(const QAngle angles) {
	matrix3x4_t result{};

	m128 angle, sin, cos;
	angle.f[0] = DEG2RAD(angles.x);
	angle.f[1] = DEG2RAD(angles.y);
	angle.f[2] = DEG2RAD(angles.z);
	sincos_ps(angle.v, &sin.v, &cos.v);

	result[0][0] = cos.f[0] * cos.f[1];
	result[1][0] = cos.f[0] * sin.f[1];
	result[2][0] = -sin.f[0];

	const auto crcy = cos.f[2] * cos.f[1];
	const auto crsy = cos.f[2] * sin.f[1];
	const auto srcy = sin.f[2] * cos.f[1];
	const auto srsy = sin.f[2] * sin.f[1];

	result[0][1] = sin.f[0] * srcy - crsy;
	result[1][1] = sin.f[0] * srsy + crcy;
	result[2][1] = sin.f[2] * cos.f[0];

	result[0][2] = sin.f[0] * crcy + srsy;
	result[1][2] = sin.f[0] * crsy - srcy;
	result[2][2] = cos.f[2] * cos.f[0];

	return result;
}

void math::matrix_set_column(const Vector &in, int column, matrix3x4_t& out) {
	out[0][column] = in.x;
	out[1][column] = in.y;
	out[2][column] = in.z;
}

void math::angle_matrix(const QAngle &angles, matrix3x4_t& matrix) {
#ifdef _VPROF_MATHLIB
	VPROF_BUDGET("AngleMatrix", "Mathlib");
#endif

	float sr, sp, sy, cr, cp, cy;

#ifdef _X360
	fltx4 radians, scale, sine, cosine;
	radians = LoadUnaligned3SIMD(angles.Base());
	scale = ReplicateX4(M_PI_F / 180.f);
	radians = MulSIMD(radians, scale);
	SinCos3SIMD(sine, cosine, radians);

	sp = SubFloat(sine, 0);	sy = SubFloat(sine, 1);	sr = SubFloat(sine, 2);
	cp = SubFloat(cosine, 0);	cy = SubFloat(cosine, 1);	cr = SubFloat(cosine, 2);
#else
	sin_cos(DEG2RAD(angles[1]), &sy, &cy);
	sin_cos(DEG2RAD(angles[0]), &sp, &cp);
	sin_cos(DEG2RAD(angles[2]), &sr, &cr);
#endif

	matrix[0][0] = cp * cy;
	matrix[1][0] = cp * sy;
	matrix[2][0] = -sp;

	float crcy = cr * cy;
	float crsy = cr * sy;
	float srcy = sr * cy;
	float srsy = sr * sy;
	matrix[0][1] = sp * srcy - crsy;
	matrix[1][1] = sp * srsy + crcy;
	matrix[2][1] = sr * cp;

	matrix[0][2] = (sp*crcy + srsy);
	matrix[1][2] = (sp*crsy - srcy);
	matrix[2][2] = cr * cp;

	matrix[0][3] = 0.0f;
	matrix[1][3] = 0.0f;
	matrix[2][3] = 0.0f;
}

void math::angle_matrix(const QAngle &angles, const Vector &position, matrix3x4_t& matrix) {
	angle_matrix(angles, matrix);
	matrix_set_column(position, 3, matrix);
}

Vector math::vector_rotate(const Vector& in1, const QAngle& in2) {
	const auto matrix = angle_matrix(in2);
	return vector_rotate(in1, matrix);
}

void math::rotate(const Vector& in1, const matrix3x4_t& in2, Vector& out) {
	out.x = in1.x * in2[0][0] + in1.y * in2[1][0] + in1.z * in2[2][0];
	out.y = in1.x * in2[0][1] + in1.y * in2[1][1] + in1.z * in2[2][1];
	out.z = in1.x * in2[0][2] + in1.y * in2[1][2] + in1.z * in2[2][2];
}

void math::vector_angles(const Vector& forward, QAngle& angles) {
	float tmp, yaw, pitch;
	if (forward[1] == 0 && forward[0] == 0) {
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else {
		yaw = (atan2(forward[1], forward[0]) * 180 / DirectX::XM_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / DirectX::XM_PI);
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void math::angle_vectors(const QAngle& angles, Vector* forward) {
	float sp, sy, cp, cy;

	sin_cos(DEG2RAD(angles[YAW]), &sy, &cy);
	sin_cos(DEG2RAD(angles[PITCH]), &sp, &cp);

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

void math::clamp_angles(QAngle& angles) {
	if (angles.x > 89.0f && angles.x <= 180.0f)
		angles.x = 89.0f;

	if (angles.x > 180.0f)
		angles.x = angles.x - 360.0f;

	if (angles.x < -89.0f)
		angles.x = -89.0f;

	angles.y = fmodf(angles.y + 180, 360) - 180;

	angles.z = 0;
}

void math::normalize_angles(QAngle& angles) {
	for (auto i = 0; i < 3; i++) {
		while (angles[i] < -180.0f) angles[i] += 360.0f;
		while (angles[i] > 180.0f) angles[i] -= 360.0f;
	}
}

Vector math::calculate_angle(const Vector& in, Vector out) {
	double delta[3] = { (in[0] - out[0]), (in[1] - out[1]), (in[2] - out[2]) };
	double hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);
	Vector ret = Vector();
	ret.x = (asinf((float)(delta[2] / hyp)) * 57.295779513082f);
	ret.y = (atanf((float)(delta[1] / delta[0])) * 57.295779513082f);
	ret.z = 0.0f;

	if (delta[0] >= 0.0)
		ret.y += 180.0f;
	return ret;
}

void math::angle_vectors(const QAngle& angles, Vector& forward, Vector& right, Vector& up) {
	float sr, sp, sy, cr, cp, cy;

	DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
	DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
	DirectX::XMScalarSinCos(&sr, &cr, DEG2RAD(angles[2]));

	forward.x = (cp * cy);
	forward.y = (cp * sy);
	forward.z = (-sp);
	right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
	right.y = (-1 * sr * sp * sy + -1 * cr * cy);
	right.z = (-1 * sr * cp);
	up.x = (cr * sp * cy + -sr * -sy);
	up.y = (cr * sp * sy + -sr * cy);
	up.z = (cr * cp);
}

void math::angle_vectors(const QAngle &angles, Vector *forward, Vector *right, Vector *up) {
	float sr, sp, sy, cr, cp, cy;

	sin_cos(DEG2RAD(angles.y), &sy, &cy);
	sin_cos(DEG2RAD(angles.x), &sp, &cp);
	sin_cos(DEG2RAD(angles.z), &sr, &cr);

	if (forward) {
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right) {
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}

	if (up) {
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}

void math::fix_movement(CUserCmd* cmd, QAngle& wishangle)
{
	Vector view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	angle_vectors(wishangle, view_fwd, view_right, view_up);
	angle_vectors(cmd->viewangles, cmd_fwd, cmd_right, cmd_up);

	const auto v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
	const auto v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
	const auto v12 = sqrtf(view_up.z * view_up.z);

	const Vector norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
	const Vector norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
	const Vector norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

	const auto v14 = sqrtf((cmd_fwd.x * cmd_fwd.x) + (cmd_fwd.y * cmd_fwd.y));
	const auto v16 = sqrtf((cmd_right.x * cmd_right.x) + (cmd_right.y * cmd_right.y));
	const auto v18 = sqrtf(cmd_up.z * cmd_up.z);

	const Vector norm_cmd_fwd((1.f / v14) * cmd_fwd.x, (1.f / v14) * cmd_fwd.y, 0.f);
	const Vector norm_cmd_right((1.f / v16) * cmd_right.x, (1.f / v16) * cmd_right.y, 0.f);
	const Vector norm_cmd_up(0.f, 0.f, (1.f / v18) * cmd_up.z);

	const auto v22 = norm_view_fwd.x * cmd->forwardmove;
	const auto v26 = norm_view_fwd.y * cmd->forwardmove;
	const auto v28 = norm_view_fwd.z * cmd->forwardmove;
	const auto v24 = norm_view_right.x * cmd->sidemove;
	const auto v23 = norm_view_right.y * cmd->sidemove;
	const auto v25 = norm_view_right.z * cmd->sidemove;
	const auto v30 = norm_view_up.x * cmd->upmove;
	const auto v27 = norm_view_up.z * cmd->upmove;
	const auto v29 = norm_view_up.y * cmd->upmove;

	cmd->forwardmove = ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25))
		+ (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)))
		+ (((norm_cmd_fwd.y * v30) + (norm_cmd_fwd.x * v29)) + (norm_cmd_fwd.z * v27));
	cmd->sidemove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25))
		+ (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)))
		+ (((norm_cmd_right.x * v29) + (norm_cmd_right.y * v30)) + (norm_cmd_right.z * v27));
	cmd->upmove = ((((norm_cmd_up.x * v23) + (norm_cmd_up.y * v24)) + (norm_cmd_up.z * v25))
		+ (((norm_cmd_up.x * v26) + (norm_cmd_up.y * v22)) + (norm_cmd_up.z * v28)))
		+ (((norm_cmd_up.x * v30) + (norm_cmd_up.y * v29)) + (norm_cmd_up.z * v27));

	const auto ratio = 2.f - fmaxf(fabsf(cmd->sidemove), fabsf(cmd->forwardmove)) / 450.f;
	cmd->forwardmove *= ratio;
	cmd->sidemove *= ratio;

	wishangle = cmd->viewangles;
}

float math::angle_diff(float dest_angle, float src_angle) {

	float delta;

	delta = fmodf(dest_angle - src_angle, 360.0f);
	if (dest_angle > src_angle) {
		if (delta >= 180)
			delta -= 360;
	}
	else {
		if (delta <= -180)
			delta += 360;
	}
	return delta;
}

Vector math::calc_angle(const Vector& source, const Vector& destination, const Vector& viewangles) {
	Vector delta = destination - source;
	auto radiansToDegrees = [](float radians) noexcept { return radians * 180 / static_cast<float>(DirectX::XM_PI); };
	Vector angles{ radiansToDegrees(atan2f(-delta.z, std::hypotf(delta.x, delta.y))) - viewangles.x, radiansToDegrees(atan2f(delta.y, delta.x)) - viewangles.y, 0.f };
	angles.Normalized();
	return angles;
}

static float segment_to_segment(const Vector s1, const Vector s2, const Vector k1, const Vector k2) {
	static auto constexpr epsilon = 0.00000001;

	auto u = s2 - s1;
	auto v = k2 - k1;
	const auto w = s1 - k1;

	const auto a = u.Dot(u);
	const auto b = u.Dot(v);
	const auto c = v.Dot(v);
	const auto d = u.Dot(w);
	const auto e = v.Dot(w);
	const auto D = a * c - b * b;
	float sn, sd = D;
	float tn, td = D;

	if (D < epsilon) {
		sn = 0.0;
		sd = 1.0;
		tn = e;
		td = c;
	}
	else {
		sn = b * e - c * d;
		tn = a * e - b * d;

		if (sn < 0.0) {
			sn = 0.0;
			tn = e;
			td = c;
		}
		else if (sn > sd) {
			sn = sd;
			tn = e + b;
			td = c;
		}
	}

	if (tn < 0.0) {
		tn = 0.0;

		if (-d < 0.0)
			sn = 0.0;
		else if (-d > a)
			sn = sd;
		else {
			sn = -d;
			sd = a;
		}
	}
	else if (tn > td) {
		tn = td;

		if (-d + b < 0.0)
			sn = 0;
		else if (-d + b > a)
			sn = sd;
		else {
			sn = -d + b;
			sd = a;
		}
	}

	const float sc = abs(sn) < epsilon ? 0.0f : sn / sd;
	const float tc = abs(tn) < epsilon ? 0.0f : tn / td;

	m128 n;
	auto dp = w + u * sc - v * tc;
	n.f[0] = dp.Dot(dp);
	const auto calc = sqrt_ps(n.v);
	return reinterpret_cast<const m128*>(&calc)->f[0];
}

bool math::intersect_bb(Vector& start, Vector& end, Vector& min, Vector& max) {
	float d1, d2, f;
	auto start_solid = true;
	auto t1 = -1.0f, t2 = 1.0f;

	const float s[3] = { start.x, start.y, start.z };
	const float e[3] = { end.x, end.y, end.z };
	const float mi[3] = { min.x, min.y, min.z };
	const float ma[3] = { max.x, max.y, max.z };

	for (auto i = 0; i < 6; i++) {
		if (i >= 3) {
			const auto j = i - 3;

			d1 = s[j] - ma[j];
			d2 = d1 + e[j];
		}
		else {
			d1 = -s[i] + mi[i];
			d2 = d1 - e[i];
		}

		if (d1 > 0.0f && d2 > 0.0f)
			return false;

		if (d1 <= 0.0f && d2 <= 0.0f)
			continue;

		if (d1 > 0)
			start_solid = false;

		if (d1 > d2) {
			f = d1;
			if (f < 0.0f)
				f = 0.0f;

			f /= d1 - d2;
			if (f > t1)
				t1 = f;
		}
		else {
			f = d1 / (d1 - d2);
			if (f < t2)
				t2 = f;
		}
	}

	return start_solid || (t1 < t2 && t1 >= 0.0f);
}

bool math::intersect(Vector start, Vector end, Vector a, Vector b, float radius) {
	const auto dist = segment_to_segment(a, b, start, end);
	return (dist < radius);
}