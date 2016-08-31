#include "Rect.h"

Rect::Rect() {}

Rect::Rect(const Vector2& a, const Vector2& b)
	: mMinPoint(a), mMaxPoint(b)
{}

Rect::Rect(float x0, float y0, float x1, float y1)
	: mMinPoint(x0, y0), mMaxPoint(x1, y1)
{}

void Rect::forceInside(Circle &A)
{
	Vector2 p = A.c; //center point of circle
	float r = A.r;   //radius

	//no part of circle is allowed outside min or max point
	if(p.mX - r < mMinPoint.mX)
		p.mX = mMinPoint.mX + r;
	if(p.mX + r > mMaxPoint.mX)
		p.mX = mMaxPoint.mX - r;

	if(p.mY - r < mMinPoint.mY)
		p.mY = mMinPoint.mY + r;
	if(p.mY + r > mMaxPoint.mY)
		p.mY = mMaxPoint.mY - r;

	//keep in bounds
	A.c = p;
}

bool Rect::isPointInside(const Vector2& pt)
{
	return pt.mX >= mMinPoint.mX && pt.mY >= mMinPoint.mY &&
		pt.mX <= mMaxPoint.mX && pt.mY <= mMaxPoint.mY;
}