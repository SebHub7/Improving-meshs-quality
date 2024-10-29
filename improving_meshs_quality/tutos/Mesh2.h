#pragma once
#include <vector>
#include "Sommet.h"
#include "Face.h"
#include <fstream>
#include <iostream>

struct Mesh2
{
	std::vector<Sommet> vertices;
	std::vector<Face> faces;

	Mesh2();

	void saveMeshToObj(std::string file_name);

	std::vector<Sommet> readPointCloudFile(std::string file_name);

	void getMeshBounds(std::vector<Sommet>& vertices_file, Sommet& min, Sommet& max);

	index getLocalIndexOfVertex(index face, index vertex) const;

	index getLocalIndexOfVertexOppositeToEdge(Face* f, Sommet& s) const;

	index getLocalIndexOfVertexOppositeToFace(index face, index neighbor_face) const;

	index getEdgeWhereVertexIs(index face, Sommet& s) const;

	float orientationTest(const Sommet& p1, const Sommet& p2, const Sommet& p3) const;

	int isVertexInTriangle(index face, Sommet& p);

	void triangleSplit(index face, Sommet& s);

	void edgeSplit(index face, Sommet& s);

	void edgeFlip2D(index face, index neighbor_face);

	bool isTriangleInfinite(index face);

	int isVertexInATriangle(Sommet& s, index& face);

	int isvertexInATriangleFast(Sommet& s, index& face);

	void insertFirstTriangle();

	index findFirstVisibleTriangle(Sommet& s);

	index getLeftTriangleAroundInfinite(index face);

	index getRightTriangleAroundInfinite(index face);

	void flipTrianglesOnTheSide(index current_face, bool search_right, Sommet& s);

	void insertPoint(Sommet& s);
};
