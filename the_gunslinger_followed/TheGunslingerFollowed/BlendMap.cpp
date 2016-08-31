#include "BlendMap.h"

BlendMap::BlendMap()
	: m_blendmapSRV(0), m_layermapSRV(0)
{
}

BlendMap::~BlendMap()
{
	ReleaseCOM(m_blendmapSRV);
	ReleaseCOM(m_layermapSRV);
}

BlendMap& BlendMap::operator=(const BlendMap& other)
{
	if(this == &other)
		return *this;

	m_blendmapSRV = other.m_blendmapSRV;
	m_layermapSRV = other.m_layermapSRV;

	m_blendmapId = other.m_blendmapId;
	m_textureIds = other.m_textureIds;

	return *this;
}

void BlendMap::AddBlendTexture(std::wstring filename)
{
	m_blendmapId = filename;
}

void BlendMap::AddLayerTexture(std::wstring filename)
{
	m_textureIds.push_back(filename);
}

void BlendMap::AddLayerTextures(std::vector<std::wstring> textures)
{
	m_textureIds = textures;
}

void BlendMap::CreateShaderResourceViews(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	m_layermapSRV = d3dHelper::CreateTexture2DArraySRV(device, dc, m_textureIds);

	HR(D3DX11CreateShaderResourceViewFromFile(device, 
		m_blendmapId.c_str(), 0, 0, &m_blendmapSRV, 0));
}

ID3D11ShaderResourceView* BlendMap::GetBlendmapSRV()
{
	return m_blendmapSRV;
}

ID3D11ShaderResourceView* BlendMap::GetLayermapSRV()
{
	return m_layermapSRV;
}
