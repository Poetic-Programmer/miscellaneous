//***************************************************************************************
// 'Air Hockey' by Michael Dunleavy
//
// Demonstrates a succesful connection from server to client.
//
// Controls:
//			'w', 's', 'd', 'a' to move around the scene	
//          'Mouse' to rotate camera
//          'r' to change the rasterizer state in "Raom" mode
//          'c' to set up connection, "ENTER" to connect
//***************************************************************************************
#include "Network.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>


#include "AirHockeyTable.h"
#include "D3DApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "RenderStates.h"
#include "Vertex.h"
#include "Camera.h"
#include "Game.h"

using namespace std;
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

	void RecievePacket();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	float m_timeBetweenPackets;
private:
	void RenderToBackbuffer();

private:
	bool    m_connected;
	Vector3 m_velocity;
	Vector3 m_prevPlayerPosition;
	Vector3 m_currPlayerPosition;

	Network* g_network;

	Game             m_game;
	DirectionalLight m_directionLight[3];
	Camera           m_fpsCamera;
	POINT            m_prevMousePos;

	ID3D11RasterizerState* m_currentRS;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	 
	AllocConsole();

    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;

	std::cout << "Yo" << endl;
	
	ShapesApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;

	return theApp.Run();
}
 

ShapesApp::ShapesApp(HINSTANCE hInstance)
: D3DApp(hInstance)
{
	m_mainWindowCaption = L"Air Hockey Client";
	
	g_network = new Network();

	m_prevMousePos.x = 0;
	m_prevMousePos.y = 0;

	m_fpsCamera.SetPosition(-20.0f, 15.0f, 0.0f);
	m_fpsCamera.LookAt(XMFLOAT3(-20.0f, 40.0f, 0.0f), XMFLOAT3(0.0f, 5.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_directionLight[0].Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_directionLight[0].Diffuse  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_directionLight[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_directionLight[0].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	m_directionLight[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_directionLight[1].Diffuse  = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	m_directionLight[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_directionLight[1].Direction = XMFLOAT3(-0.0f, -0.707f, -0.707f);

	m_directionLight[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_directionLight[2].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_directionLight[2].Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_directionLight[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	m_currentRS = RenderStates::WireframeRS;
	m_connected = false;
	m_timeBetweenPackets = 0;
}

ShapesApp::~ShapesApp()
{
	SafeDelete(g_network);
	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool ShapesApp::Init()
{
	if(!D3DApp::Initialize())
		return false;

	g_network->Initialize();

	Effects::InitializeAll(m_d3dDevice);
	InputLayouts::InitializeAll(m_d3dDevice);
	RenderStates::InitializeAll(m_d3dDevice);

	m_game.Initialize(m_d3dDevice);

	return true;
}

void ShapesApp::OnResize()
{
	D3DApp::OnResize();

	m_fpsCamera.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void ShapesApp::UpdateScene(float dt)
{
	m_game.Update(dt);

	//=========================================================================================================
	// Network
	//=========================================================================================================
	// Check for connection
	if( GetAsyncKeyState('C') & 0x8000 )
	{
		if(!g_network->IsConnected())
		{
			g_network->SetUpClientConnection();
		}
	}

	// If connection exists, update game accordingly
	if(g_network->IsConnected())
	{
		if(g_network->RecievePacket())
		{
			m_currPlayerPosition = g_network->GetPacket().PlayerPosition; // end interpolation point

			m_game.SetPlayerInfo(m_prevPlayerPosition, m_currPlayerPosition, m_timeBetweenPackets); 
			m_game.SetPuckInfo(g_network->GetPacket().Puckposition, g_network->GetPacket().PuckVelocity);

			m_timeBetweenPackets = 0;                    // reset interpolation time
			m_prevPlayerPosition = m_currPlayerPosition; // start interpolation point
		}
		else
		{
			m_timeBetweenPackets += dt;
		}
	}
	//==========================================================================================================


	//==========================================================================================================
	// CAMERA 
	//==========================================================================================================
	// Camera Movement
	if( GetAsyncKeyState('W') & 0x8000 )
		m_fpsCamera.Walk(200.0f*dt);
	
	if( GetAsyncKeyState('S') & 0x8000 )
		m_fpsCamera.Walk(-100.0f*dt);

	if( GetAsyncKeyState('A') & 0x8000 )
		m_fpsCamera.Strafe(-100.0f*dt);

	if( GetAsyncKeyState('D') & 0x8000 )
		m_fpsCamera.Strafe(100.0f*dt);

	if(m_fpsCamera.GetPosition().z >= 500)
		m_fpsCamera.SetPosition(m_fpsCamera.GetPosition().x, m_fpsCamera.GetPosition().y, 500);
	if(m_fpsCamera.GetPosition().z <= -500)
		m_fpsCamera.SetPosition(m_fpsCamera.GetPosition().x, m_fpsCamera.GetPosition().y, -500);
	if(m_fpsCamera.GetPosition().x >= 500)
		m_fpsCamera.SetPosition(500, m_fpsCamera.GetPosition().y, m_fpsCamera.GetPosition().z);
	if(m_fpsCamera.GetPosition().x <= -500)
		m_fpsCamera.SetPosition(-500, m_fpsCamera.GetPosition().y, m_fpsCamera.GetPosition().z);

	m_fpsCamera.UpdateViewMatrix();
	//========================================================================================================

	//Change the rasterizer sate
	if(GetAsyncKeyState('R') & 0x8000)
	{
		if(m_currentRS == RenderStates::WireframeRS)
			m_currentRS = RenderStates::NoCullRS;
		else
			m_currentRS = RenderStates::WireframeRS;

		Sleep(1000);
	}

	// Update the cameras matrices.
	
}

void ShapesApp::DrawScene()
{
	RenderToBackbuffer();
}

void ShapesApp::RenderToBackbuffer()
{
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	m_d3dImmediateContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colours::Black));
	m_d3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dImmediateContext->RSSetState(m_currentRS);
	
	// Render objects.
	m_game.Render(m_d3dImmediateContext, m_fpsCamera, m_directionLight);
	m_d3dImmediateContext->RSSetState(0);

	// restore default states, as the SkyFX changes them in the effect file.
	m_d3dImmediateContext->RSSetState(0);
	m_d3dImmediateContext->OMSetDepthStencilState(0, 0);
	m_d3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff); 

	HR(m_swapChain->Present(0, 0));
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