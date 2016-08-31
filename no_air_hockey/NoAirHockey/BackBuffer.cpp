#include "BackBuffer.h"

BackBuffer::BackBuffer(HWND hWnd, int width, int height)
{
	mhWnd = hWnd; //save a copy of the main window handle

	HDC hWndDC = GetDC(hWnd); //get a handle to the DC 

	//save dimensions
	mWidth = width; 
	mHeight = height;

	mhDC = CreateCompatibleDC(hWndDC); //create system memory DC

	//create the backbuffer surface bitmap
	mhSurface = CreateCompatibleBitmap(
		hWndDC, width, height);

	ReleaseDC(hWnd, hWndDC); //release DC

	//select the backbuffer bitmap into the DC
	mhOldObject = (HBITMAP)SelectObject(mhDC, mhSurface);
}

BackBuffer::~BackBuffer()
{
	SelectObject(mhDC, mhOldObject);
	DeleteObject(mhSurface);
	DeleteDC(mhDC);
}

HDC BackBuffer::getDC()
{
	return mhDC;
}

int BackBuffer::height()
{
	return mHeight;
}

int BackBuffer::width()
{
	return mWidth;
}

void BackBuffer::present()
{
	HDC hWndDC = GetDC(mhWnd);

	BitBlt(hWndDC, 0, 0, mWidth, mHeight,
		mhDC, 0, 0, SRCCOPY);

	ReleaseDC(mhWnd, hWndDC);
}