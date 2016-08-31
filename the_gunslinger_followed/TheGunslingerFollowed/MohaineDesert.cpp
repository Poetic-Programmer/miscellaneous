#include "MohaineDesert.h"
#include "Camera.h"
#include "LightHelper.h"
#include "GeometryGenerator.h"

MohaineDesert::MohaineDesert()
{

}

MohaineDesert::~MohaineDesert()
{

}

void MohaineDesert::Initialize(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	// Store layer texture ids.
	std::vector<std::wstring> tex;
	tex.push_back(L"Textures/darkdirt.dds");
	tex.push_back(L"Textures/lightdirt.dds");
	tex.push_back(L"Textures/darkdirt.dds");
	tex.push_back(L"Textures/stone.dds");

	// Set up terrain info.
	Terrain::InitInfo info;
	info.BasemapWidth    = 2049;
	info.BasemapHeight   = 2049;
	info.BasemapScale    = 200;
	info.OffsetmapWidth  = 512;
	info.OffsetmapHeight = 512;
	info.OffsetmapScale  = 10;
	info.HeightmapFileId = L"Textures/Desert Outter.raw";
	info.TextureLayerIds = tex;
	info.BlendmapId      = L"Textures/blend.dds";
	tex.clear();

	// Initialize terrain.
	m_terrain.Initialize(device, dc, info);	
}

void MohaineDesert::Update(float deltaTime)
{
	m_terrain.Update(deltaTime);
}

void MohaineDesert::Render(ID3D11DeviceContext* dc, Camera& cam, DirectionalLight lights[3])
{
	m_terrain.Render(dc, cam, lights);
}

float MohaineDesert::GetHeight(float x, float y)
{
	return m_terrain.GetHeight(x, y);
}