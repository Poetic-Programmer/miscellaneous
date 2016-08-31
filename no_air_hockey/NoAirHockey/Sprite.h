#ifndef SPRITE_H
#define SPRITE_H

#include <Windows.h>
#include "Vector2.h"
#include "Circle.h"
#include "Rect.h"
#include <assert.h>

class Sprite
{
public:
	Sprite();
	Sprite(HINSTANCE hAppInst, int imageID, int maskID,
		const Circle& bc, const Vector2& p0, const Vector2& v0);

	~Sprite();

	int width();
	int height();

	void rotate(float angle);
	void move(float x);
	void moveTo(float x, float y);

	void update(float dt);
	void render(HDC hBackBufferDC, HDC hSpriteDC);

public:
	Circle mBoundingCircle;  //approx describes the are of sprite
	Vector2 mPosition; //the center position of the sprite
	Vector2 mVelocity; //the velocity of the sprite

	Sprite(const Sprite& rhs);
	Sprite& operator = (const Sprite& rhs);

protected:
	HINSTANCE mhAppInst; //a handle to the apllication instance
	HBITMAP mhImage; //a handle to the sprite image bitmap
	HBITMAP mhMask; //a handle to the sprite mask bitmap
	BITMAP mImageBM; //sprite image bitmap info
	BITMAP mMaskBM; //sprite mask bitmap info
};

#endif