
#include "D3DApp.h"
#include <WindowsX.h>
#include <iostream>
#include "Defines.h"
#include <sstream>

using namespace std;
namespace
{
	D3DApp* g_d3dApp = 0;
}

LRESULT CALLBACK
	MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return g_d3dApp->MsgProc(hwnd, msg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE hInstance)
: 	m_hAppInstance(hInstance),
	m_mainWindowCaption(L"Air Hockey Server"),
	m_d3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	m_clientWidth(800),
	m_clientHeight(600),
	m_enable4XMSAA(false),
	m_hMainWindow(0),
	m_appPaused(false),
	m_minimized(false),
	m_maximized(false),
	m_resizing(false),
	m_4XMSAAQuality(0),
	m_d3dDevice(0),
	m_d3dImmediateContext(0),
	m_swapChain(0),
	m_depthStencilBuffer(0),
	m_renderTargetView(0),
	m_depthStencilView(0)
{
	ZeroMemory(&m_screenViewport, sizeof(D3D11_VIEWPORT));

	g_d3dApp = this;
}

D3DApp::~D3DApp()
{
	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_depthStencilView);
	ReleaseCOM(m_swapChain);
	ReleaseCOM(m_depthStencilView);

	// Restore all default settings
	if(m_d3dImmediateContext)
		m_d3dImmediateContext->ClearState();

	ReleaseCOM(m_d3dImmediateContext);
	ReleaseCOM(m_d3dDevice);
}

HINSTANCE D3DApp::AppInst()const
{
	return m_hAppInstance;
}

HWND D3DApp::MainWnd()const
{
	return m_hMainWindow;
}

float D3DApp::AspectRatio()const
{
	return static_cast<float>(m_clientWidth) / (m_clientHeight);
}

int D3DApp::Run()
{
	MSG msg = {0};

	m_timer.Reset();

	while(msg.message != WM_QUIT)
	{
		// If there are window messages, handle them.
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// else, do cool stuff
		else
		{
			m_timer.Tick();

			if(!m_appPaused)
			{
				CalculateFrameInfo();
				UpdateScene(m_timer.DeltaTime());
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

bool D3DApp::Initialize()
{
	if(!InitializeMainWindow())
		return false;

	if(!InitializeDirect3D())
		return false;

	return true;
}


void D3DApp::OnResize()
{
	assert(m_d3dImmediateContext);
	assert(m_d3dDevice);
	assert(m_swapChain);

	// Release the old views, they hold out of date buffers.
	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_depthStencilView);
	ReleaseCOM(m_depthStencilBuffer);

	// Resize the swap chain and recreate the render target view.
	HR(m_swapChain->ResizeBuffers(1, m_clientWidth, m_clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backbuffer;
	HR(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer)));
	HR(m_d3dDevice->CreateRenderTargetView(backbuffer, 0, &m_renderTargetView));
	ReleaseCOM(backbuffer);

	// Create the Depth/Stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width     = m_clientWidth;
	depthStencilDesc.Height    = m_clientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// If using 4XMSAA, the swap chain must match MSAA values.
	if(m_enable4XMSAA)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m_4XMSAAQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags      = 0;

	HR(m_d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &m_depthStencilBuffer));
	HR(m_d3dDevice->CreateDepthStencilView(m_depthStencilBuffer, 0, &m_depthStencilView));

	// Bind the render target view and depth/stencil view to the pipeline.
	m_d3dImmediateContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// Set the viewport transform.
	m_screenViewport.TopLeftX = 0;
	m_screenViewport.TopLeftY = 0;
	m_screenViewport.Width    = static_cast<float>(m_clientWidth);
	m_screenViewport.Height   = static_cast<float>(m_clientHeight);
	m_screenViewport.MinDepth = 0.0f;
	m_screenViewport.MaxDepth = 1.0f;

	m_d3dImmediateContext->RSSetViewports(1, &m_screenViewport);
}

LRESULT D3DApp::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	//Pause when window is deactivated and unpause when active.
	case WM_ACTIVATE:
		if(LOWORD(wParam) == WA_INACTIVE)
		{
			m_appPaused = true;
			m_timer.Stop();
		}
		else
		{
			m_appPaused = false;
			m_timer.Start();
		}
		return 0;

	// Resize the window
	case WM_SIZE:
		m_clientWidth  = LOWORD(lParam);
		m_clientHeight = HIWORD(lParam);

		if(m_d3dDevice)
		{
			if(wParam == SIZE_MINIMIZED)
			{
				m_appPaused = true;
				m_minimized = true;
				m_maximized = false;
			}
			else if(wParam == SIZE_MAXIMIZED)
			{
				m_appPaused = false;
				m_minimized = false;
				m_maximized = true;
				OnResize();
			}
			else if(wParam == SIZE_RESTORED)
			{
				//Restoring from which size state?
				if(m_minimized)
				{
					m_appPaused = false;
					m_minimized = false;
					OnResize();
				}
				else if(m_maximized)
				{
					m_appPaused = false;
					m_maximized = false;
					OnResize();
				}
				else if(m_resizing)
				{
					// HE WHO UPDATES WHILE THE USER IS RESIZING HAS FORGOTTON THE
					// FACE OF HIS FATHER!!!
				}
				else
				{
					OnResize();
				}
			}
		}
		return 0;

	// User holding the resize bars
	case WM_ENTERSIZEMOVE:
		m_appPaused = true;
		m_resizing = true;
		m_timer.Stop();
		return 0;

	// User releases resize bars
	case WM_EXITSIZEMOVE:
		m_appPaused = false;
		m_resizing = false;
		m_timer.Start();
		OnResize();
		return 0;

	// Window being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// Menu active and user presses a wrong key.
	case WM_MENUCHAR:
		// Dont beep when we alt-enter
		return MAKELRESULT(0, MNC_CLOSE);

	// Prevent Window from becoming too small
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool D3DApp::InitializeMainWindow()
{
	// 1) Fill out a window class
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hAppInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"D3DWndClassName";

	// 2) Register the class
	if( !RegisterClass(&wc) )
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// 3) Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, m_clientWidth, m_clientHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	// 4) Create the Window
	m_hMainWindow = CreateWindow(L"D3DWndClassName", m_mainWindowCaption.c_str(), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hAppInstance, 0); 
	if(!m_hMainWindow)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	// 5) Show and Update the window
	ShowWindow(m_hMainWindow, SW_SHOW);
	UpdateWindow(m_hMainWindow);

	return true;
}

bool D3DApp::InitializeDirect3D()
{
	// 1) Create the device and device context
	UINT createDeviceFlags = 0;

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,
		m_d3dDriverType,
		0,
		createDeviceFlags,
		0, 0,
		D3D11_SDK_VERSION,
		&m_d3dDevice,
		&featureLevel,
		&m_d3dImmediateContext);

	if(FAILED(hr))
	{
		//MessageBox(0, L"D3D11CreateDevice FAILED!", 0, 0);
		//return false;
	}

	if(featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	//2) Check 4X MSAA quality support for our back buffer format.
	HR(m_d3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4XMSAAQuality));
	assert(m_4XMSAAQuality > 0);

	//3) Fill out a DXGI_SWAP_CHAIN_DESC structure.
	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferDesc.Width                   = m_clientWidth;
	scd.BufferDesc.Height                  = m_clientHeight;
	scd.BufferDesc.RefreshRate.Numerator   = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

	// Use 4XMSAA?
	if(m_enable4XMSAA)
	{
		scd.SampleDesc.Count   = 4;
		scd.SampleDesc.Quality = m_4XMSAAQuality - 1;
	}
	else
	{
		scd.SampleDesc.Count   = 1;
		scd.SampleDesc.Quality = 0;
	}

	scd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount  = 1;
	scd.OutputWindow = m_hMainWindow;
	scd.Windowed     = true;
	scd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags        = 0;

	// 4) Query the IDXGIFactory result
	IDXGIDevice* dxgiDevice = 0;
	HR(m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));
	      
	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	// 5) Create the Swap Chain
	HR(dxgiFactory->CreateSwapChain(m_d3dDevice, &scd, &m_swapChain));
	
	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	OnResize();

	return true;
}

void D3DApp::CalculateFrameInfo()
{
	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;

	// Average over 1 second
	if((m_timer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCount;
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << m_mainWindowCaption << L"   "
			 << L"FPS: " << fps << L" (ms)";
		SetWindowText(m_hMainWindow, outs.str().c_str());

		//reset
		frameCount = 0;
		timeElapsed += 1.0f;
	}
}