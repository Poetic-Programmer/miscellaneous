#include "Puck.h"
#include "Camera.h"
#include "LightHelper.h"
#include "RenderStates.h"

Puck::Puck()
	: 
	m_vb(0),
	m_ib(0),

	m_puckTexSRV(0)
{}

Puck::~Puck()
{
	ReleaseCOM(m_vb);
	ReleaseCOM(m_ib);

	ReleaseCOM(m_puckTexSRV);
}

void Puck::Initialize(ID3D11Device* device)
{	
	HR(D3DX11CreateShaderResourceViewFromFile(device, 
		L"Textures/base.dds", 0, 0, &m_puckTexSRV, 0 ));

	m_position3D = Vector3(0, 9.0f, 0);
	m_velocity = Vector3(100, 0, 100);
	XMMATRIX I = XMMatrixIdentity();
			
	m_puckMaterial.Ambient  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_puckMaterial.Diffuse  = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	m_puckMaterial.Specular = XMFLOAT4(1.0f, 0.0f, 1.0f, 16.0f);

	m_bounds = Circle(m_position3D.x, m_position3D.z, 0.8f);
	BuildGeometryBuffers(device);
}

void Puck::SetPosition(const Vector3 pos)
{
	m_position3D.x = pos.x;
	m_position3D.y = pos.y;
	m_position3D.z = pos.z;
}

void Puck::SetVelocity(const Vector3 vel)
{
	m_velocity.x = vel.x;
	m_velocity.y = vel.y;
	m_velocity.z = vel.z;
}

void Puck::Update(float deltaTime)
{
	m_position3D.x += m_velocity.x * deltaTime;
	m_position3D.z += m_velocity.z * deltaTime;

	if(m_position3D.x > 7.4f)
	{
		m_position3D.x = 7.4f;
		m_velocity = m_velocity.reflect(Vector3(-1, 0, 0), 0.9f);
	}
	if(m_position3D.x < -7.4f)
	{
		m_position3D.x = -7.4f;
		m_velocity.reflect(Vector3(1, 0, 0), 0.9f);
	}
	if(m_position3D.z > 17.4f)
	{
		m_position3D.z = 17.4f;
		m_velocity.reflect(Vector3(0, 0, -1), 0.9f);
	}
	if(m_position3D.z < -17.4f)
	{
		m_position3D.z = -17.4f;
		m_velocity.reflect(Vector3(0, 0, 1), 0.9f);
	}

	m_bounds.SetCenterPosition(m_position3D.x, m_position3D.z);
}

void Puck::Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
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
	XMMATRIX scale = XMMatrixScaling(0.8f, 0.5f, 0.8f);
	XMStoreFloat4x4(&m_puckWorldMatrix, XMMatrixMultiply(scale, translation));

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

		XMMATRIX world = XMLoadFloat4x4(&m_puckWorldMatrix);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(m_puckMaterial);
		Effects::BasicFX->SetDiffuseMap(m_puckTexSRV);
		
		activeTexTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(m_puckIndexCount, m_puckIndexOffset, m_puckVertexOffset);

		dc->OMSetBlendState(0, blendFactor, 0xffffffff);
    }
}

void Puck::BuildGeometryBuffers(ID3D11Device* device)
{
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateCylinder(1.0f, 1.0f, 1.0f, 40, 40, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	m_puckVertexOffset = 0;

	// Cache the index count of each object.
	m_puckIndexCount = cylinder.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	m_puckIndexOffset = 0;
	
	UINT totalVertexCount = 
		cylinder.Vertices.size();

	UINT totalIndexCount = 
		m_puckIndexCount;

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