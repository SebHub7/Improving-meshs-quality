#pragma once
#include "Mesh2.h"
#include "Vector3.h"

struct Delaunay
{
	bool isVertexInCircumscribedCircle(Mesh2& m, Face* f, Sommet& s) const;

	bool isEdgeLocallyDelaunay(Mesh2& m, index face, index local_opp_vertex) const;

	void flipTriangles(Mesh2& m, index face, index vertex);

	// after a split insertion
	void correctTrianglesInside(Mesh2& m, index face, index vertex);
};