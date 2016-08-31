#ifndef SKY_H
#define SKY_H

#include "D3DUtil.h"

class Camera;

class Sky
{
public:
	Sky(ID3D11Device* device, const std::wstring& cubeMapFileName, float sphereRadius);
	~Sky();

	ID3D11ShaderResourceView* CubeMapSRV();

	void Render(ID3D11DeviceContext* dc, const Camera& cam);

private:
	Sky(const Sky& other);
	Sky& operator =(const Sky& other);

private:
	ID3D11Buffer* m_vb;
	ID3D11Buffer* m_ib;

	ID3D11ShaderResourceView* m_cubeMapSRV;

	UINT m_indexCount;
};

#endif