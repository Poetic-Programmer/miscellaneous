#include "AirHockeyTable.h"
#include "Camera.h"
#include "LightHelper.h"
#include "RenderStates.h"

AirHockeyTable::AirHockeyTable()
	: 
	m_vb(0),
	m_ib(0),

	m_legTexSRV(0),
	m_cusionTexSRV(0),
	m_baseTexSRV(0)
{}

AirHockeyTable::~AirHockeyTable()
{
	ReleaseCOM(m_vb);
	ReleaseCOM(m_ib);

	ReleaseCOM(m_baseTexSRV);
	ReleaseCOM(m_cusionTexSRV);
	ReleaseCOM(m_legTexSRV);
}

void AirHockeyTable::Initialize(ID3D11Device* device)
{	
	HR(D3DX11CreateShaderResourceViewFromFile(device, 
		L"Textures/base.dds", 0, 0, &m_baseTexSRV, 0 ));

	HR(D3DX11CreateShaderResourceViewFromFile(device, 
		L"Textures/bricks.dds", 0, 0, &m_cusionTexSRV, 0 ));

	HR(D3DX11CreateShaderResourceViewFromFile(device, 
		L"Textures/floor.dds", 0, 0, &m_legTexSRV, 0 ));

	XMMATRIX I = XMMatrixIdentity();

	XMMATRIX baseScale = XMMatrixScaling(20.0f, 2.0f, 40.0f);
	XMMATRIX baseOffset = XMMatrixTranslation(0.0f, 8.0f, 0.0f);
	XMStoreFloat4x4(&m_baseWorldMatrix, XMMatrixMultiply(baseScale, baseOffset));

	XMStoreFloat4x4(&m_cusionWorldMatrix[0], 
		XMMatrixMultiply(XMMatrixScaling(2, 40, 2), 
		XMMatrixMultiply(XMMatrixRotationX(90 * MathHelper::DegreesToRadians), XMMatrixTranslation(10.0f, 8.5f, 0.0f))));   // Left

	XMStoreFloat4x4(&m_cusionWorldMatrix[1],
		XMMatrixMultiply(XMMatrixScaling(2, 40, 2), 
		XMMatrixMultiply(XMMatrixRotationX(90 * MathHelper::DegreesToRadians), XMMatrixTranslation(-10.0f, 8.5f, 0.0f))));	 //Right

	XMStoreFloat4x4(&m_cusionWorldMatrix[2], 
		XMMatrixMultiply(XMMatrixScaling(2, 20, 2),
		XMMatrixMultiply(XMMatrixRotationZ(90 * MathHelper::DegreesToRadians), XMMatrixTranslation( 0.0f, 8.5f, 20.0f))));  // Far

	XMStoreFloat4x4(&m_cusionWorldMatrix[3], 
		XMMatrixMultiply(XMMatrixScaling(2, 20, 2), 
		XMMatrixMultiply(XMMatrixRotationZ(90 * MathHelper::DegreesToRadians), XMMatrixTranslation( 0.0f, 8.5f, -20.0f)))); // Near	

	XMStoreFloat4x4(&m_legWorldMatrix[0], XMMatrixTranslation(7.0f, 2.0f, 15.0f));
	XMStoreFloat4x4(&m_legWorldMatrix[1], XMMatrixTranslation(7.0f, 2.0f, -15.0f));	
	XMStoreFloat4x4(&m_legWorldMatrix[2], XMMatrixTranslation(-7.0f, 2.0f, 15.0f));
	XMStoreFloat4x4(&m_legWorldMatrix[3], XMMatrixTranslation(-7.0f, 2.0f, -15.0f));
			
	m_legMaterial.Ambient  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_legMaterial.Diffuse  = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	m_legMaterial.Specular = XMFLOAT4(1.0f, 0.0f, 0.0f, 16.0f);

	m_cusionMaterial.Ambient  = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_cusionMaterial.Diffuse  = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	m_cusionMaterial.Specular = XMFLOAT4(0.0f, 0.0f, 1.0f, 16.0f);

	m_baseMaterial.Ambient  = XMFLOAT4(0.4f, 0.6f, 1.0f, 1.0f);
	m_baseMaterial.Diffuse  = XMFLOAT4(0.5f, 0.7f, 1.0f, 1.0f);
	m_baseMaterial.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f);

	BuildGeometryBuffers(device);
}

void AirHockeyTable::Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
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
		XMMATRIX world = XMLoadFloat4x4(&m_baseWorldMatrix);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(m_baseMaterial);
		Effects::BasicFX->SetDiffuseMap(m_baseTexSRV);

		
		activeTexTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(m_baseIndexCount, m_baseIndexOffset, m_baseVertexOffset);

		// Draw the cusions.
		for(int i = 0; i < 4; ++i)
		{
			world = XMLoadFloat4x4(&m_cusionWorldMatrix[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetMaterial(m_cusionMaterial);
			Effects::BasicFX->SetDiffuseMap(m_cusionTexSRV);

			activeTexTech->GetPassByIndex(p)->Apply(0, dc);
			dc->DrawIndexed(m_cusionIndexCount, m_cusionIndexOffset, m_cusionVertexOffset);
		}

		// Draw the legs.
		for(int i = 0; i < 4; ++i)
		{
			world = XMLoadFloat4x4(&m_legWorldMatrix[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetMaterial(m_legMaterial);
			Effects::BasicFX->SetDiffuseMap(m_legTexSRV);

			activeTexTech->GetPassByIndex(p)->Apply(0, dc);
			dc->DrawIndexed(m_legIndexCount, m_legIndexOffset, m_legVertexOffset);
		}



		//dc->RSSetState(0);
		dc->OMSetBlendState(0, blendFactor, 0xffffffff);
    }
}

void AirHockeyTable::BuildGeometryBuffers(ID3D11Device* device)
{
	GeometryGenerator::MeshData baseBox;
	GeometryGenerator::MeshData cusionCylinder;
	GeometryGenerator::MeshData legCylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, baseBox);
	geoGen.CreateCylinder(1.0f, 1.0f, 1.0f, 40, 40, cusionCylinder);
	geoGen.CreateCylinder(1.5f, 2.0f, 10.0f, 40, 40, legCylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	m_baseVertexOffset      = 0;
	m_cusionVertexOffset    = baseBox.Vertices.size();
	m_legVertexOffset       = m_cusionVertexOffset + cusionCylinder.Vertices.size();

	// Cache the index count of each object.
	m_baseIndexCount    = baseBox.Indices.size();
	m_cusionIndexCount  = cusionCylinder.Indices.size();
	m_legIndexCount     = legCylinder.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	m_baseIndexOffset     = 0;
	m_cusionIndexOffset   = m_baseIndexCount;
	m_legIndexOffset      = m_cusionIndexOffset + m_cusionIndexCount;
	
	UINT totalVertexCount = 
		baseBox.Vertices.size() + 
		cusionCylinder.Vertices.size() +
		legCylinder.Vertices.size();

	UINT totalIndexCount = 
		m_baseIndexCount + 
		m_cusionIndexCount +
		m_legIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);

	UINT k = 0;
	for(size_t i = 0; i < baseBox.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = baseBox.Vertices[i].Position;
		vertices[k].Normal = baseBox.Vertices[i].Normal;
		vertices[k].Tex    = baseBox.Vertices[i].TexC;
	}

	for(size_t i = 0; i < cusionCylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = cusionCylinder.Vertices[i].Position;
		vertices[k].Normal = cusionCylinder.Vertices[i].Normal;
		vertices[k].Tex    = cusionCylinder.Vertices[i].TexC;
	}

	for(size_t i = 0; i < legCylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = legCylinder.Vertices[i].Position;
		vertices[k].Normal = legCylinder.Vertices[i].Normal;
		vertices[k].Tex    = legCylinder.Vertices[i].TexC;
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
	indices.insert(indices.end(), baseBox.Indices.begin(), baseBox.Indices.end());
	indices.insert(indices.end(), cusionCylinder.Indices.begin(), cusionCylinder.Indices.end());
	indices.insert(indices.end(), legCylinder.Indices.begin(), legCylinder.Indices.end());

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