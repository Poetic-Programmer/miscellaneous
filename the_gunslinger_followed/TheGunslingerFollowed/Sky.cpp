#include "Sky.h"
#include "GeometryGenerator.h"
#include "Camera.h"
#include "Vertex.h"
#include "Effects.h"

Sky::Sky(ID3D11Device* device, const std::wstring& cubeMapFileName, float sphereRadius)
{
	HR(D3DX11CreateShaderResourceViewFromFile(device, cubeMapFileName.c_str(), 0, 0, &m_cubeMapSRV, 0));

	GeometryGenerator::MeshData sphere;
	GeometryGenerator geoGen;
	geoGen.CreateSphere(sphereRadius, 30, 30, sphere);

	std::vector<XMFLOAT3> vertices(sphere.Vertices.size());

	for(size_t i = 0; i < sphere.Vertices.size(); ++i)
	{
		vertices[i] = sphere.Vertices[i].Position;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3) * vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];

    HR(device->CreateBuffer(&vbd, &vinitData, &m_vb));

	m_indexCount = sphere.Indices.size();

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(USHORT) * m_indexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
    ibd.MiscFlags = 0;

	std::vector<USHORT> indices16;
	indices16.assign(sphere.Indices.begin(), sphere.Indices.end());

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices16[0];

    HR(device->CreateBuffer(&ibd, &iinitData, &m_ib));
}

Sky::~Sky()
{
	ReleaseCOM(m_vb);
	ReleaseCOM(m_ib);
	ReleaseCOM(m_cubeMapSRV);
}

ID3D11ShaderResourceView* Sky::CubeMapSRV()
{
	return m_cubeMapSRV;
}

void Sky::Render(ID3D11DeviceContext* dc, const Camera& camera)
{
	// center Sky about eye in world space
	XMFLOAT3 eyePos = camera.GetPosition();
	XMMATRIX T = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);


	XMMATRIX WVP = XMMatrixMultiply(T, camera.ViewProj());

	Effects::SkyFX->SetWorldViewProj(WVP);
	Effects::SkyFX->SetCubeMap(m_cubeMapSRV);
	
	UINT stride = sizeof(XMFLOAT3);
    UINT offset = 0;
    dc->IASetVertexBuffers(0, 1, &m_vb, &stride, &offset);
	dc->IASetIndexBuffer(m_ib, DXGI_FORMAT_R16_UINT, 0);
	dc->IASetInputLayout(InputLayouts::Pos);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	D3DX11_TECHNIQUE_DESC techDesc;
    Effects::SkyFX->SkyTech->GetDesc( &techDesc );

    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        ID3DX11EffectPass* pass = Effects::SkyFX->SkyTech->GetPassByIndex(p);

		pass->Apply(0, dc);

		dc->DrawIndexed(m_indexCount, 0, 0);
	}
}