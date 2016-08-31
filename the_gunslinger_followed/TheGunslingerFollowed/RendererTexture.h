#ifndef RENDERERTEXTURE_H
#define RENDERERTEXTURE_H

#include "D3DApp.h"

class RendererTexture
{
public:
	RendererTexture();
	~RendererTexture();

	void Initialize(ID3D11Device* device, int width, int height);
	void ShutDown();

	void SetRenderTarget(ID3D11DeviceContext* dc, ID3D11DepthStencilView* dsv);
	void ClearRenderTarget(ID3D11DeviceContext* dc, ID3D11DepthStencilView* dsv);
	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
};

#endif // RendererTexture.h