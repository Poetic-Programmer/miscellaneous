/////////////////////////////////////////////////////////////
//Mandelbrot Thread 
//Michael Dunleavy
//Last edit: 23/04/2011
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//INCLUDES
////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <mmsystem.h>
#include <math.h>
#include <process.h>

#include <gl/gl.h>
#include <gl/glu.h>


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <complex>

#ifdef _WIN32
#include <tchar.h>
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

using namespace std;

/////////////////////////////////////////////////////////////////////////////////
//STRUCTS
/////////////////////////////////////////////////////////////////////////////////
struct cMandel
{
    double left, right, top, bottom;
    int yMin, yMax;
};

cMandel myStruct;
/////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
/////////////////////////////////////////////////////////////

//Mandelbot and timing
void compute_mandelbrot(double left, double right, double top, double bottom,
                        int ymin, int ymax);
void DrawScene();
void keyPress();
void renderMandelThread();
void renderMandel();
void getTime(float startTime, float endTime);

//Win32 and OpenGL setup
bool SetPixelFormat(HDC hdc);
void ResizeGLWindow(int width, int height);
void InitializeOpenGL(int width, int height);
void Init(HWND hwnd);
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND CreateOurWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle,
                     bool bFullScreen, HINSTANCE hInstance);

/////////////////////////////////////////////////////////////////////////////////
//DEFINES and Globals
/////////////////////////////////////////////////////////////////////////////////
#define MAX_PIXEL    2000
#define COLOUR_DEPTH 16    //Colour depth

const int WIDTH    = 800;
const int HEIGHT   = 600;
int MAX_ITERATIONS = 100;
const int MAX_THREAD_COUNT = 12;
float pixel[HEIGHT*WIDTH][3];

float count = 0;
int S_WIDTH     =    800;        //client area resolution
int S_HEIGHT    =    600;

//////////////////////////////////////////////////////////////////////////////////
//HANDELS & Screen info
//////////////////////////////////////////////////////////////////////////////////
HWND         ghwnd;
HDC          ghdc;
HGLRC        ghrc;            //hardware RENDERING CONTEXT
HINSTANCE    ghInstance;
RECT         gRect;
RECT         screenRect;

CRITICAL_SECTION cs;
HANDLE hThread[MAX_THREAD_COUNT];
HANDLE hSemaphore;

bool        keys[256];

unsigned int __stdcall computeThread(void * data)
{
    cMandel * args = (cMandel*) data;

    compute_mandelbrot(args->left, args->right, args->top, args->bottom, args->yMin, args->yMax);

	return 1;
}

void getTime(float startTime, float endTime)
{	
	float result = (endTime - startTime) / 1000;

	count = result;
	WaitForSingleObject(hSemaphore, INFINITE);
	DrawScene();		
}

// Render the Mandelbrot set into the image array.
// The parameters specify the region on the complex plane to plot.
void compute_mandelbrot(double left, double right, double top, double bottom,
                        int yMin, int yMax)
{
	int iterations = 0;

    for (int y = yMin; y < yMax; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
			
            // Work out the point in the complex plane that
            // corresponds to this pixel in the output image.
            complex<double> c(left + (x * (right - left) / WIDTH),
                              top + (y * (bottom - top) / HEIGHT));

            // Start off z at (0, 0).
            complex<double> z(0.0, 0.0);
            iterations = 0;
            // Iterate z = z^2 + c until z moves more than 2 units
            // away from (0, 0), or we've iterated too many times.
            //int iterations = 0;
            while (abs(z) < 2.0 && iterations < MAX_ITERATIONS) {
                z = (z * z) + c;

                ++iterations;
            }

			EnterCriticalSection(&cs);
            if (iterations == MAX_ITERATIONS) 
            {
                // z didn't escape from the circle.
                // This point is in the Mandelbrot set.
                pixel[y * WIDTH + x][0] = 0.0f;
                pixel[y * WIDTH + x][1] = 0.0f;
                pixel[y * WIDTH + x][2] = 0.0f;            

            } 
            else 
            {
                // z escaped within less than MAX_ITERATIONS
                // iterations. This point isn't in the set.
                pixel[y * WIDTH + x][0] = (float) abs(z)/iterations * 1.2f;
                pixel[y * WIDTH + x][1] = (float) abs(z)/iterations;    
                pixel[y * WIDTH + x][2] = 127.0f;    
            }
			LeaveCriticalSection(&cs);
        }	
    }
	ReleaseSemaphore(hSemaphore, 1, NULL);
}

void DrawScene() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Clear The Screen And The Depth Buffer
    glLoadIdentity();// load Identity Matrix

    glClear(GL_COLOR_BUFFER_BIT);    

	EnterCriticalSection(&cs);
    glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_FLOAT, pixel);
	LeaveCriticalSection(&cs);

	char buffer[256];

	sprintf_s(buffer, "Time taken: %f,   Iterations:  %d,   Threads:  %d,   Left: %f, Right: %f, Top: %f, Bottom: %f", 
		count, MAX_ITERATIONS, MAX_THREAD_COUNT, 
		(float)myStruct.left, (float)myStruct.right, 
		(float)myStruct.top, (float)myStruct.bottom);

	SetWindowText(ghwnd, buffer);

    SwapBuffers(ghdc);// Swap the frame buffers.

}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int nCmdShow)            
{    
    HWND   hwnd;
    MSG    msg;

    //initialise and create window
    hwnd = CreateOurWindow("SuperDuper Opengl APP", S_WIDTH, S_HEIGHT, 0, false, hInstance);    
    if(hwnd == NULL) return true;


    //initialise opengl and other settings
    Init(hwnd);



	myStruct.left   = -2.0;
	myStruct.right  = 1.0;
	myStruct.top    = 1.125;
	myStruct.bottom = -1.125;

	/*
	myStruct.left   = -0.751085;
	myStruct.right  = -0.734975;
	myStruct.top    = 0.118378;
	myStruct.bottom = 0.134488;
	*/
	InitializeCriticalSection(&cs);
	hSemaphore = CreateSemaphore(NULL, 0, 10, NULL);
    while (true)                    
    {        

        if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        {
            if (msg.message==WM_QUIT)
                break;
            TranslateMessage (&msg);                            
            DispatchMessage (&msg);
        }

        else
        {        
            //any intensive proccessing for the app,  do it here.         
            keyPress();
            DrawScene();
        }
    }

    return msg.wParam ;                                        
}

void renderMandelThread()
{
    myStruct.yMax = 0;
    myStruct.yMin = 0;

    float startTime = 0;
    float endTime = 0;


	startTime = (float)timeGetTime();

    for(int i = 0; i < MAX_THREAD_COUNT; ++i)
    {
        myStruct.yMax += (HEIGHT / MAX_THREAD_COUNT);

		hThread[i] = (HANDLE) _beginthreadex(NULL, MAX_THREAD_COUNT, computeThread, &myStruct, NULL, 0);
	    DrawScene();

        myStruct.yMin += (HEIGHT / MAX_THREAD_COUNT);
    } 
	WaitForMultipleObjects(MAX_THREAD_COUNT, hThread, true, INFINITE);
	endTime = (float)timeGetTime();


	getTime(startTime, endTime);


	for(int i = 0; i < MAX_THREAD_COUNT; ++i)
		CloseHandle(hThread[i]);
}

void renderMandel()
{
    myStruct.yMax = HEIGHT;
    myStruct.yMin = 0;

	float startTime = 0;
	float endTime = 0;

	startTime = (float)timeGetTime();

    compute_mandelbrot(myStruct.left, myStruct.right, myStruct.top, myStruct.bottom, 
        myStruct.yMin, myStruct.yMax);

	endTime = (float)timeGetTime();

	getTime(startTime, endTime);
}

void keyPress()
{
    if(keys[VK_RETURN])
    {
        renderMandelThread();
    }
	if(keys[VK_SPACE])
	{
		renderMandel();
	}

    if(keys[VK_LEFT])
    {
		if(MAX_ITERATIONS >= 0)
			MAX_ITERATIONS--;
		Sleep(10);
    }

	if(keys[VK_RIGHT])
	{
		if(MAX_ITERATIONS <= 1000)
			MAX_ITERATIONS++;
		Sleep(10);
	}

    if(keys[VK_UP])
    {
		myStruct.left   *= 0.9;
		myStruct.right  *= 0.9;
		myStruct.top    *= 0.9;
		myStruct.bottom *= 0.9;
		Sleep(10);
    }

    if(keys[VK_DOWN])
    {
		myStruct.left   *= 1.1;
		myStruct.right  *= 1.1;
		myStruct.top    *= 1.1;
		myStruct.bottom *= 1.1;
		Sleep(10);
    }
}

HWND CreateOurWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance)
{
    HWND hwnd;

    WNDCLASS wcex;

    memset(&wcex, 0, sizeof(WNDCLASS));
    wcex.style            = CS_HREDRAW | CS_VREDRAW;        
    wcex.lpfnWndProc    = WndProc;        
    wcex.hInstance        = hInstance;                        
    wcex.hIcon            = LoadIcon(NULL, IDI_APPLICATION);; 
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);        
    wcex.hbrBackground    = (HBRUSH) (COLOR_WINDOW+1);
    wcex.lpszMenuName    = NULL;    
    wcex.lpszClassName    = "FirstWindowClass";    

    
    RegisterClass(&wcex);// Register the class

    dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    ghInstance    = hInstance;// Assign our global hInstance to the window's hInstance

    //Set the Client area of the window to be our resolution.
    RECT glwindow;
    glwindow.left        = 0;        
    glwindow.right        = width;    
    glwindow.top        = 0;        
    glwindow.bottom        = height;    

    AdjustWindowRect( &glwindow, dwStyle, false);

    //Create the window
    hwnd = CreateWindow(    "FirstWindowClass", 
                            strWindowName, 
                            dwStyle, 
                            0, 
                            0,
                            glwindow.right  - glwindow.left,
                            glwindow.bottom - glwindow.top, 
                            NULL,
                            NULL,
                            hInstance,
                            NULL
                            );

    if(!hwnd) return NULL;// If we could get a handle, return NULL

    ShowWindow(hwnd, SW_SHOWNORMAL);    
    UpdateWindow(hwnd);                    
    SetFocus(hwnd);                        

    return hwnd;
}

bool SetPixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd = {0}; 
    int pixelformat; 
 
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);    // Set the size of the structure
    pfd.nVersion = 1;                            // Always set this to 1
    // Pass in the appropriate OpenGL flags
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask = PFD_MAIN_PLANE;            // standard mask (this is ignored anyway)
    pfd.iPixelType = PFD_TYPE_RGBA;                // RGB and Alpha pixel type
    pfd.cColorBits = COLOUR_DEPTH;                // Here we use our #define for the color bits
    pfd.cDepthBits = COLOUR_DEPTH;                // Ignored for RBA
    pfd.cAccumBits = 0;                            // nothing for accumulation
    pfd.cStencilBits = 0;                        // nothing for stencil
 
    //Gets a best match on the pixel format as passed in from device
    if ( (pixelformat = ChoosePixelFormat(hdc, &pfd)) == false ) 
    { 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return false; 
    } 
 
    //sets the pixel format if its ok. 
    if (SetPixelFormat(hdc, pixelformat, &pfd) == false) 
    { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return false; 
    } 
 
    return true;
}

void ResizeGLWindow(int width, int height)// Initialize The GL Window
{
    if (height==0)// Prevent A Divide By Zero error
    {
        height=1;// Make the Height Equal One
    }

    glViewport(0,0,WIDTH,HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //calculate aspect ratio
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, 1 ,150.0f);

    glMatrixMode(GL_MODELVIEW);// Select The Modelview Matrix
    glLoadIdentity();// Reset The Modelview Matrix
}

void InitializeOpenGL(int width, int height) 
{  
    ghdc = GetDC(ghwnd);//  sets  global HDC

    if (!SetPixelFormat(ghdc))//  sets  pixel format
        PostQuitMessage (0);


    ghrc = wglCreateContext(ghdc);    //  creates  rendering context from  hdc
    wglMakeCurrent(ghdc, ghrc);        //    Use this HRC.

    ResizeGLWindow(width, height);    // Setup the Screen
}



void Init(HWND hwnd)
{
    ghwnd = hwnd;
    GetClientRect(ghwnd, &gRect);    //get rect into our handy global rect
    InitializeOpenGL(gRect.right, gRect.bottom);// initialise openGL

    //OpenGL settings
    glShadeModel(GL_SMOOTH);                            // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);                // Black Background
    glClearDepth(1.0f);                                    // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);                            // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);                                // The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);    // Really Nice Perspective Calculations

    //Also, do any other setting ov variables here for your app if you wish. 
}

void Cleanup()
{
    if (ghrc)
    {
        wglMakeCurrent(NULL, NULL);    // free rendering memory
        wglDeleteContext(ghrc);        // Delete our OpenGL Rendering Context
    }

    if (ghdc) 
    ReleaseDC(ghwnd, ghdc);            // Release our HDC from memory

    UnregisterClass("FirstWindowClass", ghInstance);// Free the window class

    PostQuitMessage (0);        // Post a QUIT message to the window
}



LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{


    switch (message)                                            
    {                                                        
        case WM_CREATE:    
            break;

        case WM_SIZE:
            //resize the open gl window when the window is resized
            ResizeGLWindow(LOWORD(lParam),HIWORD(lParam));
            GetClientRect(hwnd, &gRect);
            break;    

        case WM_KEYDOWN:
            keys[wParam]=true;
            break;

        case WM_KEYUP:
            keys[wParam]=false;
            break;

        case WM_PAINT:
            
    
            break;        

        case WM_DESTROY:
			DeleteCriticalSection(&cs);
            PostQuitMessage(0);    
                                
            break;                
    }                                                    

    return DefWindowProc (hwnd, message, wParam, lParam);        
                                                            
}