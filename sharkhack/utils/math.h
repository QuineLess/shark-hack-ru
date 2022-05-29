#pragma once

#include "../sdk/interfaces.h"

#include <DirectXMath.h>
#include <corecrt_math_defines.h>

#define RAD2DEG(x) DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x) DirectX::XMConvertToRadians(x)
#define squares( x ) ( x * x )

#define M_PI 3.14159265358979323846f
#define PI_F ( ( float )( M_PI ) ) 
#define M_RADPI 57.295779513082f

enum {
	PITCH = 0,
	YAW,
	ROLL
};

namespace math 
{
	__forceinline float RemapVal( float val, float A, float B, float C, float D )
	{
		if ( A == B )
			return val >= B ? D : C;
		return C + ( D - C ) * ( val - A ) / ( B - A );
	}

	__forceinline float RemapValClamped( float val, float A, float B, float C, float D )
	{
		if ( A == B )
			return val >= B ? D : C;
		float cVal = ( val - A ) / ( B - A );
		cVal = std::clamp( cVal, 0.0f, 1.0f );

		return C + ( D - C ) * cVal;
	}
	template <class T>
	inline T Lerp( float flPercent, T const& A, T const& B )
	{
		return A + ( B - A ) * flPercent;
	}
	void FixMovement( void* pCmd );
	__forceinline QAngle CalcAngle( const Vector& src, const Vector& dst )
	{
		QAngle vAngle;
		Vector delta( ( src.x - dst.x ), ( src.y - dst.y ), ( src.z - dst.z ) );
		double hyp = sqrt( delta.x * delta.x + delta.y * delta.y );

		vAngle.x = float( atanf( float( delta.z / hyp ) ) * 57.295779513082f );
		vAngle.y = float( atanf( float( delta.y / delta.x ) ) * 57.295779513082f );
		vAngle.z = 0.0f;

		if ( delta.x >= 0.0 )
			vAngle.y += 180.0f;

		return vAngle;
	}
	__forceinline Vector CrossProduct( const Vector& a, const Vector& b )
	{
		return Vector( a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x );
	}
	__forceinline void VectorAngles( const Vector& forward, Vector& up, QAngle& angles )
	{
		Vector left = CrossProduct( up, forward );
		left.NormalizeInPlace( );

		float forwardDist = forward.Length2D( );

		if ( forwardDist > 0.001f )
		{
			angles.x = atan2f( -forward.z, forwardDist ) * 180.0f / PI_F;
			angles.y = atan2f( forward.y, forward.x ) * 180.0f / PI_F;

			float upZ = ( left.y * forward.x ) - ( left.x * forward.y );
			angles.z = atan2f( left.z, upZ ) * 180.0f / PI_F;
		}
		else
		{
			angles.x = atan2f( -forward.z, forwardDist ) * 180.0f / PI_F;
			angles.y = atan2f( -left.x, left.y ) * 180.0f / PI_F;
			angles.z = 0;
		}
	}
	bool screen_transform(const Vector & in, Vector & out);
	bool world_to_screen(const Vector & in, Vector & out);
	void vector_transform(const Vector & in1, const matrix3x4_t & in2, Vector & out);
	void i_transform(const Vector & in1, const matrix3x4_t & in2, Vector & out);
	void angle_matrix(const QAngle& ang_view, matrix3x4_t& mat_output, const Vector& vec_origin);
	matrix3x4_t angle_matrix(const QAngle angles);
	void matrix_set_column(const Vector & in, int column, matrix3x4_t & out);
	void angle_matrix(const QAngle & angles, matrix3x4_t & matrix);
	void angle_matrix(const QAngle & angles, const Vector & position, matrix3x4_t & matrix);
	Vector vector_rotate(const Vector & in1, const QAngle & in2);
	void rotate(const Vector & in1, const matrix3x4_t & in2, Vector & out);
	void vector_angles(const Vector & forward, QAngle & angles);
	void angle_vectors(const QAngle & angles, Vector * forward);
	void angle_vectors(const QAngle & angles, Vector & forward, Vector & right, Vector & up);
	void angle_vectors(const QAngle & angles, Vector * forward, Vector * right, Vector * up);
	void clamp_angles(QAngle & angles);
	void normalize_angles(QAngle & angles); 
	__forceinline float AngleNormalize( float angle )
	{
		angle = fmodf( angle, 360.0f );
		if ( angle > 180 )
		{
			angle -= 360;
		}
		if ( angle < -180 )
		{
			angle += 360;
		}
		return angle;
	}
	Vector calculate_angle(const Vector & in, Vector out);
	void fix_movement(CUserCmd * cmd, QAngle & wishangle);
	float angle_diff(float dest_angle, float src_angle);
	Vector calc_angle(const Vector & source, const Vector & destination, const Vector & viewangles);
	bool intersect_bb(Vector & start, Vector & end, Vector & min, Vector & max);
	bool intersect(Vector start, Vector end, Vector a, Vector b, float radius);
	void inline sin_cos(float radians, float* sine, float* cosine) {
		*sine = sin(radians);
		*cosine = cos(radians);
	}
	inline float fast_sqrts(float x) {
		unsigned int i = *(unsigned int*)&x;
		i += 127 << 23;
		i >>= 1;
		return *(float*)&i;
	}

}