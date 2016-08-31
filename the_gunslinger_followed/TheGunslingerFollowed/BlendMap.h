//***************************************************************************************
// 'Blandmap Class' by Michael Dunleavy          Last Edit: 03/05/2013
//
// This class is responsible for loading and storing the textures used for the blending
// calculations as well as the blend map itself.  
//***************************************************************************************

#ifndef BLENDMAP_H
#define BLENDMAP_H

#include "D3DUtil.h"

class BlendMap
{
public:
	BlendMap();
	~BlendMap();

	BlendMap& operator=(const BlendMap& other);

	void AddBlendTexture(std::wstring filename);
	void AddLayerTexture(std::wstring filename);
	void AddLayerTextures(std::vector<std::wstring> textures);

	void CreateShaderResourceViews(ID3D11Device* device, ID3D11DeviceContext* dc);

	ID3D11ShaderResourceView* GetLayermapSRV();
	ID3D11ShaderResourceView* GetBlendmapSRV();

private:
	ID3D11ShaderResourceView* m_blendmapSRV;
	ID3D11ShaderResourceView* m_layermapSRV;

	std::wstring m_blendmapId;
	std::vector<std::wstring> m_textureIds;
};

#endif // Blendmap