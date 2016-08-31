#ifndef MESH_H
#define MESH_H

#include <vector>

struct MeshType1
{
	std::vector<VertexType1> Vertices;
	std::vector<UINT> Indices;
};

struct MeshType2
{
	std::vector<VertexType2> Vertices;
	std::vector<UINT> Indices;
};

#endif