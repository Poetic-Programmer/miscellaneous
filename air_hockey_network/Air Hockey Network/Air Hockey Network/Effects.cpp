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

	WorldViewProj     = m_fx->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = m_fx->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = m_fx->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = m_fx->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = m_fx->GetVariableByName("gEyePosW")->AsVector();
	FogColor          = m_fx->GetVariableByName("gFogColor")->AsVector();
	FogStart          = m_fx->GetVariableByName("gFogStart")->AsScalar();
	FogRange          = m_fx->GetVariableByName("gFogRange")->AsScalar();
	DirLights         = m_fx->GetVariableByName("gDirLights");
	Mat               = m_fx->GetVariableByName("gMaterial");
	DiffuseMap        = m_fx->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap           = m_fx->GetVariableByName("gCubeMap")->AsShaderResource();
}

BasicEffect::~BasicEffect()
{
}
#pragma endregion

#pragma region Effects

BasicEffect*    Effects::BasicFX   = 0;

void Effects::InitializeAll(ID3D11Device* device)
{
	BasicFX   = new BasicEffect(device, L"FX/Basic.fxo");
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
}
#pragma endregion