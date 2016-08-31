#include "InputLayouts.h"
#include "Effects.h"

ID3D10InputLayout* InputLayout::Pos               = 0;
ID3D10InputLayout* InputLayout::PosNormalTex      = 0;
ID3D10InputLayout* InputLayout::PosNormalDiffSpec = 0;

void InputLayout::InitAll(ID3D10Device* device)
{
	D3D10_PASS_DESC PassDesc;

	//
	// SkyVertex
	//
 	D3D10_INPUT_ELEMENT_DESC skyVertexDesc[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	fx::SkyFX->GetTechniqueByName("SkyTech")->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(device->CreateInputLayout(skyVertexDesc, 1, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &Pos));

	// 
	// Vertex
	//
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	fx::TextureFX->GetTechniqueByName("TexAlphaTech")->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(device->CreateInputLayout(vertexDesc, 3, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &PosNormalTex));

	//
	// Lighting
	//
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC lightvertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
	fx::LightingFX->GetTechniqueByName("LightTech")->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(device->CreateInputLayout(lightvertexDesc, 4, PassDesc.pIAInputSignature,
	    PassDesc.IAInputSignatureSize, &PosNormalDiffSpec));
}

void InputLayout::DestroyAll()
{
	ReleaseCOM(Pos);
	ReleaseCOM(PosNormalTex);
	ReleaseCOM(PosNormalDiffSpec);
}