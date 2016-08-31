#include "D3DApplication.h"
#include "d3dx11Effect.h"
#include "MathHelper.h"
#include "SoundEngine.h"
#include "Game.h"

enum GameScreenState
{
	MenuScreen,
	GameScreen,
};

class SoundRunApp : public D3DApplication
{
public:
	SoundRunApp(HINSTANCE hInstance);
	~SoundRunApp();

	bool Init();
	void OnResize();
	void Update(float deltaTime);
	void Render(); 

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
private:
	Game m_game;
private:
	ID3D11RasterizerState* m_currentRS;

	GameScreenState m_gameState;
	POINT mLastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int showcmd)
{
	SoundRunApp theApp(hInstance);

	if(!theApp.Init())
		return 0;

	return theApp.Run();
}

SoundRunApp::SoundRunApp(HINSTANCE hinstance)
	:D3DApplication(hinstance)
{
}

SoundRunApp::~SoundRunApp()
{

}

bool SoundRunApp::Init()
{
	if(!D3DApplication::Init())
		return false;

	m_game.Initialize();
	m_gameState = MenuScreen;
	return true;
}

void SoundRunApp::OnResize()
{
	D3DApplication::OnResize();
}

void SoundRunApp::Update(float deltaTime)
{
	switch(m_gameState)
	{
	case GameScreen:
		m_game.UpdateGame(mTimer.DeltaTime());

		if(m_game.IsGameOver())
		{
			m_gameState = MenuScreen;
		}

		if (GetAsyncKeyState (VK_RETURN) & 0x0001) 
	    {
		    m_gameState = MenuScreen;
	    }
		break;
	case MenuScreen:
		m_game.UpdateMainMenu(mTimer.DeltaTime());

		if (GetAsyncKeyState (VK_RETURN) & 0x0001) 
	    {
			m_game.RestartGame();
		    m_gameState = GameScreen;
	    }
	    else if (GetAsyncKeyState (VK_ESCAPE) & 0x0001) 
	    {
		    PostQuitMessage(0);
		}
		break;
	}
}

void SoundRunApp::Render()
{
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

	switch(m_gameState)
	{
	case GameScreen:	
		md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Red));
		md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

		// restore default states, as the SkyFX changes them in the effect file.
		md3dImmediateContext->RSSetState(0);
		md3dImmediateContext->OMSetDepthStencilState(0, 0);
		md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);			
		break;

	case MenuScreen:
		md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Blue));
		md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

		// restore default states, as the SkyFX changes them in the effect file.
		md3dImmediateContext->RSSetState(0);
		md3dImmediateContext->OMSetDepthStencilState(0, 0);
		md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff); 
		break;
	}

	HR(mSwapChain->Present(0, 0));
}

void SoundRunApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void SoundRunApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void SoundRunApp::OnMouseMove(WPARAM btnState, int x, int y)
{
}