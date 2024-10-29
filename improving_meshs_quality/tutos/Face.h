#pragma once
typedef int index;

struct Face
{
	index vertices[3]; // indices des sommets
	index index_opposite_faces[3]; // faces opposées aux sommets

	Face(index v1, index v2, index v3, index f1 = -1, index f2 = -1, index f3 = -1)
	{
		vertices[0] = v1;
		vertices[1] = v2;
		vertices[2] = v3;
		index_opposite_faces[0] = f1;
		index_opposite_faces[1] = f2;
		index_opposite_faces[2] = f3;
	}

	Face(const Face& f)
	{
		if (this != &f)
			for (int i = 0; i < 3; i++)
			{
				vertices[i] = f.vertices[i];
				index_opposite_faces[i] = f.index_opposite_faces[i];
			}
	}
};
