#include "States.h"

ID3D10BlendState*      states::TransparentBS    = 0;
ID3D10RasterizerState* states::WireframeRS      = 0;
ID3D10RasterizerState* states::SolidframeRS     = 0;
ID3D10RasterizerState* states::NoCullRS         = 0;

void states::InitAll(ID3D10Device* device)
{
	///////////////////////////////////////////////////////////////////
	//CREATE BLEND STATES
	///////////////////////////////////////////////////////////////////
	D3D10_BLEND_DESC blendDesc = {0};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.BlendEnable[0] = true;
	blendDesc.SrcBlend       = D3D10_BLEND_SRC_ALPHA;
	blendDesc.DestBlend      = D3D10_BLEND_INV_SRC_ALPHA;
	blendDesc.BlendOp        = D3D10_BLEND_OP_ADD;
	blendDesc.SrcBlendAlpha  = D3D10_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	blendDesc.BlendOpAlpha   = D3D10_BLEND_OP_ADD;
	blendDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	HR(device->CreateBlendState(&blendDesc, &TransparentBS));

	///////////////////////////////////////////////////////////////////
	//CREATE RASTERIZER STATES
	///////////////////////////////////////////////////////////////////
	D3D10_RASTERIZER_DESC  frameDesc;
	ZeroMemory(&frameDesc, sizeof(D3D10_RASTERIZER_DESC));
	frameDesc.FillMode = D3D10_FILL_SOLID;
	frameDesc.CullMode = D3D10_CULL_BACK;
	frameDesc.FrontCounterClockwise = false;
	frameDesc.DepthClipEnable = true;
	HR(device->CreateRasterizerState(&frameDesc, &SolidframeRS));

	ZeroMemory(&frameDesc, sizeof(D3D10_RASTERIZER_DESC));
	frameDesc.FillMode = D3D10_FILL_WIREFRAME;
	frameDesc.CullMode = D3D10_CULL_BACK;
	frameDesc.FrontCounterClockwise = false;
	frameDesc.DepthClipEnable = true;
	HR(device->CreateRasterizerState(&frameDesc, &WireframeRS));

	ZeroMemory(&frameDesc, sizeof(D3D10_RASTERIZER_DESC));
	frameDesc.FillMode = D3D10_FILL_SOLID;
	frameDesc.CullMode = D3D10_CULL_NONE;
	HR(device->CreateRasterizerState(&frameDesc, &NoCullRS));
}

void states::DestroyAll()
{
	ReleaseCOM(TransparentBS);
	ReleaseCOM(WireframeRS);
	ReleaseCOM(SolidframeRS);
	ReleaseCOM(NoCullRS);
}