#include "Effects.h"

ID3D10Effect* fx::TextureFX           = 0;
ID3D10Effect* fx::SkyFX               = 0;
ID3D10Effect* fx::CubeMapFX           = 0;
ID3D10Effect* fx::LightingFX          = 0;

ID3D10Effect* CreateFX(ID3D10Device* device, std::wstring filename)
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
  
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	ID3D10Effect* fx = 0;
	hr = D3DX10CreateEffectFromFile(filename.c_str(), 0, 0, 
		"fx_4_0", shaderFlags, 0, device, 0, 0, &fx, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, filename.c_str(), true);
	}

	return fx;
}

void fx::InitAll(ID3D10Device* device)
{
	TextureFX  = CreateFX(device, L"FX/texAlpha.fx");
	SkyFX      = CreateFX(device, L"FX/sky.fx");
	CubeMapFX  = CreateFX(device, L"FX/cubemap.fx");
	LightingFX = CreateFX(device, L"FX/lighting.fx");
}

void fx::DestroyAll()
{
	ReleaseCOM(TextureFX);
	ReleaseCOM(SkyFX);
	ReleaseCOM(CubeMapFX);
	ReleaseCOM(LightingFX);
}