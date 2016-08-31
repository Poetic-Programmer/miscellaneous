#include "ParticleSystem.h"
#include "TextureManager.h"
#include "Vertex.h"
#include "Effects.h"
#include "Camera.h"
 
ParticleSystem::ParticleSystem()
: m_initVB(0), m_drawVB(0), m_streamOutVB(0), m_texArraySRV(0), m_randomTexSRV(0)
{
	m_firstRun = true;
	m_gameTime = 0.0f;
	m_timeStep = 0.0f;
	m_age      = 0.0f;

	m_eyePosW  = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_emitPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_emitDirW = XMFLOAT3(0.0f, 1.0f, 0.0f);
}

ParticleSystem::~ParticleSystem()
{
	ReleaseCOM(m_initVB);
	ReleaseCOM(m_drawVB);
	ReleaseCOM(m_streamOutVB);
}

float ParticleSystem::GetAge()const
{
	return m_age;
}

void ParticleSystem::SetEyePos(const XMFLOAT3& eyePosW)
{
	m_eyePosW = eyePosW;
}

void ParticleSystem::SetEmitPos(const XMFLOAT3& emitPosW)
{
	m_emitPosW = emitPosW;
}

void ParticleSystem::SetEmitDir(const XMFLOAT3& emitDirW)
{
	m_emitDirW = emitDirW;
}

void ParticleSystem::Initialize(ID3D11Device* device, ParticleEffect* fx, ID3D11ShaderResourceView* texArraySRV, 
	                      ID3D11ShaderResourceView* randomTexSRV, UINT maxParticles)
{
	m_maxParticles = maxParticles;

	m_fx = fx;

	m_texArraySRV  = texArraySRV;
	m_randomTexSRV = randomTexSRV; 

	BuildVB(device);
}

void ParticleSystem::Reset()
{
	m_firstRun = true;
	m_age      = 0.0f;
}

void ParticleSystem::Update(float dt, float gameTime)
{
	m_gameTime = gameTime;
	m_timeStep = dt;

	m_age += dt;
}

void ParticleSystem::Render(ID3D11DeviceContext* dc, const Camera& cam)
{
	XMMATRIX VP = cam.ViewProj();

	//
	// Set constants.
	//
	m_fx->SetViewProj(VP);
	m_fx->SetGameTime(m_gameTime);
	m_fx->SetTimeStep(m_timeStep);
	m_fx->SetEyePosW(m_eyePosW);
	m_fx->SetEmitPosW(m_emitPosW);
	m_fx->SetEmitDirW(m_emitDirW);
	m_fx->SetTexArray(m_texArraySRV);
	m_fx->SetRandomTex(m_randomTexSRV);

	//
	// Set IA stage.
	//
	dc->IASetInputLayout(InputLayouts::Particle);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = sizeof(Vertex::Particle);
    UINT offset = 0;

	// On the first pass, use the initialization VB.  Otherwise, use
	// the VB that contains the current particle list.
	if(m_firstRun)
		dc->IASetVertexBuffers(0, 1, &m_initVB, &stride, &offset);
	else
		dc->IASetVertexBuffers(0, 1, &m_drawVB, &stride, &offset);

	//
	// Draw the current particle list using stream-out only to update them.  
	// The updated vertices are streamed-out to the target VB. 
	//
	dc->SOSetTargets(1, &m_streamOutVB, &offset);

    D3DX11_TECHNIQUE_DESC techDesc;
	m_fx->StreamOutTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        m_fx->StreamOutTech->GetPassByIndex( p )->Apply(0, dc);
        
		if(m_firstRun)
		{
			dc->Draw(1, 0);
			m_firstRun = false;
		}
		else
		{
			dc->DrawAuto();
		}
    }

	// done streaming-out--unbind the vertex buffer
	ID3D11Buffer* bufferArray[1] = {0};
	dc->SOSetTargets(1, bufferArray, &offset);

	// ping-pong the vertex buffers
	std::swap(m_drawVB, m_streamOutVB);

	//
	// Draw the updated particle system we just streamed-out. 
	//
	dc->IASetVertexBuffers(0, 1, &m_drawVB, &stride, &offset);

	m_fx->DrawTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        m_fx->DrawTech->GetPassByIndex( p )->Apply(0, dc);
        
		dc->DrawAuto();
    }
}

void ParticleSystem::BuildVB(ID3D11Device* device)
{
	//
	// Create the buffer to kick-off the particle system.
	//

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(Vertex::Particle) * 1;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// The initial particle emitter has type 0 and age 0.  The rest
	// of the particle attributes do not apply to an emitter.
	Vertex::Particle p;
	ZeroMemory(&p, sizeof(Vertex::Particle));
	p.Age  = 0.0f;
	p.Type = 0; 
 
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &p;

	HR(device->CreateBuffer(&vbd, &vinitData, &m_initVB));
	
	//
	// Create the ping-pong buffers for stream-out and drawing.
	//
	vbd.ByteWidth = sizeof(Vertex::Particle) * m_maxParticles;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

    HR(device->CreateBuffer(&vbd, 0, &m_drawVB));
	HR(device->CreateBuffer(&vbd, 0, &m_streamOutVB));
}