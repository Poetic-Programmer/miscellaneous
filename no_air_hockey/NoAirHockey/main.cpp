//=========================================================
// Includes
//=========================================================
#include <string>
#include "resource.h"

#include "BackBuffer.h"

#include "Game.h"
#include <list>
#include <cstdio>
using namespace std;

//=========================================================
// Globals
//=========================================================
HWND      ghMainWnd  = 0;
HMENU     ghMainMenu = 0;
HDC       ghSpriteDC = 0;
HINSTANCE ghAppInst  = 0;

BackBuffer    * gBackBuffer     = 0;
Game          * gGame           = 0;

RECT gMapRect = {0, 0, 800, 600};

string gWndCaption = "No Air Hockey";

const int gClientWidth = 1000;
const int gClientHeight = 800;

// Center point of client rectangle.
const POINT gClientCenter =
{
	gClientWidth / 2,
	gClientHeight / 2
};

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void RegisterMyWindow(HINSTANCE hInstance);

bool InitializeMyWindow();

void DrawFramesPerSecond(float deltaTime);

LRESULT CALLBACK
	WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


int WINAPI
	WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR cmdLine, int nCmdShow)
{
	//save application instance
	ghAppInst = hInstance;

	if(!InitializeMyWindow())
	{
		return FALSE;
	}
	
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// Get the current time.
	float lastTime = (float)timeGetTime();

	float time = 0.0f;

	while (TRUE)
	{
		// IF there is a Windows message then process it.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			float currTime = (float)timeGetTime();

			float deltaTime = max((currTime - lastTime)*0.001f, 0.0f);

			time += deltaTime;

			if(time >= 0.01)
			{
				gGame->update((float)time);
				gGame->render(gBackBuffer->getDC(), ghSpriteDC);
				

				DrawFramesPerSecond(deltaTime);

				gBackBuffer->present();
				time = 0.0f;
			}
			lastTime = currTime;
		}
	}
	return msg.wParam;
}


void RegisterMyWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize        = sizeof(wcex);
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = ghAppInst;
	wcex.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wcex.hCursor       = LoadCursor(0, IDC_ARROW);

	wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = "MyWndClassName";
	wcex.hIconSm       = 0;

	RegisterClassEx( &wcex );
}

bool InitializeMyWindow()
{
	RegisterMyWindow(ghAppInst);

	ghMainWnd = ::CreateWindow("MyWndClassName",
		"",
		WS_OVERLAPPED | WS_SYSMENU, 0, 0, gClientWidth, gClientHeight, NULL, NULL, ghAppInst, NULL);

	if(ghMainWnd == 0)
	{
		return false;
	}

	ShowWindow (ghMainWnd, SW_NORMAL);
	UpdateWindow(ghMainWnd);

	return true;
}

void DrawFramesPerSecond(float deltaTime)
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;
	static char buffer[256];

	++frameCnt;

	timeElapsed += deltaTime;

	if( timeElapsed >= 1.0f )
	{
	
		sprintf_s(buffer, "--Frames Per Second = %d", frameCnt);

		string newCaption = gWndCaption + buffer;

		SetWindowText(ghMainWnd, newCaption.c_str());

		frameCnt = 0;
		timeElapsed = 0.0f;
	}
}

LRESULT CALLBACK
	WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_CREATE:

		gGame = new Game(ghAppInst, hWnd, gClientCenter);
		ghSpriteDC = CreateCompatibleDC(0);

		gBackBuffer = new BackBuffer(hWnd, gClientWidth, gClientHeight);

		return 0;

	case WM_KEYDOWN:
		switch(wParam)
		{
	
		case 'A':
	
			break;
		case 'D':
		
			break;
			
		case 'W':
			
			break;
	
		case 'S':
			
			break;

		case 'P':
			gGame->pause();

			break;
		}
		return 0;

	case WM_LBUTTONDOWN:
		gGame->unpause();
		return 0;

	case WM_RBUTTONDOWN:
		gGame->pause();
		return 0;

	case WM_DESTROY:
		delete gGame;
		delete gBackBuffer;
		DeleteDC(ghSpriteDC);
		PostQuitMessage(0);
		return 0;
	}
	// Forward any other messages we didn't handle to the
	// default window procedure.
	return DefWindowProc(hWnd, msg, wParam, lParam);
}