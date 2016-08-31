#ifndef BACKBUFFER_H
#define BACKBUFFER_H

#include <Windows.h>

class BackBuffer
{
public:
	BackBuffer(HWND hWnd, int width, int height);
	~BackBuffer();

	HDC getDC();

	int width();
	int height();

	void present();

private:
	HWND mhWnd;          //handle to the main window
	HDC  mhDC;           //handle to the system memory dc
	HBITMAP mhSurface;   //handle to bitmap we will draw to
	HBITMAP mhOldObject; //handle to previous bitmap
	int mWidth;          //width of bitmap 
	int mHeight;         //height of bitmap
};

#endif