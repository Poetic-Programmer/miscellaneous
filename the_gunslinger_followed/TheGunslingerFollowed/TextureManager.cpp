#include "TextureManager.h"

TextureManager::TextureManager() 
	: m_d3dDevice(0)
{}

TextureManager::~TextureManager()
{
	for(auto it = m_textureSRV.begin(); it != m_textureSRV.end(); ++it)
	{
		ReleaseCOM(it->second);
	}

	m_textureSRV.clear();
}

void TextureManager::Initialize(ID3D11Device* device)
{
	m_d3dDevice = device;
}

ID3D11ShaderResourceView* TextureManager::CreateTexture(std::wstring fileName)
{
	ID3D11ShaderResourceView* srv = 0;

	// Does it exist?
	if(m_textureSRV.find(fileName) != m_textureSRV.end())
	{
		srv = m_textureSRV[fileName];
	}
	else
	{
		HR(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice, fileName.c_str(), 0, 0, &srv, 0));

		m_textureSRV[fileName] = srv;
	}

	return srv;
}