#pragma once
#include <corecrt_math.h>

typedef int index;

struct Sommet
{
	float x, y, z;
	int indice_face_incidente;

	Sommet(double x, double y, double z, index indice_face = -1)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		indice_face_incidente = indice_face;
	}

	Sommet operator = (const Sommet& s)
	{
		if (this != &s)
		{
			x = s.x;
			y = s.y; 
			z = s.z;

			indice_face_incidente = s.indice_face_incidente;
		}
		return *this;
	}
};