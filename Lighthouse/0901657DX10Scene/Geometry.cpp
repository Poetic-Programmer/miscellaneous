#include "Geometry.h"
#include "Vertex.h"

void Geometry::CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshType2& mesh)
{
	mesh.Vertices.clear();
	mesh.Indices.clear();

	//
	// Build Stacks.
	// 

	float stackHeight = height / stackCount;

	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	UINT ringCount = stackCount+1;

	// Compute vertices for each stack ring starting at the bottom and moving up.
	for(UINT i = 0; i < ringCount; ++i)
	{
		float y = -0.5f*height + i*stackHeight;
		float r = bottomRadius + i*radiusStep;

		// vertices of ring
		float dTheta = 2.0f*MathHelp::Pi/sliceCount;
		for(UINT j = 0; j <= sliceCount; ++j)
		{
			VertexType2 vertex;

			float c = cosf(j*dTheta);
			float s = sinf(j*dTheta);

			vertex.Position = D3DXVECTOR3(r*c, y, r*s);

			vertex.TexC.x = (float)j/sliceCount;
			vertex.TexC.y = 1.0f - (float)i/stackCount;

			// Cylinder can be parameterized as follows, where we introduce v
			// parameter that goes in the same direction as the v tex-coord
			// so that the bitangent goes in the same direction as the v tex-coord.
			//   Let r0 be the bottom radius and let r1 be the top radius.
			//   y(v) = h - hv for v in [0,1].
			//   r(v) = r1 + (r0-r1)v
			//
			//   x(t, v) = r(v)*cos(t)
			//   y(t, v) = h - hv
			//   z(t, v) = r(v)*sin(t)
			// 
			//  dx/dt = -r(v)*sin(t)
			//  dy/dt = 0
			//  dz/dt = +r(v)*cos(t)
			//
			//  dx/dv = (r0-r1)*cos(t)
			//  dy/dv = -h
			//  dz/dv = (r0-r1)*sin(t)

			// This is unit length.
			vertex.TangentU = D3DXVECTOR3(-s, 0.0f, c);

			float dr = bottomRadius-topRadius;
			D3DXVECTOR3 bitangent(dr*c, -height, dr*s);

			D3DXVECTOR3 T, B, N;
			T = vertex.TangentU;
			B = bitangent;
			D3DXVec3Cross(&N, &T, &B);
			D3DXVec3Normalize(&N, &N);
			vertex.Normal = N;

			mesh.Vertices.push_back(vertex);
		}
	}

	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different.
	UINT ringVertexCount = sliceCount+1;

	// Compute indices for each stack.
	for(UINT i = 0; i < stackCount; ++i)
	{
		for(UINT j = 0; j < sliceCount; ++j)
		{
			mesh.Indices.push_back(i*ringVertexCount + j);
			mesh.Indices.push_back((i+1)*ringVertexCount + j);
			mesh.Indices.push_back((i+1)*ringVertexCount + j+1);

			mesh.Indices.push_back(i*ringVertexCount + j);
			mesh.Indices.push_back((i+1)*ringVertexCount + j+1);
			mesh.Indices.push_back(i*ringVertexCount + j+1);
		}
	}

	BuildTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, mesh);
	BuildBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, mesh);
}

void Geometry::BuildBottomCap(float bottomRadius, float topRadius, float height, 
					    UINT numSlices, UINT numStacks, MeshType2& mesh)
{
	// 
	// Build bottom cap.
	//
	UINT baseIndex = (UINT)mesh.Vertices.size();
	float y = -0.5f*height;

	// vertices of ring
	float dTheta = 2.0f*MathHelp::Pi/numSlices;
	for(UINT i = 0; i <= numSlices; ++i)
	{
		float x = bottomRadius*cosf(i*dTheta);
		float z = bottomRadius*sinf(i*dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x/height + 0.5f;
		float v = z/height + 0.5f;

		mesh.Vertices.push_back( VertexType2(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v) );
	}

	// Cap center vertex.
	mesh.Vertices.push_back( VertexType2(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f) );

	// Cache the index of center vertex.
	UINT centerIndex = (UINT)mesh.Vertices.size()-1;

	for(UINT i = 0; i < numSlices; ++i)
	{
		mesh.Indices.push_back(centerIndex);
		mesh.Indices.push_back(baseIndex + i);
		mesh.Indices.push_back(baseIndex + i+1);
	}
}

void Geometry::BuildTopCap(float bottomRadius, float topRadius, float height, 
					 UINT numSlices, UINT numStacks, MeshType2& mesh)
{
	UINT baseIndex = (UINT)mesh.Vertices.size();

	float y = 0.5f*height;
	float dTheta = 2.0f*MathHelp::Pi/numSlices;

	// Duplicate cap ring vertices because the texture coordinates and normals differ.
	for(UINT i = 0; i <= numSlices; ++i)
	{
		float x = topRadius*cosf(i*dTheta);
		float z = topRadius*sinf(i*dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x/height + 0.5f;
		float v = z/height + 0.5f;

		mesh.Vertices.push_back( VertexType2(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v) );
	}

	// Cap center vertex.
	mesh.Vertices.push_back( VertexType2(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f) );

	// Index of center vertex.
	UINT centerIndex = (UINT)mesh.Vertices.size()-1;

	for(UINT i = 0; i < numSlices; ++i)
	{
		mesh.Indices.push_back(centerIndex);
		mesh.Indices.push_back(baseIndex + i+1);
		mesh.Indices.push_back(baseIndex + i);
	}
}