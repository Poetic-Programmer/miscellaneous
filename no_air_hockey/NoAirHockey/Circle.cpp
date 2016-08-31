#include "Circle.h"

Circle::Circle()
	: r(0.0f), c(0.0f, 0.0f)
{}

Circle::Circle(float R, const Vector2& center)
	: r(R), c(center)
{}

bool Circle::hits(Circle& A, Vector2& normal)
{
	Vector2 x = A.c - c;

	if(x.length() <= r + A.r)
	{
		normal = x;
		normal.normalize();

		A.c = c + (normal * (r + A.r));

		return true;
	}
	return false;
}