#include "RendererTexture.h"

RendererTexture::RendererTexture()
	:	m_renderTargetTexture(0),
		m_renderTargetView(0),
		m_shaderResourceView(0)
{
}

RendererTexture::~RendererTexture()
{
	ReleaseCOM(m_renderTargetTexture);
	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_shaderResourceView);
}

void RendererTexture::Initialize(ID3D11Device* device, int width, int height)
{
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	ZeroMemory(&texDesc, sizeof(texDesc));

	texDesc.Width            = width;
	texDesc.Height           = height;
	texDesc.MipLevels        = 1;
	texDesc.ArraySize        = 1;
	texDesc.Format           = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage            = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags        = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags   = 0;
	texDesc.MiscFlags        = 0;

	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	HR(device->CreateTexture2D(&texDesc, NULL, &m_renderTargetTexture));
	HR(device->CreateRenderTargetView(m_renderTargetTexture, &rtvDesc, &m_renderTargetView));
	HR(device->CreateShaderResourceView(m_renderTargetTexture, &srvDesc, &m_shaderResourceView));
}

void RendererTexture::SetRenderTarget(ID3D11DeviceContext* dc, ID3D11DepthStencilView* dsv)
{
	dc->OMSetRenderTargets(1, &m_renderTargetView, dsv);
}

void RendererTexture::ClearRenderTarget(ID3D11DeviceContext* dc, ID3D11DepthStencilView* dsv)
{
	float blendFactor[] = {0, 0, 0, 0};

	dc->ClearRenderTargetView(m_renderTargetView, blendFactor);

	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* RendererTexture::GetShaderResourceView()
{
	return m_shaderResourceView;
}