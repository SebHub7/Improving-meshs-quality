#pragma once
#include "Sommet.h"

struct Vector3
{
	float x, y, z;

	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	Vector3(const Vector3& v) : x(0), y(0), z(0)
	{
		if (this != &v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
		}
	}

	Vector3(Sommet& s) : x(s.x), y(s.y), z(s.z) {}

	float length() { return sqrt(x * x + y * y + z * z); }

	Vector3 operator - (const Vector3& v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	void normalize()
	{
		float l = length();
		x /= l; 
		y /= l; 
		z /= l;
	}

	static Vector3 cross(Vector3& v1, Vector3& v2)
	{
		Vector3 n(
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x
		);

		n.normalize();
		return n;
	}

	static float dot(const Vector3& v1, const Vector3& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

};