#include "Terrain.h"
#include "Camera.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include <fstream>
#include <sstream>
#include "Heightmap.h"

Terrain::Terrain() : 
	m_quadPatchVB(0), 
	m_quadPatchIB(0), 
	m_numPatchVertices(0),
	m_numPatchQuadFaces(0),
	m_numPatchVertRows(0),
	m_numPatchVertCols(0)
{
	XMStoreFloat4x4(&m_world, XMMatrixIdentity());

	m_material.Ambient  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 64.0f);
	m_material.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	m_time = 0;
	m_erode = false;
	m_scaleValue = 0.01f;
	m_cellSpacing = 0.5f;
}

Terrain::~Terrain()
{
	ReleaseCOM(m_quadPatchVB);
	ReleaseCOM(m_quadPatchIB);
}

float Terrain::GetWidth()const
{
	return(m_basemap.GetWidth() - 1) * m_cellSpacing;
}

float Terrain::GetDepth()const
{
	return(m_basemap.GetHeight() - 1) * m_cellSpacing;
}

float Terrain::GetHeight(float x, float z)const
{
	int offsetScaleX = m_info.BasemapWidth / m_info.OffsetmapWidth;
	int offsetScaleY = m_info.BasemapHeight / m_info.OffsetmapHeight;

	// Transform from local space to "cell" space
	float c = (x + 0.5f * GetWidth()) / m_cellSpacing;
	float d = (z - 0.5f * GetDepth()) / -m_cellSpacing;

	// Get the current row and column
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Get the heights of the current cell.
	float A = m_basemap.GetHeightValue(row, col) + (m_offsetmap.GetHeightValue(row / offsetScaleX, col / offsetScaleY) * m_scaleValue);
	float B = m_basemap.GetHeightValue(row, col + 1) + (m_offsetmap.GetHeightValue(row / offsetScaleX, (col / offsetScaleY) + 1) * m_scaleValue);//m_heightmap[row * m_info.HeightmapWidth + col + 1];// + m_heightmapOffset[0];
	float C = m_basemap.GetHeightValue(row + 1, col) + (m_offsetmap.GetHeightValue((row / offsetScaleX) + 1, col / offsetScaleY) * m_scaleValue);//m_heightmap[(row + 1) * m_info.HeightmapWidth + col];// + m_heightmapOffset[0];
	float D = m_basemap.GetHeightValue(row + 1, col + 1) + (m_offsetmap.GetHeightValue((row / offsetScaleX) + 1, (col / offsetScaleY) + 1) * m_scaleValue);//m_heightmap[(row + 1) * m_info.HeightmapWidth + col + 1];// + m_heightmapOffset[0];

	// Where we are relative to the cell
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC
	if( s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s*uy + t*vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f-s)*uy + (1.0f-t)*vy;
	}
}

XMMATRIX Terrain::GetWorld()const
{
	return XMLoadFloat4x4(&m_world);
}

void Terrain::SetWorld(CXMMATRIX M)
{
	XMStoreFloat4x4(&m_world, M);
}

void Terrain::Initialize(ID3D11Device* device, ID3D11DeviceContext* dc, InitInfo& info)
{
	m_info = info;

	m_basemap   = Heightmap(m_info.BasemapWidth, m_info.BasemapHeight, m_info.BasemapScale);
	m_offsetmap = Heightmap(m_info.OffsetmapWidth, m_info.OffsetmapHeight, m_info.OffsetmapScale);
	m_blendmap  = BlendMap();

	m_basemap.CreateHeightmapFromFile(m_info.HeightmapFileId);
	//m_basemap.CreateHeightmapFromPerlinNoise();
	m_basemap.CreateShaderResourceView(device, dc);

	m_offsetmap.CreateHeightmapFromPerlinNoise();
	m_offsetmap.CreateShaderResourceView(device, dc);

	m_blendmap.AddLayerTextures(m_info.TextureLayerIds);
	m_blendmap.AddBlendTexture(m_info.BlendmapId);
	m_blendmap.CreateShaderResourceViews(device, dc);

	// Divide heightmap into patches
	m_numPatchVertRows = ((m_basemap.GetHeight() - 1) / CellsPerPatch) + 1;
	m_numPatchVertCols = ((m_basemap.GetWidth() - 1) / CellsPerPatch) + 1;

	m_numPatchVertices  = m_numPatchVertRows * m_numPatchVertCols;
	m_numPatchQuadFaces = (m_numPatchVertRows - 1) * (m_numPatchVertCols - 1);

	CalcAllPatchBoundsY();

	BuildQuadPatchVB(device, dc);
	BuildQuadPatchIB(device);
}

void Terrain::Update(float deltaTime)
{
	if(m_scaleValue < 1)
		m_scaleValue += (deltaTime) / 100;
	if(m_scaleValue >= 1)
		m_scaleValue = 1;
}

void Terrain::Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	dc->IASetInputLayout(InputLayouts::Terrain);

	UINT stride = sizeof(Vertex::Terrain);
    UINT offset = 0;

    dc->IASetVertexBuffers(0, 1, &m_quadPatchVB, &stride, &offset);
	dc->IASetIndexBuffer(m_quadPatchIB, DXGI_FORMAT_R16_UINT, 0);

	XMMATRIX viewProj = cam.ViewProj();
	XMMATRIX world  = XMLoadFloat4x4(&m_world);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX worldViewProj = world*viewProj;

	XMFLOAT4 worlanes[6];
	ExtractFrustumPlanes(worlanes, viewProj);

	// Set per frame constants.
	Effects::TerrainFX->SetViewProj(viewProj);
	Effects::TerrainFX->SetEyePosW(cam.GetPosition());
	Effects::TerrainFX->SetDirLights(lights);
	Effects::TerrainFX->SetFogColor(Colours::Sand);
	Effects::TerrainFX->SetFogStart(15.0f);
	Effects::TerrainFX->SetFogRange(175.0f);
	Effects::TerrainFX->SetMinDist(20.0f);
	Effects::TerrainFX->SetMaxDist(500.0f);
	Effects::TerrainFX->SetMinTess(0.0f);
	Effects::TerrainFX->SetMaxTess(6.0f);
	Effects::TerrainFX->SetTexelCellSpaceU(1.0f / m_basemap.GetWidth());
	Effects::TerrainFX->SetTexelCellSpaceV(1.0f / m_basemap.GetHeight());
	Effects::TerrainFX->SetWorldCellSpace(m_cellSpacing);
	
	Effects::TerrainFX->SetLayerMapArray(m_blendmap.GetLayermapSRV());
	Effects::TerrainFX->SetBlendMap(m_blendmap.GetBlendmapSRV());
	Effects::TerrainFX->SetHeightMap(m_basemap.GetShaderResourceView());
	Effects::TerrainFX->SetHeightMapOffset(m_offsetmap.GetShaderResourceView());

	Effects::TerrainFX->SetMaterial(m_material);
	
	Effects::TerrainFX->SetErodeVariable(m_erode);
	Effects::TerrainFX->SetScaleVariable(m_scaleValue);

	ID3DX11EffectTechnique* tech = Effects::TerrainFX->Light3FogTech;
    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc( &techDesc );
	
    for(UINT i = 0; i < techDesc.Passes; ++i)
    {
        ID3DX11EffectPass* pass = tech->GetPassByIndex(i);
		pass->Apply(0, dc);

		dc->DrawIndexed(m_numPatchQuadFaces*4, 0, 0);
	}	

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	dc->HSSetShader(0, 0, 0);
	dc->DSSetShader(0, 0, 0);
}

void Terrain::CalcAllPatchBoundsY()
{
	m_patchBoundsY.resize(m_numPatchQuadFaces);

	// For each patch
	for(UINT i = 0; i < m_numPatchVertRows-1; ++i)
	{
		for(UINT j = 0; j < m_numPatchVertCols-1; ++j)
		{
			CalcPatchBoundsY(i, j);
		}
	}
}

void Terrain::CalcPatchBoundsY(UINT i, UINT j)
{
	// Scan the heightmap values this patch covers and compute the min/max height.

	UINT x0 = j*CellsPerPatch;
	UINT x1 = (j+1)*CellsPerPatch;

	UINT y0 = i*CellsPerPatch;
	UINT y1 = (i+1)*CellsPerPatch;

	float minY = +MathHelper::Infinity;
	float maxY = -MathHelper::Infinity;
	for(UINT y = y0; y <= y1; ++y)
	{
		for(UINT x = x0; x <= x1; ++x)
		{
			UINT k = y * m_basemap.GetWidth() + x;
			k;

			minY = MathHelper::Min(minY, m_basemap.GetIndexValue(k));
			maxY = MathHelper::Max(maxY, m_basemap.GetIndexValue(k));

		}
	}

	UINT patchID = i*(m_numPatchVertCols-1)+j;
	m_patchBoundsY[patchID] = XMFLOAT2(minY, maxY);
}

void Terrain::BuildQuadPatchVB(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	std::vector<Vertex::Terrain> patchVertices(m_numPatchVertRows*m_numPatchVertCols);

	float halfWidth = 0.5f*GetWidth();
	float halfDepth = 0.5f*GetDepth();

	float patchWidth = GetWidth() / (m_numPatchVertCols-1);
	float patchDepth = GetDepth() / (m_numPatchVertRows-1);
	float du = 1.0f / (m_numPatchVertCols-1);
	float dv = 1.0f / (m_numPatchVertRows-1);

	for(UINT i = 0; i < m_numPatchVertRows; ++i)
	{
		float z = halfDepth - i*patchDepth;
		for(UINT j = 0; j < m_numPatchVertCols; ++j)
		{
			float x = -halfWidth + j*patchWidth;

			patchVertices[i*m_numPatchVertCols+j].Pos = XMFLOAT3(x, 0.0f, z);

			// Stretch texture over grid.
			patchVertices[i*m_numPatchVertCols+j].Tex.x = j*du;
			patchVertices[i*m_numPatchVertCols+j].Tex.y = i*dv;
		}
	}

	// Store axis-aligned bounding box y-bounds in upper-left patch corner.
	for(UINT i = 0; i < m_numPatchVertRows-1; ++i)
	{
		for(UINT j = 0; j < m_numPatchVertCols-1; ++j)
		{
			UINT patchID = i * (m_numPatchVertCols -1 ) + j;
			patchVertices[i * m_numPatchVertCols+j].BoundsY = m_patchBoundsY[patchID];
		}
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex::Terrain) * patchVertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &patchVertices[0];
    HR(device->CreateBuffer(&vbd, &vinitData, &m_quadPatchVB));
}

void Terrain::BuildQuadPatchIB(ID3D11Device* device)
{
	std::vector<USHORT> indices(m_numPatchQuadFaces*4); // 4 indices per quad face

	// Iterate over each quad and compute indices.
	int k = 0;
	for(UINT i = 0; i < m_numPatchVertRows-1; ++i)
	{
		for(UINT j = 0; j < m_numPatchVertCols-1; ++j)
		{
			// Top row of 2x2 quad patch
			indices[k]   = i * m_numPatchVertCols+j;
			indices[k+1] = i * m_numPatchVertCols+j+1;

			// Bottom row of 2x2 quad patch
			indices[k+2] = (i + 1) * m_numPatchVertCols+j;
			indices[k+3] = (i + 1) * m_numPatchVertCols+j+1;

			k += 4; // next quad
		}
	}

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(USHORT) * indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(device->CreateBuffer(&ibd, &iinitData, &m_quadPatchIB));
}