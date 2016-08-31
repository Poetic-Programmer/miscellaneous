//***************************************************************************************
// Effects.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Effects.h"

#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
	: m_fx(0)
{
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();
	
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 
		0, device, &m_fx));
}

Effect::~Effect()
{
	ReleaseCOM(m_fx);
}
#pragma endregion

#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light1Tech    = m_fx->GetTechniqueByName("Light1");
	Light2Tech    = m_fx->GetTechniqueByName("Light2");
	Light3Tech    = m_fx->GetTechniqueByName("Light3");

	Light0TexTech = m_fx->GetTechniqueByName("Light0Tex");
	Light1TexTech = m_fx->GetTechniqueByName("Light1Tex");
	Light2TexTech = m_fx->GetTechniqueByName("Light2Tex");
	Light3TexTech = m_fx->GetTechniqueByName("Light3Tex");

	Light0TexAlphaClipTech = m_fx->GetTechniqueByName("Light0TexAlphaClip");
	Light1TexAlphaClipTech = m_fx->GetTechniqueByName("Light1TexAlphaClip");
	Light2TexAlphaClipTech = m_fx->GetTechniqueByName("Light2TexAlphaClip");
	Light3TexAlphaClipTech = m_fx->GetTechniqueByName("Light3TexAlphaClip");

	Light1FogTech    = m_fx->GetTechniqueByName("Light1Fog");
	Light2FogTech    = m_fx->GetTechniqueByName("Light2Fog");
	Light3FogTech    = m_fx->GetTechniqueByName("Light3Fog");

	Light0TexFogTech = m_fx->GetTechniqueByName("Light0TexFog");
	Light1TexFogTech = m_fx->GetTechniqueByName("Light1TexFog");
	Light2TexFogTech = m_fx->GetTechniqueByName("Light2TexFog");
	Light3TexFogTech = m_fx->GetTechniqueByName("Light3TexFog");

	Light0TexAlphaClipFogTech = m_fx->GetTechniqueByName("Light0TexAlphaClipFog");
	Light1TexAlphaClipFogTech = m_fx->GetTechniqueByName("Light1TexAlphaClipFog");
	Light2TexAlphaClipFogTech = m_fx->GetTechniqueByName("Light2TexAlphaClipFog");
	Light3TexAlphaClipFogTech = m_fx->GetTechniqueByName("Light3TexAlphaClipFog");

	WorldViewProj       = m_fx->GetVariableByName("gWorldViewProj")->AsMatrix();
	World               = m_fx->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose   = m_fx->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform        = m_fx->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW             = m_fx->GetVariableByName("gEyePosW")->AsVector();
	FrameTime           = m_fx->GetVariableByName("gFrameTime")->AsScalar();
	TextureScrollSpeed  = m_fx->GetVariableByName("gTexScrollSpeed")->AsScalar();
	FogColor            = m_fx->GetVariableByName("gFogColor")->AsVector();
	FogStart            = m_fx->GetVariableByName("gFogStart")->AsScalar();
	FogRange            = m_fx->GetVariableByName("gFogRange")->AsScalar();
	DirLights           = m_fx->GetVariableByName("gDirLights");
	Mat                 = m_fx->GetVariableByName("gMaterial");
	DiffuseMap          = m_fx->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap             = m_fx->GetVariableByName("gCubeMap")->AsShaderResource();
}

BasicEffect::~BasicEffect()
{
}
#pragma endregion

#pragma region SkyEffect
SkyEffect::SkyEffect(ID3D11Device* device, const std::wstring& fileName)
	: Effect(device, fileName)
{
	SkyTech       = m_fx->GetTechniqueByName("SkyTech");
	WorldViewProj = m_fx->GetVariableByName("gWorldViewProj")->AsMatrix();
	CubeMap       = m_fx->GetVariableByName("gCubeMap")->AsShaderResource();
	
}

SkyEffect::~SkyEffect()
{
}
#pragma endregion

#pragma region TerrainEffect
TerrainEffect::TerrainEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light1Tech    = m_fx->GetTechniqueByName("Light1");
	Light2Tech    = m_fx->GetTechniqueByName("Light2");
	Light3Tech    = m_fx->GetTechniqueByName("Light3");
	Light1FogTech = m_fx->GetTechniqueByName("Light1Fog");
	Light2FogTech = m_fx->GetTechniqueByName("Light2Fog");
	Light3FogTech = m_fx->GetTechniqueByName("Light3Fog");

	ViewProj           = m_fx->GetVariableByName("gViewProj")->AsMatrix();
	EyePosW            = m_fx->GetVariableByName("gEyePosW")->AsVector();
	FogColor           = m_fx->GetVariableByName("gFogColor")->AsVector();
	FogStart           = m_fx->GetVariableByName("gFogStart")->AsScalar();
	FogRange           = m_fx->GetVariableByName("gFogRange")->AsScalar();
	DirLights          = m_fx->GetVariableByName("gDirLights");
	Mat                = m_fx->GetVariableByName("gMaterial");

	MinDist            = m_fx->GetVariableByName("gMinDist")->AsScalar();
	MaxDist            = m_fx->GetVariableByName("gMaxDist")->AsScalar();
	MinTess            = m_fx->GetVariableByName("gMinTess")->AsScalar();
	MaxTess            = m_fx->GetVariableByName("gMaxTess")->AsScalar();
	TexelCellSpaceU    = m_fx->GetVariableByName("gTexelCellSpaceU")->AsScalar();
	TexelCellSpaceV    = m_fx->GetVariableByName("gTexelCellSpaceV")->AsScalar();
	WorldCellSpace     = m_fx->GetVariableByName("gWorldCellSpace")->AsScalar();
	WorldFrustumPlanes = m_fx->GetVariableByName("gWorldFrustumPlanes")->AsVector();

	LayerMapArray      = m_fx->GetVariableByName("gLayerMapArray")->AsShaderResource();
	BlendMap           = m_fx->GetVariableByName("gBlendMap")->AsShaderResource();
	HeightMap          = m_fx->GetVariableByName("gHeightMap")->AsShaderResource();
	HeightMapOffset    = m_fx->GetVariableByName("gHeightMapOffset")->AsShaderResource();

	Erode              = m_fx->GetVariableByName("gErode")->AsScalar();
	ScaleValue         = m_fx->GetVariableByName("gScaleValue")->AsScalar();
}

TerrainEffect::~TerrainEffect()
{
}
#pragma endregion

#pragma region ParticleEffect
ParticleEffect::ParticleEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	StreamOutTech    = m_fx->GetTechniqueByName("StreamOutTech");
	DrawTech         = m_fx->GetTechniqueByName("DrawTech");

	ViewProj    = m_fx->GetVariableByName("gViewProj")->AsMatrix();
	GameTime    = m_fx->GetVariableByName("gGameTime")->AsScalar();
	TimeStep    = m_fx->GetVariableByName("gTimeStep")->AsScalar();
	EyePosW     = m_fx->GetVariableByName("gEyePosW")->AsVector();
	EmitPosW    = m_fx->GetVariableByName("gEmitPosW")->AsVector();
	EmitDirW    = m_fx->GetVariableByName("gEmitDirW")->AsVector();
	TexArray    = m_fx->GetVariableByName("gTexArray")->AsShaderResource();
	RandomTex   = m_fx->GetVariableByName("gRandomTex")->AsShaderResource();
}

ParticleEffect::~ParticleEffect()
{
}
#pragma endregion

#pragma region BlurEffect
BlurEffect::BlurEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	HorzBlurTech = m_fx->GetTechniqueByName("HorzBlur");
	VertBlurTech = m_fx->GetTechniqueByName("VertBlur");

	Weights     = m_fx->GetVariableByName("gWeights")->AsScalar();
	Time        = m_fx->GetVariableByName("gTime")->AsScalar();
	InputMap    = m_fx->GetVariableByName("gInput")->AsShaderResource();
	OutputMap   = m_fx->GetVariableByName("gOutput")->AsUnorderedAccessView();
}

BlurEffect::~BlurEffect()
{
}
#pragma endregion

#pragma region Effects

BasicEffect*    Effects::BasicFX   = 0;
SkyEffect*      Effects::SkyFX     = 0;
TerrainEffect*  Effects::TerrainFX = 0;
ParticleEffect* Effects::FireFX   = 0;
ParticleEffect* Effects::RainFX   = 0;
BlurEffect*       Effects::BlurFX       = 0;

void Effects::InitializeAll(ID3D11Device* device)
{
	BasicFX   = new BasicEffect(device, L"FX/Basic.fxo");
	SkyFX     = new SkyEffect(device, L"FX/Sky.fxo");
	TerrainFX = new TerrainEffect(device, L"FX/Terrain.fxo");
	FireFX    = new ParticleEffect(device, L"FX/Fire.fxo");
	RainFX    = new ParticleEffect(device, L"FX/Rain.fxo");
	BlurFX  = new BlurEffect(device, L"FX/Blur.fxo");
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
	SafeDelete(SkyFX);
	SafeDelete(TerrainFX);
	SafeDelete(FireFX);
	SafeDelete(RainFX);
	SafeDelete(BlurFX);
}
#pragma endregion