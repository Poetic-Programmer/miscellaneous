#ifndef LDPRENDERSTATES_H
#define LDPRENDERSTATES_H

#include "D3DUtil.h"

class RenderStates
{
public:
	static void InitializeAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11RasterizerState* WireframeRS;
	static ID3D11RasterizerState* NoCullRS;

	static ID3D11DepthStencilState* EqualsDSS;
	 
	static ID3D11BlendState* AlphaToCoverageBS;
	static ID3D11BlendState* TransparentBS;
};

#endif // LDPRenderStates_h