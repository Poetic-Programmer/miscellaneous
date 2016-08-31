#ifndef D3DAPP_H
#define D3DAPP_H

#include "D3DUtil.h"
#include "SceneTimer.h"
#include <string>

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	HINSTANCE AppInst()const;
	HWND      MainWnd()const;
	float     AspectRatio()const;

	int Run();

	virtual bool Initialize();
	virtual void OnResize();
	virtual void UpdateScene(float dt) = 0;
	virtual void DrawScene() = 0;
	virtual LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void OnMouseDown(WPARAM buttonState, int x, int y){}
	virtual void OnMouseUp(WPARAM buttonState, int x, int y){}
	virtual void OnMouseMove(WPARAM buttonState, int x, int y) {}
protected:
	bool InitializeMainWindow();
	bool InitializeDirect3D();

	void CalculateFrameInfo();

protected:
	HINSTANCE m_hAppInstance;
	HWND      m_hMainWindow;
	bool      m_appPaused;
	bool      m_minimized;
	bool      m_maximized;
	bool      m_resizing;
	UINT      m_4XMSAAQuality;

	SceneTimer m_timer;

	ID3D11Device*           m_d3dDevice;
	ID3D11DeviceContext*    m_d3dImmediateContext;
	IDXGISwapChain*         m_swapChain;
	ID3D11Texture2D*        m_depthStencilBuffer;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT          m_screenViewport;

	std::wstring    m_mainWindowCaption;
	D3D_DRIVER_TYPE m_d3dDriverType;
	int             m_clientWidth;
	int             m_clientHeight;
	bool            m_enable4XMSAA;
};

#endif  //D3DApp