//=============================================================================
// Create.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Shows how to texture a box to make it look like a crate.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//              'Z'/'X' - Zoom
//
//=============================================================================

#include "d3dApp.h"
#include "Light.h"
#include "Lighthouse.h"
#include "Camera.h"
#include "InputLayouts.h"
#include "Effects.h"
#include "States.h"
#include "Waves.h"
#include "Tech.h"
#include "Sky.h"
#include "AncientRuin.h"

class MainApp : public D3DApp
{
public:
	MainApp(HINSTANCE hInstance);
	~MainApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

	LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);
private:

	POINT mOldMousePos;
	int mLightType;

	Lighthouse mLighthouse;
	AncientRuin mRuin;
	Waves mWaves;
	Light mLights[3];
	Sky   mSky;

	ID3D10RasterizerState* mCurrentRS;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	MainApp theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

MainApp::MainApp(HINSTANCE hInstance)
: D3DApp(hInstance), mLightType(0)
{
}

MainApp::~MainApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	fx::DestroyAll();
	InputLayout::DestroyAll();
	states::DestroyAll();

	ReleaseCOM(mCurrentRS);
}

void MainApp::initApp()
{
	D3DApp::initApp();

	mClearColor = D3DXCOLOR(0.2f, 0.2f, 0.2f, 0.2f);
	GetCamera().position() = D3DXVECTOR3(0.0f, 5.0f, 0.0f);

	fx::InitAll(md3dDevice);
	InputLayout::InitAll(md3dDevice);
	states::InitAll(md3dDevice);
	TextureAlpha::InitializeVariables();
	CubeMap::InitialIzeVariables();
	SkyMap::InitializeVariables();
	Lighting::InitializeVariables();
	GetTextureMgr().init(md3dDevice);

	mLighthouse.Init(md3dDevice);
	mRuin.Initialize(md3dDevice);
	mWaves.init(md3dDevice, 257, 257, 0.5f, 0.03f, 3.25f, 0.0f);
	mSky.init(md3dDevice, 5000.0f);

	// Generate some waves at start up.
	for(int k = 0; k < 50; ++k)
	{ 
		DWORD i = 5 + rand() % 250;
		DWORD j = 5 + rand() % 250;

		float r = RandF(0.5f, 1.25f);

		mWaves.disturb(i, j, r);
	}

	mLightType = 0;
 
	// Parallel light.
	mLights[0].dir      = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
	mLights[0].ambient  = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	mLights[0].diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[0].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
 
	// Pointlight--position is changed every frame to animate.
	mLights[1].ambient  = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	mLights[1].diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[1].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[1].att.x    = 0.0f;
	mLights[1].att.y    = 0.1f;
	mLights[1].att.z    = 0.0f;
	mLights[1].range    = 50.0f;

	// Spotlight--position and direction changed every frame to animate.
	mLights[2].ambient  = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	mLights[2].diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[2].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[2].att.x    = 1.0f;
	mLights[2].att.y    = 0.0f;
	mLights[2].att.z    = 0.0f;
	mLights[2].spotPow  = 64.0f;
	mLights[2].range    = 10000.0f;

	mCurrentRS = states::WireframeRS;
}

void MainApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	GetCamera().setLens(0.25f*PI, aspect, 0.5f, 1000.0f);
}

void MainApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);

	if(GetAsyncKeyState('A') & 0x8000)	GetCamera().strafe(-10.0f*dt);
	if(GetAsyncKeyState('D') & 0x8000)	GetCamera().strafe(+10.0f*dt);
	if(GetAsyncKeyState('W') & 0x8000)	GetCamera().walk(+10.0f*dt);
	if(GetAsyncKeyState('S') & 0x8000)	GetCamera().walk(-10.0f*dt);
 	if(GetAsyncKeyState('R') & 0x8000)
	{
		if(mCurrentRS == states::WireframeRS)
			mCurrentRS = states::SolidframeRS;
		else
			mCurrentRS = states::WireframeRS;
		Sleep(1000);
	}

	if(GetAsyncKeyState('1') & 0x8000)	mLightType = 0;
	if(GetAsyncKeyState('2') & 0x8000)	mLightType = 1;
	if(GetAsyncKeyState('3') & 0x8000)	mLightType = 2;

	GetCamera().rebuildView();

	// The point light circles the scene as a function of time, 
	// staying 7 units above the land's or water's surface.
	mLights[1].pos.x = 50.0f*cosf( mTimer.getGameTime() );
	mLights[1].pos.z = 50.0f*sinf( mTimer.getGameTime() );
	mLights[1].pos.y = 5;


	// The spotlight takes on the camera position and is aimed in the
	// same direction the camera is looking.  In this way, it looks
	// like we are holding a flashlight.
	mLights[2].pos = GetCamera().position();
	D3DXVec3Normalize(&mLights[2].dir, &(GetCamera().look() - GetCamera().position()));

	mWaves.update(dt);
}

void MainApp::drawScene()
{
	D3DApp::drawScene();

	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	md3dDevice->OMSetBlendState(states::TransparentBS, blendFactor, 0xffffffff);
    md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	
	md3dDevice->IASetInputLayout(InputLayout::PosNormalTex);

	// Set per frame constants.
	Lighting::fxEyePosVar->SetRawValue(&GetCamera().position(), 0, sizeof(D3DXVECTOR3));
	Lighting::fxLightVar->SetRawValue(&mLights[mLightType], 0, sizeof(Light));
	Lighting::fxLightType->SetInt(mLightType);

	// set constants
	TextureAlpha::fxEyePosVar->SetRawValue(&GetCamera().position(), 0, sizeof(D3DXVECTOR3));
	TextureAlpha::fxLightVar->SetRawValue(&mLights[mLightType], 0, sizeof(Light));

	
	mSky.draw();

	md3dDevice->RSSetState(mCurrentRS);

	mWaves.draw();
	mRuin.Draw();
	mLighthouse.Draw();

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};

	md3dDevice->RSSetState(states::SolidframeRS);

	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);

	mSwapChain->Present(0, 0);
}

 LRESULT MainApp::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	POINT mousePos;
	int dx = 0;
	int dy = 0;
	switch(msg)
	{
	case WM_LBUTTONDOWN:
		if( wParam & MK_LBUTTON )
		{
			SetCapture(mhMainWnd);

			mOldMousePos.x = LOWORD(lParam);
			mOldMousePos.y = HIWORD(lParam);
		}
		return 0;

	case WM_LBUTTONUP:
		ReleaseCapture();
		return 0;

	case WM_MOUSEMOVE:
		if( wParam & MK_LBUTTON )
		{
			mousePos.x = (int)LOWORD(lParam); 
			mousePos.y = (int)HIWORD(lParam); 

			dx = mousePos.x - mOldMousePos.x;
			dy = mousePos.y - mOldMousePos.y;

			GetCamera().pitch( dy * 0.0087266f );
			GetCamera().rotateY( dx * 0.0087266f );
			
			mOldMousePos = mousePos;
		}
		return 0;
	}

	return D3DApp::msgProc(msg, wParam, lParam);
}