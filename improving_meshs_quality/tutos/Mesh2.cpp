#include "Mesh2.h"
#include <string>
#include "Vector3.h"
#include "Delaunay.h"

Mesh2::Mesh2()
{
	vertices.push_back(Sommet(0, 0, 3000, 1));
}

void Mesh2::saveMeshToObj(std::string file_name)
{
	std::ofstream file;
	file.open(file_name);

	for (int i = 0; i < vertices.size(); i++)
		file << "v " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << "\n";

	file << "\n";

	for (Face& f : faces)
	{
		file << "f " << f.vertices[0] + 1 << "/" << f.vertices[0] + 1 << "/" << f.vertices[0] + 1 << " ";
		file << f.vertices[1] + 1 << "/" << f.vertices[1] + 1 << "/" << f.vertices[1] + 1 << " ";
		file << f.vertices[2] + 1 << "/" << f.vertices[2] + 1 << "/" << f.vertices[2] + 1 << "\n";
	}

	file.close();
	std::cout << "Fichier: " << file_name << " sauvegarde" << std::endl;
}

std::vector<Sommet> Mesh2::readPointCloudFile(std::string file_name)
{
	std::fstream file;
	file.open(file_name);
	std::string line;
	std::getline(file, line);
	std::vector<Sommet> vertices_file;

	while (std::getline(file, line))
	{
		float x, y, z;

		if (sscanf(line.c_str(), "%f %f %f", &x, &y, &z) == 3)
			vertices_file.push_back(Sommet(x, y, z));
	}

	file.close();
	return vertices_file;
}

void Mesh2::getMeshBounds(std::vector<Sommet>& vertices_file, Sommet& min, Sommet& max)
{
	for (Sommet& v : vertices_file)
	{
		if (v.x < min.x)
			min.x = v.x;

		if (v.y < min.y)
			min.y = v.y;

		if (v.z < min.z)
			min.z = v.z;

		if (v.x > max.x)
			max.x = v.x;

		if (v.y > max.y)
			max.y = v.y;

		if (v.z > max.z)
			max.z = v.z;
	}
}

index Mesh2::getLocalIndexOfVertex(index face, index vertex) const
{
	for (int i = 0; i < 3; i++)
		if (faces[face].vertices[i] == vertex)
			return i;

	return -1;
}

index Mesh2::getLocalIndexOfVertexOppositeToEdge(Face* f, Sommet& s) const
{
	index local = -1;

	for (int i = 0; i < 3; i++)
		if (orientationTest(vertices[f->vertices[i]], vertices[f->vertices[(i + 1) % 3]], s) == 0)
		{
			local = (i + 2) % 3;
			break;
		}
	return local;
}

index Mesh2::getLocalIndexOfVertexOppositeToFace(index face, index neighbor_face) const
{
	for (int i = 0; i < 3; i++)
		if (faces[face].index_opposite_faces[i] == neighbor_face)
			return i;

	return -1;
}

index Mesh2::getEdgeWhereVertexIs(index face, Sommet& s) const
{
	Face const* f = &faces[face];

	for (int i = 0; i < 3; i++)
		if (orientationTest(vertices[f->vertices[i]], vertices[f->vertices[(i + 1) % 3]], s) == 0)
			return (i + 2) % 3;

	return -1;
}

float Mesh2::orientationTest(const Sommet& p1, const Sommet& p2, const Sommet& p3) const
{
	Vector3 v1 = Vector3(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
	Vector3 v2 = Vector3(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
	v1.normalize();
	v2.normalize();

	return v1.x * v2.y - v1.y * v2.x;
}

int Mesh2::isVertexInTriangle(index triangle, Sommet& p)
{
	Face* f = &faces[triangle];
	float signs[3];

	for (int i = 0; i < 3; i++)
		signs[i] = orientationTest(vertices[f->vertices[i]], vertices[f->vertices[(i + 1) % 3]], p);

	if (signs[0] > 0 && signs[1] > 0 && signs[2] > 0)
		return 1;

	if (signs[0] == 0 || signs[1] == 0 || signs[2] == 0)
		return 0;

	return -1;
}

void Mesh2::edgeSplit(index face, Sommet& s)
{
	Face* f = &faces[face];
	index new_vertex = vertices.size() - 1;

	index local_op = getEdgeWhereVertexIs(face, s);
	index neighbor = f->index_opposite_faces[local_op];

	Face old_f = *f;
	Face* neighbor_f = &faces[neighbor];
	Face old_neighbor = *neighbor_f;
	index local_op_neighbor = getLocalIndexOfVertexOppositeToFace(neighbor, face);

	index new_face1 = faces.size();
	index new_face2 = faces.size() + 1;

	f->vertices[(local_op + 1) % 3] = new_vertex;
	f->index_opposite_faces[(local_op + 2) % 3] = new_face1;

	neighbor_f->vertices[(local_op_neighbor + 2) % 3] = new_vertex;
	neighbor_f->index_opposite_faces[(local_op_neighbor + 1) % 3] = new_face2;

	faces.push_back(Face(f->vertices[local_op], old_f.vertices[(local_op + 1) % 3], new_vertex,
					new_face2, face, old_f.index_opposite_faces[(local_op + 2) % 3]));

	faces.push_back(Face(old_neighbor.vertices[local_op_neighbor], new_vertex, old_neighbor.vertices[(local_op_neighbor + 2) % 3],
		new_face1, old_neighbor.index_opposite_faces[(local_op_neighbor + 1) % 3], neighbor));

	// modification des index voisins des faces voisines
	index n1 = old_f.index_opposite_faces[(local_op + 2) % 3];
	index n2 = old_neighbor.index_opposite_faces[(local_op_neighbor + 1) % 3];

	index local_op_n1 = getLocalIndexOfVertexOppositeToFace(n1, face);
	index local_op_n2 = getLocalIndexOfVertexOppositeToFace(n2, neighbor);

	faces[n1].index_opposite_faces[local_op_n1] = new_face1;
	faces[n2].index_opposite_faces[local_op_n2] = new_face2;
}

void Mesh2::edgeFlip2D(index face, index neighbor_face)
{
	Face* f = &faces[face];
	index local_opp_vertex = getLocalIndexOfVertexOppositeToFace(face, neighbor_face);
	Face* f_neighbor = &faces[neighbor_face];
	Face old_f(*f);
	Face old_neighbor(*f_neighbor);

	index oppoVertex2 = getLocalIndexOfVertexOppositeToFace(neighbor_face, face);

	// modifying vertices
	f->vertices[(local_opp_vertex + 1) % 3] = f_neighbor->vertices[oppoVertex2];
	f_neighbor->vertices[(oppoVertex2 + 1) % 3] = f->vertices[local_opp_vertex];

	// modifying index neighbors
	f->index_opposite_faces[(local_opp_vertex + 2) % 3] = neighbor_face;
	f->index_opposite_faces[local_opp_vertex] = old_neighbor.index_opposite_faces[(oppoVertex2 + 2) % 3];

	f_neighbor->index_opposite_faces[(oppoVertex2 + 2) % 3] = face;
	f_neighbor->index_opposite_faces[oppoVertex2] = old_f.index_opposite_faces[(local_opp_vertex + 2) % 3];

	// modifyng other neighbors's neighbors
	index n1 = old_neighbor.index_opposite_faces[(oppoVertex2 + 2) % 3];
	index n2 = old_f.index_opposite_faces[(local_opp_vertex + 2) % 3];

	Face* neighbor1 = &faces[n1];
	Face* neighbor2 = &faces[n2];

	index local_opp_n1 = getLocalIndexOfVertexOppositeToFace(n1, neighbor_face);
	index local_opp_n2 = getLocalIndexOfVertexOppositeToFace(n2, face);

	neighbor1->index_opposite_faces[local_opp_n1] = face;
	neighbor2->index_opposite_faces[local_opp_n2] = neighbor_face;
}

void Mesh2::triangleSplit(index face, Sommet& s)
{
	int index_s = vertices.size() - 1;
	int index_f1 = faces.size();
	int index_f2 = faces.size() + 1;
	s.indice_face_incidente = index_f1;

	Face old_face = faces[face];
	Face* f = &faces[face];

	// flat triangle
	index v1 = f->vertices[2];
	index v2 = index_s;
	index neighbor1 = face;
	index neighbor2 = f->index_opposite_faces[0];

	Face f1( f->vertices[1], v1, v2,
		index_f2, neighbor1, neighbor2 );

	// infinite triangle en 3D
	Face f2( f->vertices[0], index_s, f->vertices[2],
		index_f1, f->index_opposite_faces[1], face );

	f->vertices[2] = index_s;
	f->index_opposite_faces[0] = index_f1;
	f->index_opposite_faces[1] = index_f2;

	faces.push_back(f1);
	faces.push_back(f2);

	// modification des indices triangles voisins des triangles voisins
	index n1 = old_face.index_opposite_faces[0];
	index n2 = old_face.index_opposite_faces[1];

	index local_opp_vertex = getLocalIndexOfVertexOppositeToFace(n1, face);
	index local_opp_verte2 = getLocalIndexOfVertexOppositeToFace(n2, face);

	faces[n1].index_opposite_faces[local_opp_vertex] = index_f1;
	faces[n2].index_opposite_faces[local_opp_verte2] = index_f2;
}

bool Mesh2::isTriangleInfinite(index face)
{
	return faces[face].vertices[0] == 0;
}

int Mesh2::isVertexInATriangle(Sommet& s, index& face)
{
	for (index i = 0; i < faces.size(); i++)
		if (!isTriangleInfinite(i))
		{
			int test = isVertexInTriangle(i, s);
			if (test == 1)
			{
				face = i;
				return 1;
			}
			else if (test == 0)
			{
				face = i;
				return 0;
			}
		}

	return -1;
}

int Mesh2::isvertexInATriangleFast(Sommet& s, index& face)
{
	index current_face = -1;

	for (index i = 0; i < faces.size(); i++)
		if (!isTriangleInfinite(i))
		{
			current_face = i;
			break;
		}

	while (1)
	{
		if (isTriangleInfinite(current_face))
			return -1;

		int isInTriangle = isVertexInTriangle(current_face, s);
		if (isInTriangle > 0)
		{
			face = current_face;
			return 1;
		}
		else if (isInTriangle == 0)
		{
			face = current_face;
			return 0;
		}

		Face* current = &faces[current_face];
		for (index i = 0; i < 3; i++)
		{
			Sommet s1 = vertices[current->vertices[i]];
			Sommet s2 = vertices[current->vertices[(i+1) % 3]];

			if (orientationTest(s1, s2, s) < 0)
				current_face = current->index_opposite_faces[(i+2) % 3];
		}
	}
}


void Mesh2::insertFirstTriangle()
{
	if (orientationTest(vertices[1], vertices[2], vertices[3]) > 0)
		faces.push_back(Face(1, 2, 3, 2, 3, 1));
	else
		faces.push_back(Face(1, 3, 2, 2, 3, 1));

	Face f = faces[0];
	faces.push_back(Face(0, f.vertices[1], f.vertices[0], 0, 3, 2)); // faces infinies
	faces.push_back(Face(0, f.vertices[2], f.vertices[1], 0, 1, 3));
	faces.push_back(Face(0, f.vertices[0], f.vertices[2], 0, 2, 1));
}

index Mesh2::findFirstVisibleTriangle(Sommet& s)
{
	for (index i = 0; i < faces.size(); i++)
	{
		Face* f = &faces[i];

		if (isTriangleInfinite(i) &&
			orientationTest(vertices[f->vertices[1]], vertices[f->vertices[2]], s) > 0)
			return i;
	}
	return -1;
}

index Mesh2::getLeftTriangleAroundInfinite(index face)
{
	return faces[face].index_opposite_faces[2];
}

index Mesh2::getRightTriangleAroundInfinite(index face)
{
	return faces[face].index_opposite_faces[1];
}

void Mesh2::flipTrianglesOnTheSide(index current_face, bool search_left, Sommet& s)
{
	Delaunay d;

	while (true)
	{
		index next_face = -1;

		if (search_left)
			next_face = getLeftTriangleAroundInfinite(current_face);
		else
			next_face = getRightTriangleAroundInfinite(current_face);

		Face* f = &faces[next_face];

		if (orientationTest(vertices[f->vertices[1]], vertices[f->vertices[2]], s) > 0)
		{
			edgeFlip2D(current_face, next_face);
			d.flipTriangles(*this, current_face, vertices.size() - 1);

			if (search_left)
				current_face = next_face;
			else
				current_face = getRightTriangleAroundInfinite(next_face);
		}
		else
			return;
	}
}

void Mesh2::insertPoint(Sommet& s)
{
	vertices.push_back(s);

	if (vertices.size() == 4)
		insertFirstTriangle();

	else if (vertices.size() > 4)
	{
		index face = -1;
		//int inTriangle = isVertexInATriangle(s, face);
		int inTriangle = isvertexInATriangleFast(s, face);
		Delaunay d;

		if (inTriangle == 1)
		{
			triangleSplit(face, s);
			d.correctTrianglesInside(*this, face, vertices.size() - 1);
		}
		else if (inTriangle == 0)
			edgeSplit(face, s);
		else
		{
			index visible_face_left = findFirstVisibleTriangle(s);
			triangleSplit(visible_face_left, s);
			d.flipTriangles(*this, faces.size()-2, vertices.size() - 1);

			index visible_face_right = getRightTriangleAroundInfinite(visible_face_left);
			flipTrianglesOnTheSide(visible_face_left, true, s);
			flipTrianglesOnTheSide(visible_face_right, false, s);
		}
	}
}