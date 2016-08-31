//***************************************************************************************
// 'Hut Class' by Michael Dunleavy          Last Edit: 03/05/2013
//
// This class is responsible for creating the geometry for the hut at the center of
// the scene.  It gathers all the vertices for each of the geometric shapes and renders
// them on screen.
//***************************************************************************************

#ifndef HUT_H
#define HUT_H

#include "D3DUtil.h"
#include "GeometryGenerator.h"
#include "Vertex.h"
#include "Effects.h"

class Camera;
struct DirectionalLight;

class Hut
{
public:
	Hut();
	~Hut();

	void Initialize(ID3D11Device* device);
	void Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);

private:
	void BuildGeometryBuffers(ID3D11Device* device);

private:
	ID3D11Buffer* m_vb;
	ID3D11Buffer* m_ib;

	ID3D11ShaderResourceView* m_stoneTexSRV;
	ID3D11ShaderResourceView* m_brickTexSRV;

	Material m_boxMaterial;
	Material m_ringMaterial;
	Material m_cylinderMaterial;
	Material m_sphereMaterial;

	// Define transformations from local spaces to world space.
	XMFLOAT4X4 m_boxWorld;
	XMFLOAT4X4 m_ringWorld;
	XMFLOAT4X4 m_sphereWorld[10];
	XMFLOAT4X4 m_cylWorld[10];

	int m_boxVertexOffset;
	int m_ringVertexOffset;
	int m_sphereVertexOffset;
	int m_cylinderVertexOffset;

	UINT m_boxIndexOffset;
	UINT m_ringIndexOffset;
	UINT m_sphereIndexOffset;
	UINT m_cylinderIndexOffset;

	UINT m_boxIndexCount;
	UINT m_ringIndexCount;
	UINT m_sphereIndexCount;
	UINT m_cylinderIndexCount;
};

#endif // Hut.h