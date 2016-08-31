#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "d3dUtil.h"
#include "Vertex.h"
#include "Mesh.h"
#include "MathHelp.h"

class Geometry
{
public:
	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshType2& mesh);
 
private:
	void BuildBottomCap(float bottomRadius, float topRadius, float height, 
					    UINT sliceCount, UINT stackCount, MeshType2& mesh);
	void BuildTopCap(float bottomRadius, float topRadius, float height, 
					 UINT sliceCount, UINT stackCount, MeshType2& mesh);
};

#endif // GEOMETRY_H