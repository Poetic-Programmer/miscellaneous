//***************************************************************************************
// 'Terrain Class' by Michael Dunleavy          Last Edit: 03/05/2013
//
// This class takes a standard grey scale heightmap and offsets the y values
// based on a Perlin Noise texture.  Both these textures are sent to the "terrain.fx"
// shader file were the values of the heightmap are offset by the noise map times a scalar
// value which increases over time.			
//     This class is also responsible for initializing the tesselation factors to be sent
// to the shader.
//***************************************************************************************

#ifndef TERRAIN_H
#define TERRAIN_H

#include "D3DUtil.h"
#include "GeometryGenerator.h"
#include "Heightmap.h"
#include "BlendMap.h"

class Camera;
struct DirectionalLight;

class Terrain
{
public:
	struct InitInfo
	{
		UINT BasemapWidth;
		UINT BasemapHeight;
		float BasemapScale;
		UINT OffsetmapWidth;
		UINT OffsetmapHeight;
		float OffsetmapScale;

		std::wstring HeightmapFileId;
		std::vector<std::wstring> TextureLayerIds;
		std::wstring BlendmapId;
	};

public:
	Terrain();
	~Terrain();

	float GetWidth()const;
	float GetDepth()const;
	float GetHeight(float x, float z)const;

	XMMATRIX GetWorld()const;
	void SetWorld(CXMMATRIX M);

	void Initialize(ID3D11Device* device, ID3D11DeviceContext* dc, InitInfo& info);

	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);

private:
	void CalcAllPatchBoundsY();
	void CalcPatchBoundsY(UINT i, UINT j);
	void BuildQuadPatchVB(ID3D11Device* device, ID3D11DeviceContext* dc);
	void BuildQuadPatchIB(ID3D11Device* device);


private:
	InitInfo m_info;
	int m_counter;
	float m_time;
	float m_scaleValue;
	bool m_erode;

	static const int CellsPerPatch = 64;
	float m_cellSpacing;

	ID3D11Buffer* m_quadPatchVB;
	ID3D11Buffer* m_quadPatchIB;

	Heightmap m_basemap;
	Heightmap m_offsetmap;
	BlendMap m_blendmap;
	

	UINT m_numPatchVertices;
	UINT m_numPatchQuadFaces;

	UINT m_numPatchVertRows;
	UINT m_numPatchVertCols;

	XMFLOAT4X4 m_world;

	Material m_material;

	std::vector<XMFLOAT2> m_patchBoundsY;
};

#endif // Terrain.h