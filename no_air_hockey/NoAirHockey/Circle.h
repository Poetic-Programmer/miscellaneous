#ifndef CIRCLE_H
#define CIRCLE_H

#include "Vector2.h"

class Circle
{
public:
	Circle();
	Circle(float R, const Vector2& center);

	bool hits(Circle& A, Vector2& normal);

	float r;   //radius
	Vector2 c; //center point
};

#endif