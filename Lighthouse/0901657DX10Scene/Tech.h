#ifndef TECH_H
#define TECH_H

#include "d3dUtil.h"
#include "Effects.h"

namespace TextureAlpha
{
	void InitializeVariables();

	extern ID3D10EffectTechnique* Tech;
	extern ID3D10EffectMatrixVariable* fxWorldVar;
	extern ID3D10EffectMatrixVariable* fxWVPVar;
	extern ID3D10EffectVariable* fxEyePosVar;
	extern ID3D10EffectVariable* fxLightVar;
	extern ID3D10EffectShaderResourceVariable* fxDiffuseMapVar;
	extern ID3D10EffectShaderResourceVariable* fxSpecMapVar;
	extern ID3D10EffectMatrixVariable* fxTexMtxVar;
};

namespace CubeMap
{
	void InitialIzeVariables();

	extern ID3D10EffectTechnique* Tech;
	extern ID3D10EffectVariable* fxLightVar;
	extern ID3D10EffectVariable* fxEyePosVar;
	extern ID3D10EffectMatrixVariable* fxWVPVar;
	extern ID3D10EffectMatrixVariable* fxWorldVar;
	extern ID3D10EffectMatrixVariable* fxTexMtxVar;
	extern ID3D10EffectVectorVariable* fxReflectMtrlVar;
	extern ID3D10EffectScalarVariable* fxCubeMapEnabledVar;
	extern ID3D10EffectShaderResourceVariable* fxDiffuseMapVar;
	extern ID3D10EffectShaderResourceVariable* fxSpecMapVar;
	extern ID3D10EffectShaderResourceVariable* fxCubeMapVar;
};

namespace SkyMap
{
	void InitializeVariables();

	extern ID3D10EffectTechnique* Tech;
	extern ID3D10EffectMatrixVariable* fxWVPVar;
	extern ID3D10EffectShaderResourceVariable* fxCubeMapVar;
}

namespace Lighting
{
	void InitializeVariables();

	extern ID3D10Effect* FX;
	extern ID3D10EffectTechnique* Tech;
	extern ID3D10EffectMatrixVariable* fxWVPVar;
	extern ID3D10EffectMatrixVariable* fxWorldVar;
	extern ID3D10EffectVariable* fxEyePosVar;
	extern ID3D10EffectVariable* fxLightVar;
	extern ID3D10EffectScalarVariable* fxLightType;
};

#endif