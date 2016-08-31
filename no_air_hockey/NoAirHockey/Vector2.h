#ifndef Vector2_H
#define Vector2_H

#include <windows.h>
#include <cmath>

class Vector2
{
public:
	Vector2();
	Vector2(float s, float t);
	Vector2(float d[2]);
	Vector2(const POINT& p);

	Vector2 operator+(const Vector2& rhs)const;
	Vector2 operator-(const Vector2& rhs)const;
	Vector2 operator-();

	operator POINT();

	void operator+=(const Vector2& rhs);
	void operator-=(const Vector2& rhs);
	void operator*=(float s);
	void operator/=(float s);

	float length();
	Vector2& normalize();

	float dot(const Vector2& rhs);

	Vector2& reflect(const Vector2& normal);
	void rotate(float angle);

	// Data members.
	float mX;
	float mY;
};

Vector2 operator*(const Vector2& v, float s);
Vector2 operator*(float s, const Vector2& v);
Vector2 operator/(const Vector2& v, float s);
Vector2 operator/(float s, const Vector2& v);

#endif