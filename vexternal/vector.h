#pragma once
#include <map>

#define M_PI 3.14159265358979323846

struct c_vec3 { // basic vector struct
	float x, y, z;

	inline bool operator==(const c_vec3& src) const
	{
		return (src.x == x) && (src.y == y) && (src.z == z);
	}

	inline bool operator!=(const c_vec3& src) const
	{
		return (src.x != x) || (src.y != y) || (src.z != z);
	}

	inline c_vec3& operator+=(const c_vec3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	inline c_vec3& operator-=(const c_vec3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	inline c_vec3 operator-(c_vec3 ape)
	{
		return { x - ape.x, y - ape.y, z - ape.z };
	}

	inline c_vec3 operator+(c_vec3 ape)
	{
		return { x + ape.x, y + ape.y, z + ape.z };
	}

	inline c_vec3 operator*(float ape)
	{
		return { x * ape, y * ape, z * ape };
	}

	inline c_vec3 operator/(float ape)
	{
		return { x / ape, y / ape, z / ape };
	}

	inline float length()
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}

	inline float distance(c_vec3 ape)
	{
		return (*this - ape).length();
	}

	/*inline c_vec3 calc_angle(const c_vec3& src, const c_vec3& dst) {
		c_vec3 angle = c_vec3();
		c_vec3 delta = c_vec3((src.x - dst.x), (src.y - dst.y), src.z - dst.z);

		double hypotenuse = sqrtf(delta.x * delta.x + delta.y * delta.y);

		angle.x = atanf(delta.z / hypotenuse) * (180.0f / M_PI);
		angle.y = atanf(delta.y / delta.x) * (180.0f / M_PI);
		angle.z = 0;
		if (delta.x >= 0.0) angle.y += 180.0f;

		return angle;
	}*/

	inline void clamp() {
		if (x > 180.0f) y = 180.0f;
		else if (y < -180.0f) y = -180.0f;

		if (x > 89.0f) x = 89.0f;
		else if (x < -89.0f) x = -89.0f;

		z = 0;
	}

	inline c_vec3 calc_angle(const c_vec3& src, const c_vec3& dst) {
		c_vec3 srca = { src.x, src.y, src.z };
		c_vec3 dsta = { dst.x, dst.y, dst.z };

		c_vec3 angle = c_vec3();
		c_vec3 delta = srca - dsta;

		double hyp = sqrtf((delta.x * delta.x) + (delta.y * delta.y));
		angle.x = (float)(atan(delta.z / hyp) * 57.295779513082f);
		angle.y = (float)(atan(delta.y / delta.x) * 57.295779513082f);
		if (delta.x >= 0.0) angle.y += 180.0f;

		return angle;
	}

	inline void normalize() {
		while (x > 89.0f)
			x -= 180.f;

		while (x < -89.0f)
			x += 180.f;

		while (y > 180.f)
			y -= 360.f;

		while (y < -180.f)
			y += 360.f;
	}

	inline bool is_zero() {
		return (x > -0.1f && x < 0.1f && y > -0.1f && y < 0.1f && z > -0.1f && z < 0.1f);
	}
};
