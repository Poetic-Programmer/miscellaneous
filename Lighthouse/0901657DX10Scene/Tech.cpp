#include "Tech.h"

////////////////////////////////////////////////////////////////////////
//TEXTURE ALPHA VARIBALES
////////////////////////////////////////////////////////////////////////
ID3D10EffectTechnique*              TextureAlpha::Tech             = 0;
ID3D10EffectMatrixVariable*         TextureAlpha::fxWorldVar       = 0;
ID3D10EffectMatrixVariable*         TextureAlpha::fxWVPVar         = 0;
ID3D10EffectVariable*               TextureAlpha::fxEyePosVar      = 0;
ID3D10EffectVariable*               TextureAlpha::fxLightVar       = 0;
ID3D10EffectShaderResourceVariable* TextureAlpha::fxDiffuseMapVar  = 0;
ID3D10EffectShaderResourceVariable* TextureAlpha::fxSpecMapVar     = 0;
ID3D10EffectMatrixVariable*         TextureAlpha::fxTexMtxVar      = 0;

////////////////////////////////////////////////////////////////////////
//CUBE MAP VARIBALES
////////////////////////////////////////////////////////////////////////
ID3D10EffectTechnique*              CubeMap::Tech                  = 0;
ID3D10EffectVariable*               CubeMap::fxLightVar            = 0; 
ID3D10EffectVariable*               CubeMap::fxEyePosVar           = 0;
ID3D10EffectMatrixVariable*         CubeMap::fxWVPVar              = 0;
ID3D10EffectMatrixVariable*         CubeMap::fxWorldVar            = 0;
ID3D10EffectMatrixVariable*         CubeMap::fxTexMtxVar           = 0;
ID3D10EffectVectorVariable*         CubeMap::fxReflectMtrlVar      = 0;
ID3D10EffectScalarVariable*         CubeMap::fxCubeMapEnabledVar   = 0;
ID3D10EffectShaderResourceVariable* CubeMap::fxDiffuseMapVar       = 0;
ID3D10EffectShaderResourceVariable* CubeMap::fxSpecMapVar          = 0;
ID3D10EffectShaderResourceVariable* CubeMap::fxCubeMapVar          = 0;

////////////////////////////////////////////////////////////////////////
//SKY MAP VARIABLES
////////////////////////////////////////////////////////////////////////
ID3D10EffectTechnique*              SkyMap::Tech         = 0;
ID3D10EffectMatrixVariable*         SkyMap::fxWVPVar     = 0;
ID3D10EffectShaderResourceVariable* SkyMap::fxCubeMapVar = 0;

////////////////////////////////////////////////////////////////////////
//LIGHTING VARIABLES
////////////////////////////////////////////////////////////////////////
ID3D10Effect*               Lighting::FX          = 0;
ID3D10EffectTechnique*      Lighting::Tech        = 0;
ID3D10EffectMatrixVariable* Lighting::fxWVPVar    = 0;
ID3D10EffectMatrixVariable* Lighting::fxWorldVar  = 0;
ID3D10EffectVariable*       Lighting::fxEyePosVar = 0;
ID3D10EffectVariable*       Lighting::fxLightVar  = 0;
ID3D10EffectScalarVariable* Lighting::fxLightType = 0;

void TextureAlpha::InitializeVariables()
{
	Tech                    = fx::TextureFX->GetTechniqueByName("TexAlphaTech");
	fxWorldVar              = fx::TextureFX->GetVariableByName("gWorld")->AsMatrix();
	fxWVPVar                = fx::TextureFX->GetVariableByName("gWVP")->AsMatrix();	
	fxEyePosVar             = fx::TextureFX->GetVariableByName("gEyePosW");
	fxLightVar              = fx::TextureFX->GetVariableByName("gLight");
	fxDiffuseMapVar         = fx::TextureFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	fxSpecMapVar            = fx::TextureFX->GetVariableByName("gSpecMap")->AsShaderResource();
	fxTexMtxVar             = fx::TextureFX->GetVariableByName("gTexMtx")->AsMatrix();
}

void CubeMap::InitialIzeVariables()
{
	Tech                = fx::CubeMapFX->GetTechniqueByName("CubeMapTech");
	fxLightVar          = fx::CubeMapFX->GetVariableByName("gLight");
	fxEyePosVar         = fx::CubeMapFX->GetVariableByName("gEyePosW");
	fxWVPVar            = fx::CubeMapFX->GetVariableByName("gWVP")->AsMatrix();
	fxWorldVar          = fx::CubeMapFX->GetVariableByName("gWorld")->AsMatrix();
	fxTexMtxVar         = fx::CubeMapFX->GetVariableByName("gTexMtx")->AsMatrix();
	fxReflectMtrlVar    = fx::CubeMapFX->GetVariableByName("gReflectMtrl")->AsVector();
	fxCubeMapEnabledVar = fx::CubeMapFX->GetVariableByName("gCubeMapEnabled")->AsScalar();
	fxDiffuseMapVar     = fx::CubeMapFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	fxSpecMapVar        = fx::CubeMapFX->GetVariableByName("gSpecMap")->AsShaderResource();
	fxCubeMapVar        = fx::CubeMapFX->GetVariableByName("gCubeMap")->AsShaderResource();
}

void SkyMap::InitializeVariables()
{
	Tech         = fx::SkyFX->GetTechniqueByName("SkyTech");
	fxWVPVar     = fx::SkyFX->GetVariableByName("gWVP")->AsMatrix();
	fxCubeMapVar = fx::SkyFX->GetVariableByName("gCubeMap")->AsShaderResource();
}

void Lighting::InitializeVariables()
{
	Tech        = fx::LightingFX->GetTechniqueByName("LightTech");
	fxWVPVar    = fx::LightingFX->GetVariableByName("gWVP")->AsMatrix();
	fxWorldVar  = fx::LightingFX->GetVariableByName("gWorld")->AsMatrix();
	fxEyePosVar = fx::LightingFX->GetVariableByName("gEyePosW");
	fxLightVar  = fx::LightingFX->GetVariableByName("gLight");
	fxLightType = fx::LightingFX->GetVariableByName("gLightType")->AsScalar();
}