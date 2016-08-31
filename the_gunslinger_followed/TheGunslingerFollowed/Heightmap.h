//***************************************************************************************
// 'Heightmap Class' by Michael Dunleavy          Last Edit: 03/05/2013
//
// This class is responsible for setting up the different methods for creating heightmap
// offsets.  It currently can create heightmap values using the noise class or create
// the offsets from a grey scale texture.  Once the textures are loaded, a copy of the
// dds file will be saved in the main directory to confirm the visual appearence.
//***************************************************************************************

#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include "D3DApp.h"
#include "D3DUtil.h"
#include "Noise.h"

class Heightmap
{
public:
	Heightmap();
	Heightmap(UINT width, UINT height, float scale);
	~Heightmap();

	Heightmap& operator=(const Heightmap& other);

	void Initialize(ID3D11Device* device, ID3D11DeviceContext* dc);
	void CreateShaderResourceView(ID3D11Device* device, ID3D11DeviceContext* dc);

	void CreateHeightmapFromFile(std::wstring filename);
	void CreateHeightmapFromPerlinNoise();

	void Update(float deltaTime);

	UINT GetWidth()const;
	UINT GetHeight()const;

	std::vector<float> GetHeightValues();
	float GetIndexValue(int index)const;
	float GetHeightValue(int row, int col)const;

	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	void Smooth();
	bool InBounds(int i, int j);
	float Average(int i, int j);

private:
	std::vector<float> m_heightValues;
	Noise m_noise;

	UINT  m_width;
	UINT  m_height;
	float m_scale;

private:
	ID3D11ShaderResourceView* m_heightmapSRV;
	ID3D11Texture2D* m_texture;
};

#endif // Heightmap.h