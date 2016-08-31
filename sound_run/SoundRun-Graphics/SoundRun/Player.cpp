#include "Player.h"

#include "Camera.h"
#include "LightHelper.h"
#include "RenderStates.h"

Player::Player()
:	m_vertexBuffer(0),
	m_indexBuffer(0),
	m_roadTexSRV(0),
	m_frameTime(0),
	m_active(true),
	m_speed(0),
	m_boundingCircle(0, 0, 0, 0.5f)
{
	m_texOffset.x = 0;
	m_texOffset.y = 0;
}

Player::~Player()
{
	ReleaseCOM(m_vertexBuffer);
	ReleaseCOM(m_indexBuffer);
	ReleaseCOM(m_roadTexSRV);
	
	SafeDelete(m_3DAudioSound);
}


void Player::Initialize(ID3D11Device* device, SoundEngine& engine)
{
	HR(D3DX11CreateShaderResourceViewFromFile(device,
		L"Textures/stone.dds", 0, 0, &m_roadTexSRV, 0));

	XMMATRIX I = XMMatrixIdentity();

	XMMATRIX scale = XMMatrixScaling(10.0f, 200.0f, 1.0f);
	XMMATRIX offset = XMMatrixTranslation(2, 1.5f, 5);

	XMStoreFloat4x4(&m_world, XMMatrixMultiply(scale, offset));

	m_material.Ambient  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_material.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 16.0f);

	m_position.x = 0;
	m_position.y = 0.8f;
	m_position.z = 2;

	m_linearTime = 2.0f;

	InitializeGeometryBuffer(device);
	SetUpSound(engine);
}

void Player::SetUpSound(SoundEngine& engine)
{
	m_3DAudioSound = new X3DAudioSound(L"Sound Effects/footsteps-4-mono.wav");
	m_3DAudioSound->Initialize(engine);
	m_3DAudioSound->SetVolume(1);
}

void Player::MoveRight()
{
	m_previousState = m_currentState;
	m_currentState = MovingRight;

	if(m_currentState != m_previousState)
	{
		m_linearCurve.Clear();
		m_linearCurve.AddElement(m_position.x, m_position.y, m_position.z, 0);
		m_linearCurve.AddElement(1.0f, m_position.y, m_position.z, m_linearTime);
		m_linearTime = 0;
	}
}

void Player::MoveLeft()
{
	m_previousState = m_currentState;
	m_currentState = MovingLeft;

	if(m_currentState != m_previousState)
	{
		m_linearCurve.Clear();
		m_linearCurve.AddElement(m_position.x, m_position.y, m_position.z, 0);
		m_linearCurve.AddElement(-1.0f, m_position.y, m_position.z, m_linearTime);
		m_linearTime = 0;
	}
}

void Player::MoveToCentre()
{
	m_previousState = m_currentState;
	m_currentState = MovingCentre;

	if(m_currentState != m_previousState)
	{
		m_linearCurve.Clear();
		m_linearCurve.AddElement(m_position.x, m_position.y, m_position.z, 0);
		m_linearCurve.AddElement(0.0f, m_position.y, m_position.z, m_linearTime);
		m_linearTime = 0;
	}
}

void Player::SetActive(bool active)
{
	m_active = active;

	if(m_active)
	{
		if(!m_3DAudioSound->IsPlaying())
			m_3DAudioSound->PlaySound();
	}
	else
	{
		if(m_3DAudioSound->IsPlaying())
			m_3DAudioSound->StopSound();
	}
}

Circle Player::GetBoundingCircle()
{
	return m_boundingCircle;
}

void Player::Update(float deltaTime, SoundEngine& engine)
{
	if(m_isMoving)
	{
		//m_position.x += m_speed * deltaTime;
	}
	m_linearTime += deltaTime;

	if(m_linearTime >= 0.5f)
		m_linearTime = 0.5f;

	m_position.x = m_linearCurve.Evaluate(m_linearTime).x;

	//m_position.x += m_speed * deltaTime;
	//m_frameTime += deltaTime;
	if(m_active)
	{
		if(!m_3DAudioSound->IsPlaying())
			m_3DAudioSound->PlaySound();
	}
	else
	{
		if(m_3DAudioSound->IsPlaying())
			m_3DAudioSound->StopSound();
	}
	
	m_3DAudioSound->SetEmittierPosition(-m_position.x, 0, 0, engine);
	m_boundingCircle.SetPosition(m_position.x, m_position.y, m_position.z);

	XMMATRIX scale = XMMatrixScaling(0.5f, 1.6f, 0.5f);
	XMMATRIX offset = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	XMStoreFloat4x4(&m_world, XMMatrixMultiply(scale, offset));

}

void Player::MovePlayer(float speed)
{
	m_speed = speed;
	m_isMoving = true;
}

void Player::CenterPlayer()
{
	m_isMoving = false;
}

void Player::Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
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
	Effects::BasicFX->SetFrameTime(m_frameTime);
	Effects::BasicFX->SetTextureScrollSpeed(10.0f);
	Effects::BasicFX->SetFogColor(Colors::Yellow);
	Effects::BasicFX->SetFogStart(15.0f);
	Effects::BasicFX->SetFogRange(175.0f);
	
	ID3DX11EffectTechnique* activeTexTech = Effects::BasicFX->Light1TexTech;
	D3DX11_TECHNIQUE_DESC techDesc;

    activeTexTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		dc->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		dc->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		//dc->RSSetState(RenderStates::NoCullRS);

		// Draw the box.
		XMMATRIX world = XMLoadFloat4x4(&m_world);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(m_material);
		Effects::BasicFX->SetDiffuseMap(m_roadTexSRV);

		
		activeTexTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(m_indexCount, 0, 0);

		//dc->RSSetState(0);
		dc->OMSetBlendState(0, blendFactor, 0xffffffff);
    }
}

void Player::InitializeGeometryBuffer(ID3D11Device* device)
{
	GeometryGenerator::MeshData box;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);

	m_indexCount = box.Indices.size();

	std::vector<Vertex::Basic32> vertices(box.Vertices.size());

	UINT k = 0;
	for(size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex = box.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * box.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(device->CreateBuffer(&vbd, &vinitData, &m_vertexBuffer));

	// Pack the indices of all the meshes into one index buffer.
	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * box.Indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(device->CreateBuffer(&ibd, &iinitData, &m_indexBuffer));
}