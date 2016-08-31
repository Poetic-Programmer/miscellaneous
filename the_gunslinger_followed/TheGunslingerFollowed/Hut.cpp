#include "Hut.h"
#include "Camera.h"
#include "LightHelper.h"
#include "RenderStates.h"
Hut::Hut()
	: 
    m_vb(0), 
	m_ib(0),
	m_stoneTexSRV(0),
	m_brickTexSRV(0)
{}

Hut::~Hut()
{
	ReleaseCOM(m_vb);
	ReleaseCOM(m_ib);
	ReleaseCOM(m_stoneTexSRV);
	ReleaseCOM(m_brickTexSRV);
}

void Hut::Initialize(ID3D11Device* device)
{
	HR(D3DX11CreateShaderResourceViewFromFile(device, 
		L"Textures/stone.dds", 0, 0, &m_stoneTexSRV, 0 ));

	HR(D3DX11CreateShaderResourceViewFromFile(device, 
		L"Textures/bricks.dds", 0, 0, &m_brickTexSRV, 0 ));

	XMMATRIX I = XMMatrixIdentity();

	XMMATRIX boxScale = XMMatrixScaling(20.0f, 10.0f, 20.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	XMStoreFloat4x4(&m_boxWorld, XMMatrixMultiply(boxScale, boxOffset));

	XMMATRIX ringScale = XMMatrixScaling(12.0f, 6.0f, 12.0f);
	XMMATRIX ringOffset = XMMatrixTranslation(0.0f, 2.0f, -14.5f);
	XMMATRIX ringRotation = XMMatrixRotationX(0.34906585f);
	XMStoreFloat4x4(&m_ringWorld, XMMatrixMultiply(ringRotation, XMMatrixMultiply(ringScale, ringOffset)));

	float theta = (XM_PI * 2) / 5;
	for(int i = 0; i < 5; ++i)
	{			
		XMStoreFloat4x4(&m_cylWorld[i*2+0], XMMatrixTranslation((cosf(i * theta)) *  15.0f, 1.5f, 15.0f * (sinf(i * theta))));
		XMStoreFloat4x4(&m_cylWorld[i*2+1], XMMatrixTranslation((cosf(i * theta)) * -15.0f, 1.5f, -15.0f * (sinf(i * theta))));	

		XMStoreFloat4x4(&m_sphereWorld[i*2+0], XMMatrixTranslation((cosf(i * theta)) *  15.0f, 7.5f, 15.0f * (sinf(i * theta))));
		XMStoreFloat4x4(&m_sphereWorld[i*2+1], XMMatrixTranslation((cosf(i * theta)) * -15.0f, 7.5f, -15.0f * (sinf(i * theta))));
	}

	m_cylinderMaterial.Ambient  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_cylinderMaterial.Diffuse  = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	m_cylinderMaterial.Specular = XMFLOAT4(1.0f, 0.0f, 0.0f, 16.0f);

	m_sphereMaterial.Ambient  = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_sphereMaterial.Diffuse  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_sphereMaterial.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 16.0f);

	m_ringMaterial.Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_ringMaterial.Diffuse  = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	m_ringMaterial.Specular = XMFLOAT4(1.0f, 0.0f, 0.0f, 16.0f);

	m_boxMaterial.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_boxMaterial.Diffuse  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_boxMaterial.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);

	BuildGeometryBuffers(device);
}

void Hut::Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{
	dc->IASetInputLayout(InputLayouts::Basic32);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;

	XMMATRIX view     = cam.View();
	XMMATRIX proj     = cam.Proj();
	XMMATRIX viewProj = cam.ViewProj();


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
		XMMATRIX world = XMLoadFloat4x4(&m_boxWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(m_boxMaterial);
		Effects::BasicFX->SetDiffuseMap(m_stoneTexSRV);

		
		activeTexTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(m_boxIndexCount, m_boxIndexOffset, m_boxVertexOffset);

		// Draw the Ring
		world = XMLoadFloat4x4(&m_ringWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(m_ringMaterial);
		Effects::BasicFX->SetDiffuseMap(m_stoneTexSRV);

		activeTexTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(m_ringIndexCount, m_ringIndexOffset, m_ringVertexOffset);

		// Draw the cylinders.
		for(int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&m_cylWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetMaterial(m_cylinderMaterial);
			Effects::BasicFX->SetDiffuseMap(m_brickTexSRV);

			activeTexTech->GetPassByIndex(p)->Apply(0, dc);
			dc->DrawIndexed(m_cylinderIndexCount, m_cylinderIndexOffset, m_cylinderVertexOffset);
		}

		// Draw the spheres.
		for(int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&m_sphereWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetMaterial(m_sphereMaterial);
			Effects::BasicFX->SetDiffuseMap(m_stoneTexSRV);

			activeTexTech->GetPassByIndex(p)->Apply(0, dc);
			dc->DrawIndexed(m_sphereIndexCount, m_sphereIndexOffset, m_sphereVertexOffset);
		}

		//dc->RSSetState(0);
		dc->OMSetBlendState(0, blendFactor, 0xffffffff);
    }
}

void Hut::BuildGeometryBuffers(ID3D11Device* device)
{
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData ring;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateRing(0.7f, 1.0f, 20, 20, ring);
	geoGen.CreateGeoSphere(1.0f, 2, sphere);
	geoGen.CreateCylinder(1.5f, 1.0f, 10.0f, 40, 40, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	m_boxVertexOffset      = 0;
	m_ringVertexOffset     = box.Vertices.size();
	m_sphereVertexOffset   = m_ringVertexOffset + ring.Vertices.size();
	m_cylinderVertexOffset = m_sphereVertexOffset + sphere.Vertices.size();

	// Cache the index count of each object.
	m_boxIndexCount      = box.Indices.size();
	m_ringIndexCount     = ring.Indices.size();
	m_sphereIndexCount   = sphere.Indices.size();
	m_cylinderIndexCount = cylinder.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	m_boxIndexOffset      = 0;
	m_ringIndexOffset     = m_boxIndexCount;
	m_sphereIndexOffset   = m_ringIndexOffset + m_ringIndexCount;
	m_cylinderIndexOffset = m_sphereIndexOffset + m_sphereIndexCount;
	
	UINT totalVertexCount = 
		box.Vertices.size() + 
		ring.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	UINT totalIndexCount = 
		m_boxIndexCount + 
		m_ringIndexCount +
		m_sphereIndexCount +
		m_cylinderIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);

	UINT k = 0;
	for(size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex    = box.Vertices[i].TexC;
	}

	for(size_t i = 0; i < ring.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = ring.Vertices[i].Position;
		vertices[k].Normal = ring.Vertices[i].Normal;
		vertices[k].Tex    = ring.Vertices[i].TexC;
	}

	for(size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].Tex    = sphere.Vertices[i].TexC;
	}

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
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), ring.Indices.begin(), ring.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
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