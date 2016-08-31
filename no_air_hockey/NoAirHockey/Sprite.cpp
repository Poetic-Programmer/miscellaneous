#include "Sprite.h"

Sprite::Sprite()
{
	mhAppInst = 0;

	//load the bitmap resources
	mhImage = 0;
	mhMask  = 0;

	//get the bitmap structure
	GetObject(mhImage, sizeof(BITMAP), &mImageBM);
	GetObject(mhMask, sizeof(BITMAP), &mMaskBM);

	mPosition = 0;
	mVelocity = 0;
}
Sprite::Sprite(HINSTANCE hAppInst, int imageID, int maskID,
	const Circle& bc, const Vector2& p0, const Vector2& v0)
{
	mhAppInst = hAppInst;

	//load the bitmap resources
	mhImage = LoadBitmap(hAppInst, MAKEINTRESOURCE(imageID));
	mhMask = LoadBitmap(hAppInst, MAKEINTRESOURCE(maskID));

	//get the bitmap structure
	GetObject(mhImage, sizeof(BITMAP), &mImageBM);
	GetObject(mhMask, sizeof(BITMAP), &mMaskBM);

	//image and mask should be the same dimensions
	assert(mImageBM.bmWidth == mMaskBM.bmWidth);
	assert(mImageBM.bmHeight == mMaskBM.bmHeight);

	mBoundingCircle = bc;
	mPosition       = p0;
	mVelocity       = v0;
}

Sprite::~Sprite()
{
	DeleteObject(mhImage);
	DeleteObject(mhMask);
}

Sprite& Sprite::operator=(const Sprite& other)
{
	if(this == &other) return *this;

	mhAppInst = other.mhAppInst;

	//load the bitmap resources
	mhImage = other.mhImage;
	mhMask = other.mhMask;

	//get the bitmap structure
	GetObject(mhImage, sizeof(BITMAP), &mImageBM);
	GetObject(mhMask, sizeof(BITMAP), &mMaskBM);

	//image and mask should be the same dimensions
	assert(mImageBM.bmWidth == mMaskBM.bmWidth);
	assert(mImageBM.bmHeight == mMaskBM.bmHeight);

	mBoundingCircle = other.mBoundingCircle;
	mPosition       = other.mPosition;
	mVelocity       = other.mVelocity;

	return *this;
}


int Sprite::width()
{
	return mImageBM.bmWidth;
}

int Sprite::height()
{
	return mImageBM.bmHeight;
}

void Sprite::rotate(float angle)
{
	//mPosition.rotate(angle);
}

void Sprite::move(float x)
{
	mPosition.mX += x;
}

void Sprite::moveTo(float x, float y)
{
	mPosition.mX = x;
	mPosition.mY = y;
}

void Sprite::update(float dt)
{
	//update the sprites position
	mPosition += mVelocity * dt;

	//update the bounding circle
	mBoundingCircle.c = mPosition;
}

void Sprite::render(HDC hBackBufferDC, HDC hSpriteDC)
{
	int w = width();
	int h = height();

	//upper left corner
	int x = (int)mPosition.mX - (w / 2);
	int y = (int)mPosition.mY - (w / 2);

	//select the mask bitmap
	HGDIOBJ oldObj = SelectObject(hSpriteDC, mhMask);

	//draw the mask to the backbuffer
	BitBlt(hBackBufferDC, x, y, w, h, hSpriteDC, 0, 0, SRCAND);

	//select the image bitmap
	SelectObject(hSpriteDC, mhImage);

	//draw the image to the backbuffer
	BitBlt(hBackBufferDC, x, y, w, h, hSpriteDC, 0, 0, SRCPAINT);

	//restore the origional bitmap object
	SelectObject(hSpriteDC, oldObj);
}