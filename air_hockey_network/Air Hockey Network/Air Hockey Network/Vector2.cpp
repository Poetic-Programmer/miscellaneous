#include "Vector2.h"
#include <cmath>

Vector2::Vector2()
: x(0.0f), y(0.0f)
{
}

Vector2::Vector2(float x, float y)
: x(x), y(y)
{
}

Vector2::Vector2(float d[2])
: x(d[0]), y(d[1])
{
}

Vector2 Vector2::operator+(const Vector2& other)const
{
	Vector2 result;
	result.x = x + other.x;
	result.y = y + other.y;
	return result;
}

Vector2 Vector2::operator-(const Vector2& other)const
{
	Vector2 result;
	result.x = x - other.x;
	result.y = y - other.y;
	return result;
}

Vector2 Vector2::operator-()
{
	Vector2 result;
	result.x = -x;
	result.y = -y;
	return result;
}

Vector2::operator POINT()
{
	POINT p = {(int)x, (int)y};
	return p;
}

void Vector2::operator+=(const Vector2& other)
{
	x += other.x;
	y += other.y;
}

void Vector2::operator-=(const Vector2& other)
{
	x -= other.x;
	y -= other.y;
}

void Vector2::operator*=(const Vector2& other)
{
	x *= other.x;
	y *= other.y;
}

void Vector2::operator*=(float s)
{
	x *= s;
	y *= s;
}

void Vector2::operator/=(float s)
{
	// Assumes s != 0.
	s /= s;
	y /= s;
}

float Vector2::Length()
{
	return sqrtf(x*x + y*y);
}

Vector2& Vector2::Rotate(float angle)
{
	float xnew = x * cosf(angle) - y * sinf(angle);
	float ynew = y * cosf(angle) + x * sinf(angle);

	x = xnew;
	y = ynew;
	
	return *this;
}

Vector2& Vector2::Normalize()
{
	// Assumes len != 0.
	float len = Length();

	x /= len;
	y /= len;

	return *this;
}

float Vector2::Dot(const Vector2& other)
{
	return x*other.x + y*other.y;
}

Vector2& Vector2::Reflect(const Vector2& normal)
{
	Vector2 I = *this;

	*this = ((2.0f * -I.Dot(normal)) * normal) + I;

	return *this;
}

Vector2 operator*(const Vector2& v, float s)
{
	Vector2 result;

	result.x = v.x * s;
	result.y = v.y * s;

	return result;
}

Vector2 operator*(float s, const Vector2& v)
{
	Vector2 result;

	result.x = v.x * s;
	result.y = v.y * s;

	return result;
}

Vector2 operator/(const Vector2& v, float s)
{
	Vector2 result;

	// Assumes s != 0.
	result.x = v.x / s;
	result.y = v.y / s;

	return result;
}

Vector2 operator/(float s, const Vector2& v)
{
	Vector2 result;

	// Assumes s != 0.
	result.x = v.x / s;
	result.y = v.y / s;

	return result;
}

Vector2 Vector2::Perp()
{
    Vector2 temp = Vector2();
    temp.x = -y;
    temp.y = x;
    return temp;
}