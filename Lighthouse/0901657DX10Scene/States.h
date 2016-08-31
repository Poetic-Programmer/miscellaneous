#ifndef STATES_H
#define STATES_H

#include "d3dUtil.h"

namespace states
{
	void InitAll(ID3D10Device* device);
	void DestroyAll();

	//Blend states
	extern ID3D10BlendState* TransparentBS;

	//Rasterizer states
	extern ID3D10RasterizerState* WireframeRS;
	extern ID3D10RasterizerState* SolidframeRS;	
	extern ID3D10RasterizerState* NoCullRS;
};

#endif