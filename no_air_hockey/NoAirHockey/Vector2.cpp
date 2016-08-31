#include "Vector2.h"
#include <cmath>

Vector2::Vector2()
: mX(0.0f), mY(0.0f)
{
}

Vector2::Vector2(float s, float t)
: mX(s), mY(t)
{
}

Vector2::Vector2(float d[2])
: mX(d[0]), mY(d[1])
{
}

Vector2::Vector2(const POINT& p)
: mX((float)p.x), mY((float)p.y)
{
}

Vector2 Vector2::operator+(const Vector2& rhs)const
{
	Vector2 result;
	result.mX = mX + rhs.mX;
	result.mY = mY + rhs.mY;

	return result;
}

Vector2 Vector2::operator-(const Vector2& rhs)const
{
	Vector2 result;
	result.mX = mX - rhs.mX;
	result.mY = mY - rhs.mY;

	return result;
}

Vector2 Vector2::operator-()
{
	Vector2 result;
	result.mX = -mX;
	result.mY = -mY;

	return result;
}

Vector2::operator POINT()
{
	POINT p = {(int)mX, (int)mY};
	return p;
}

void Vector2::operator+=(const Vector2& rhs)
{
	mX += rhs.mX;
	mY += rhs.mY;
}

void Vector2::operator-=(const Vector2& rhs)
{
	mX -= rhs.mX;
	mY -= rhs.mY;
}

void Vector2::operator*=(float s)
{
	mX *= s;
	mY *= s;
}

void Vector2::operator/=(float s)
{
	s /= s;
	mY /= s;
}

float Vector2::length()
{
	return sqrtf(mX*mX + mY*mY);
}

Vector2& Vector2::normalize()
{
	float len = length();

	mX /= len;
	mY /= len;

	return *this;
}

float Vector2::dot(const Vector2& rhs)
{
	return mX*rhs.mX + mY*rhs.mY;
}

Vector2& Vector2::reflect(const Vector2& normal)
{
	Vector2 I = *this;

	*this = ((2.0f * -I.dot(normal)) * normal) + I;

	return *this;
}

void Vector2::rotate(float angle)
{
	float newX = 0.0f;
	float newY = 0.0f;
	newX = mX * cosf(angle) - mY * sinf(angle);
	newY = mY * cosf(angle) - mX * sinf(angle);

	mX = newX;
	mY = newY;
}

Vector2 operator*(const Vector2& v, float s)
{
	Vector2 result;

	result.mX = v.mX * s;
	result.mY = v.mY * s;

	return result;
}

Vector2 operator*(float s, const Vector2& v)
{
	Vector2 result;

	result.mX = v.mX * s;
	result.mY = v.mY * s;

	return result;
}

Vector2 operator/(const Vector2& v, float s)
{
	Vector2 result;

	result.mX = v.mX / s;
	result.mY = v.mY / s;

	return result;
}

Vector2 operator/(float s, const Vector2& v)
{
	Vector2 result;

	result.mX = v.mX / s;
	result.mY = v.mY / s;

	return result;
}