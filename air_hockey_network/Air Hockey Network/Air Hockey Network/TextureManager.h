#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "D3DUtil.h"
#include <map>

class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	void Initialize(ID3D11Device* device);

	ID3D11ShaderResourceView* CreateTexture(std::wstring fileName);

private:
	TextureManager(const TextureManager& other);
	TextureManager& operator =(const TextureManager& other);

private:
	ID3D11Device* m_d3dDevice;
	std::map<std::wstring, ID3D11ShaderResourceView*> m_textureSRV;
};

#endif // TEXTUREMANAGER