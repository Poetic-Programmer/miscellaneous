//***************************************************************************************
// 'The Gunslinger Followed' by Michael Dunleavy
//
// Demonstrates a noise based terrain with a basic post processing effect.
//
// Controls:
//			'w', 's', 'd', 'a' to move	
//          'r' to change the rasterizer state in "Raom" mode
//          'm' to change randering mode
//***************************************************************************************

#include "D3DApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "RenderStates.h"
#include "Vertex.h"
#include "Camera.h"
#include "Sky.h"
#include "ParticleSystem.h"
#include "Hut.h"
#include "MohaineDesert.h"
#include "BlurFilter.h"

enum Mode
{
	Full,
	Roam,
};

class ShapesApp : public D3DApp
{
public:
	ShapesApp(HINSTANCE hInstance);
	~ShapesApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
	void DrawScreenQuad();

	void BuildScreenQuadGeometryBuffers();
	void BuildOffscreenViews();
	void DrawWrapper();
private:
	void RenderToBackbuffer();
	void RenderToTexture();

private:
	Mode             m_mode;
	Sky*             m_sky;
	Hut              m_hut;
	MohaineDesert    m_desert;
	DirectionalLight m_directionLight[3];
	Camera           m_fpsCamera;
	POINT            m_prevMousePos;

	ID3D11RasterizerState* m_currentRS;

	// Particle Info
	ParticleSystem            m_fire;
	ID3D11ShaderResourceView* m_flareTexSRV;
	ID3D11ShaderResourceView* m_randomTexSRV;

	// Deferred Rendering Info
	BlurFilter mBlur;
	ID3D11ShaderResourceView*  m_offScreenSRV;
	ID3D11UnorderedAccessView* m_offScreenUAV;
	ID3D11RenderTargetView*    m_offScreenRTV;	
	ID3D11Buffer*              m_screenQuadVB;
	ID3D11Buffer*              m_screenQuadIB;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	ShapesApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}
 

ShapesApp::ShapesApp(HINSTANCE hInstance)
: D3DApp(hInstance), 
  m_sky(0), m_randomTexSRV(0), m_flareTexSRV(0), m_screenQuadVB(0), m_screenQuadIB(0),m_offScreenSRV(0), m_offScreenUAV(0), m_offScreenRTV(0)
{
	m_mainWindowCaption = L"The Gunslinger Followed";
	
	m_prevMousePos.x = 0;
	m_prevMousePos.y = 0;

	m_fpsCamera.SetPosition(0.0f, 2.0f, -35.0f);

	m_directionLight[0].Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_directionLight[0].Diffuse  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_directionLight[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_directionLight[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	m_directionLight[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_directionLight[1].Diffuse  = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	m_directionLight[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_directionLight[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	m_directionLight[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_directionLight[2].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_directionLight[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_directionLight[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	m_currentRS = RenderStates::WireframeRS;
	m_mode = Roam;
}

ShapesApp::~ShapesApp()
{
	ReleaseCOM(m_flareTexSRV);
	ReleaseCOM(m_randomTexSRV);

	ReleaseCOM(m_screenQuadVB);
	ReleaseCOM(m_screenQuadIB);
 
	ReleaseCOM(m_offScreenSRV);
	ReleaseCOM(m_offScreenUAV);
	ReleaseCOM(m_offScreenRTV);

	SafeDelete(m_sky);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool ShapesApp::Init()
{
	if(!D3DApp::Initialize())
		return false;

	Effects::InitializeAll(m_d3dDevice);
	InputLayouts::InitializeAll(m_d3dDevice);
	RenderStates::InitializeAll(m_d3dDevice);

	m_sky = new Sky(m_d3dDevice, L"Textures/desertcube1024.dds", 5000.0f);

	m_desert.Initialize(m_d3dDevice, m_d3dImmediateContext);
	m_hut.Initialize(m_d3dDevice);
	
	m_randomTexSRV = d3dHelper::CreateRandomTexture1DSRV(m_d3dDevice);
	std::vector<std::wstring> flares;
	flares.push_back(L"Textures\\flare0.dds");
	m_flareTexSRV = d3dHelper::CreateTexture2DArraySRV(m_d3dDevice, m_d3dImmediateContext, flares);

	m_fire.Initialize(m_d3dDevice, Effects::FireFX, m_flareTexSRV, m_randomTexSRV, 500); 
	m_fire.SetEmitPos(XMFLOAT3(0.0f, 1.0f, 120.0f));

	BuildScreenQuadGeometryBuffers();
	BuildOffscreenViews();

	return true;
}

void ShapesApp::OnResize()
{
	D3DApp::OnResize();

	m_fpsCamera.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);

	BuildScreenQuadGeometryBuffers();
	BuildOffscreenViews();

	mBlur.Init(m_d3dDevice, m_clientWidth, m_clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
}

void ShapesApp::UpdateScene(float dt)
{
	m_desert.Update(dt);

	// Camera Movement
	if( GetAsyncKeyState('W') & 0x8000 )
		m_fpsCamera.Walk(200.0f*dt);
	
	if( GetAsyncKeyState('S') & 0x8000 )
		m_fpsCamera.Walk(-100.0f*dt);

	if( GetAsyncKeyState('A') & 0x8000 )
		m_fpsCamera.Strafe(-100.0f*dt);

	if( GetAsyncKeyState('D') & 0x8000 )
		m_fpsCamera.Strafe(100.0f*dt);

	// Simple "out of bounds" tests
	if(m_fpsCamera.GetPosition().z >= 500)
		m_fpsCamera.SetPosition(m_fpsCamera.GetPosition().x, m_fpsCamera.GetPosition().y, 500);
	if(m_fpsCamera.GetPosition().z <= -500)
		m_fpsCamera.SetPosition(m_fpsCamera.GetPosition().x, m_fpsCamera.GetPosition().y, -500);
	if(m_fpsCamera.GetPosition().x >= 500)
		m_fpsCamera.SetPosition(500, m_fpsCamera.GetPosition().y, m_fpsCamera.GetPosition().z);
	if(m_fpsCamera.GetPosition().x <= -500)
		m_fpsCamera.SetPosition(-500, m_fpsCamera.GetPosition().y, m_fpsCamera.GetPosition().z);

	// Change the rendering mode
	if( GetAsyncKeyState('M') & 0x8000 )
	{
		if(m_mode == Full)
			m_mode = Roam;
		else
			m_mode = Full;

		Sleep(1000);
	}

	//Change the rasterizer sate
	if(GetAsyncKeyState('R') & 0x8000)
	{
		if(m_mode == Roam)
		{
			if(m_currentRS == RenderStates::WireframeRS)
				m_currentRS = RenderStates::NoCullRS;
			else
				m_currentRS = RenderStates::WireframeRS;

			Sleep(1000);
		}
	}

	m_fire.Update(dt, m_timer.TotalTime());

	// Get the camera Y position from the terrain.
	XMFLOAT3 camPos = m_fpsCamera.GetPosition();		
	float y = m_desert.GetHeight(camPos.x, camPos.z);
	m_fpsCamera.SetPosition(camPos.x, y + 4.0f, camPos.z);

	// Update the cameras matrices.
	m_fpsCamera.UpdateViewMatrix();
}

void ShapesApp::DrawScene()
{
	if(m_mode == Full)
		RenderToTexture();
	else
		RenderToBackbuffer();
}

void ShapesApp::RenderToBackbuffer()
{
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	m_d3dImmediateContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colours::Black));
	m_d3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dImmediateContext->RSSetState(m_currentRS);
	
	// Render objects.
	m_hut.Render(m_d3dImmediateContext, m_fpsCamera, m_directionLight);
	m_desert.Render(m_d3dImmediateContext, m_fpsCamera, m_directionLight);
	
	m_d3dImmediateContext->RSSetState(0);

	//Sky and particle
	m_sky->Render(m_d3dImmediateContext, m_fpsCamera);
	m_fire.Render(m_d3dImmediateContext, m_fpsCamera);

	// restore default states, as the SkyFX changes them in the effect file.
	m_d3dImmediateContext->RSSetState(0);
	m_d3dImmediateContext->OMSetDepthStencilState(0, 0);
	m_d3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff); 

	HR(m_swapChain->Present(0, 0));
}

void ShapesApp::RenderToTexture()
{
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

	ID3D11RenderTargetView* renderTargets[1] = {m_offScreenRTV};
	m_d3dImmediateContext->OMSetRenderTargets(1, renderTargets, m_depthStencilView);

	m_d3dImmediateContext->ClearRenderTargetView(m_offScreenRTV, reinterpret_cast<const float*>(&Colours::Sand));
	m_d3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Render objects
	m_desert.Render(m_d3dImmediateContext, m_fpsCamera, m_directionLight);
	m_hut.Render(m_d3dImmediateContext, m_fpsCamera, m_directionLight);
	//m_fire.Render(m_d3dImmediateContext, m_fpsCamera);

	// Restore states for post processing.
	//m_d3dImmediateContext->RSSetState(0);
	//m_d3dImmediateContext->OMSetDepthStencilState(0, 0);
	//m_d3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff); 

	renderTargets[0] = m_renderTargetView;
	m_d3dImmediateContext->OMSetRenderTargets(1, renderTargets, m_depthStencilView);

	//mBlur.SetGaussianWeights(0.0f);
	mBlur.BlurInPlace(m_d3dImmediateContext, m_offScreenSRV, m_offScreenUAV, 4, m_timer.DeltaTime());

	m_d3dImmediateContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colours::Sand));
	m_d3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	DrawScreenQuad();

	HR(m_swapChain->Present(0, 0));
}

void ShapesApp::DrawScreenQuad()
{
	m_d3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
    m_d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 
	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;
 
	XMMATRIX identity = XMMatrixIdentity();
 
	ID3DX11EffectTechnique* texOnlyTech = Effects::BasicFX->Light0TexTech;
	D3DX11_TECHNIQUE_DESC techDesc;

	texOnlyTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		m_d3dImmediateContext->IASetVertexBuffers(0, 1, &m_screenQuadVB, &stride, &offset);
		m_d3dImmediateContext->IASetIndexBuffer(m_screenQuadIB, DXGI_FORMAT_R32_UINT, 0);

		Effects::BasicFX->SetWorld(identity);
		Effects::BasicFX->SetWorldInvTranspose(identity);
		Effects::BasicFX->SetWorldViewProj(identity);
		Effects::BasicFX->SetTexTransform(identity);
		Effects::BasicFX->SetDiffuseMap(mBlur.GetBlurredOutput());

		texOnlyTech->GetPassByIndex(p)->Apply(0, m_d3dImmediateContext);
		m_d3dImmediateContext->DrawIndexed(6, 0, 0);
    }
}

void ShapesApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_prevMousePos.x = x;
	m_prevMousePos.y = y;

	SetCapture(m_hMainWindow);
}

void ShapesApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void ShapesApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_prevMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_prevMousePos.y));

		// Update angles based on input to orbit camera around box.
		m_fpsCamera.Pitch(dy);
		m_fpsCamera.RotateY(dx);
	}

	m_prevMousePos.x = x;
	m_prevMousePos.y = y;
}

void ShapesApp::BuildScreenQuadGeometryBuffers()
{
	GeometryGenerator::MeshData quad;

	GeometryGenerator geoGen;
	geoGen.CreateFullScreenQuad(quad);

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(quad.Vertices.size());

	for(UINT i = 0; i < quad.Vertices.size(); ++i)
	{
		vertices[i].Pos    = quad.Vertices[i].Position;
		vertices[i].Normal = quad.Vertices[i].Normal;
		vertices[i].Tex    = quad.Vertices[i].TexC;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::Basic32) * quad.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(m_d3dDevice->CreateBuffer(&vbd, &vinitData, &m_screenQuadVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * quad.Indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &quad.Indices[0];
    HR(m_d3dDevice->CreateBuffer(&ibd, &iinitData, &m_screenQuadIB));
}

void ShapesApp::BuildOffscreenViews()
{
	// We call this function everytime the window is resized so that the render target is a quarter
	// the client area dimensions.  So Release the previous views before we create new ones.
	ReleaseCOM(m_offScreenSRV);
	ReleaseCOM(m_offScreenRTV);
	ReleaseCOM(m_offScreenUAV);

	D3D11_TEXTURE2D_DESC texDesc;
	
	texDesc.Width     = m_clientWidth;
	texDesc.Height    = m_clientHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format    = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count   = 1;  
	texDesc.SampleDesc.Quality = 0;  
	texDesc.Usage          = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags      = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags = 0; 
	texDesc.MiscFlags      = 0;

	ID3D11Texture2D* offscreenTex = 0;
	HR(m_d3dDevice->CreateTexture2D(&texDesc, 0, &offscreenTex));

	// Null description means to create a view to all mipmap levels using 
	// the format the texture was created with.
	HR(m_d3dDevice->CreateShaderResourceView(offscreenTex, 0, &m_offScreenSRV));
	HR(m_d3dDevice->CreateRenderTargetView(offscreenTex, 0, &m_offScreenRTV));
	HR(m_d3dDevice->CreateUnorderedAccessView(offscreenTex, 0, &m_offScreenUAV));

	// View saves a reference to the texture so we can release our reference.
	ReleaseCOM(offscreenTex);
}