#include "Paddle.h"
#include "Camera.h"
#include "LightHelper.h"
#include "RenderStates.h"

Paddle::Paddle()
	: 
	m_vb(0),
	m_ib(0),

	m_paddleTexSRV(0)
{}

Paddle::~Paddle()
{
	ReleaseCOM(m_vb);
	ReleaseCOM(m_ib);

	ReleaseCOM(m_paddleTexSRV);
}

void Paddle::Initialize(ID3D11Device* device)
{	
	HR(D3DX11CreateShaderResourceViewFromFile(device, 
		L"Textures/base.dds", 0, 0, &m_paddleTexSRV, 0 ));

	XMMATRIX I = XMMatrixIdentity();

	XMMATRIX baseScale = XMMatrixScaling(20.0f, 2.0f, 40.0f);
	XMMATRIX baseOffset = XMMatrixTranslation(0.0f, 8.0f, 0.0f);
	XMStoreFloat4x4(&m_paddleWorldMatrix, XMMatrixMultiply(baseScale, baseOffset));
			
	m_paddleMaterial.Ambient  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_paddleMaterial.Diffuse  = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	m_paddleMaterial.Specular = XMFLOAT4(1.0f, 0.0f, 0.0f, 16.0f);

	BuildGeometryBuffers(device);
}

void Paddle::SetPosition(const Vector3& vec)
{
	m_position3D = vec;
}

void Paddle::Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{
	dc->IASetInputLayout(InputLayouts::Basic32);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;

	XMMATRIX view     = cam.View();
	XMMATRIX proj     = cam.Proj();
	XMMATRIX viewProj = cam.ViewProj();

	XMMATRIX translation = XMMatrixTranslation(m_position3D.x, m_position3D.y, m_position3D.z);
	XMMATRIX scale = XMMatrixScaling(1.0f, 0.5f, 1.0f);
	XMStoreFloat4x4(&m_paddleWorldMatrix, XMMatrixMultiply(scale, translation));

	Effects::BasicFX->SetDirLights(lights);
	Effects::BasicFX->SetEyePosW(cam.GetPosition());
	Effects::BasicFX->SetFogColor(Colours::Yellow);
	Effects::BasicFX->SetFogStart(15.0f);
	Effects::BasicFX->SetFogRange(175.0f);
	
	ID3DX11EffectTechnique* activeTexTech = Effects::BasicFX->Light1TexTech;
	D3DX11_TECHNIQUE_DESC techDesc;

    activeTexTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		dc->IASetVertexBuffers(0, 1, &m_vb, &stride, &offset);
		dc->IASetIndexBuffer(m_ib, DXGI_FORMAT_R32_UINT, 0);
		//dc->RSSetState(RenderStates::NoCullRS);

		// Draw the box.
		XMMATRIX world = XMLoadFloat4x4(&m_paddleWorldMatrix);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(m_paddleMaterial);
		Effects::BasicFX->SetDiffuseMap(m_paddleTexSRV);
		
		activeTexTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(m_paddleIndexCount, m_paddleIndexOffset, m_paddleVertexOffset);

		activeTexTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(m_handleIndexCount, m_handleIndexOffset, m_handleVertexOffset);

		//dc->RSSetState(0);
		dc->OMSetBlendState(0, blendFactor, 0xffffffff);
    }
}

void Paddle::BuildGeometryBuffers(ID3D11Device* device)
{
	GeometryGenerator::MeshData cylinder;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator geoGen;
	geoGen.CreateCylinder(1.0f, 1.0f, 1.0f, 40, 40, cylinder);
	geoGen.CreateGeoSphere(0.75f, 40, sphere);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	m_paddleVertexOffset      = 0;
	m_handleVertexOffset      = cylinder.Vertices.size();

	// Cache the index count of each object.
	m_paddleIndexCount    = cylinder.Indices.size();
	m_handleIndexCount    = sphere.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	m_paddleIndexOffset     = 0;
	m_handleIndexOffset     = cylinder.Indices.size();

	UINT totalVertexCount = 
		cylinder.Vertices.size() +
		sphere.Vertices.size();

	UINT totalIndexCount = 
		m_paddleIndexCount +
		m_handleIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);

	UINT k = 0;
	for(size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
		vertices[k].Tex    = cylinder.Vertices[i].TexC;
	}
	for(size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = sphere.Vertices[i].Position;
		vertices[k].Pos.y += 0.5f;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].Tex    = sphere.Vertices[i].TexC;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(device->CreateBuffer(&vbd, &vinitData, &m_vb));

	
	// Pack the indices of all the meshes into one index buffer.
	std::vector<UINT> indices;
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(device->CreateBuffer(&ibd, &iinitData, &m_ib));
}