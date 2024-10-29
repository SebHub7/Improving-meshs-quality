#include "./tutos/Delaunay.h"

bool Delaunay::isVertexInCircumscribedCircle(Mesh2& m, Face* f, Sommet& s) const
{
	Vector3 a(m.vertices[f->vertices[0]]);
	Vector3 b(m.vertices[f->vertices[1]]);
	Vector3 c(m.vertices[f->vertices[2]]);
	Vector3 d(s);

	Vector3 v1 = b - a;
	Vector3 v2 = c - a;
	Vector3 v3(d - a);

	Vector3 a1(v1.x, v1.y, v1.x * v1.x + v1.y * v1.y);
	Vector3 a2(v2.x, v2.y, v2.x * v2.x + v2.y * v2.y);
	Vector3 a3(v3.x, v3.y, v3.x * v3.x + v3.y * v3.y);

	v1.normalize();
	v2.normalize();
	v3.normalize();

	Vector3 n = Vector3::cross(a1, a2);
	n.normalize();
	float sign = -Vector3::dot(n, a3);

	return sign > 0;
}

bool Delaunay::isEdgeLocallyDelaunay(Mesh2& m, index face, index local_opp_vertex) const
{
	Face* f = &m.faces[face];
	index neighbor_face = f->index_opposite_faces[local_opp_vertex];

	if (!m.isTriangleInfinite(neighbor_face))
	{
		Face* f_neighbor = &m.faces[neighbor_face];
		index opp_vertex2 = m.getLocalIndexOfVertexOppositeToFace(neighbor_face, face);
		return !isVertexInCircumscribedCircle(m, f, m.vertices[f_neighbor->vertices[opp_vertex2]]);
	}

	return true;
}

void Delaunay::flipTriangles(Mesh2& m, index face, index vertex)
{
	index local_vertex = m.getLocalIndexOfVertex(face, vertex);

	if (!isEdgeLocallyDelaunay(m, face, local_vertex))
	{
		index neighbor = m.faces[face].index_opposite_faces[local_vertex];
		m.edgeFlip2D(face, neighbor);

		flipTriangles(m, face, vertex);
		flipTriangles(m, neighbor, vertex);
	}
}

void Delaunay::correctTrianglesInside(Mesh2& m, index face, index vertex)
{
	// lors d'un split l'indice local du sommet inséré dans la face "face" splitée est 2
	Face old_face = m.faces[face];
	index current_face = face;

	flipTriangles(m, current_face, vertex);

	current_face = old_face.index_opposite_faces[0];
	flipTriangles(m, current_face, vertex);

	current_face = old_face.index_opposite_faces[1];
	flipTriangles(m, current_face, vertex);
}