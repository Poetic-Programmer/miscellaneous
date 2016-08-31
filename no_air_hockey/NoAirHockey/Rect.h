#ifndef RECT_H
#define RECT_H

#include "Vector2.h"
#include "Circle.h"

class Rect
{
public:
	Rect();

	Rect(const Vector2& a, const Vector2& b);

	Rect(float x0, float y0, float x1, float y1);

	void forceInside(Circle& A);

	bool isPointInside(const Vector2& pt);

	Vector2 mMinPoint;
	Vector2 mMaxPoint;
};

#endif