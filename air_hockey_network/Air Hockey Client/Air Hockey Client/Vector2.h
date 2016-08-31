#ifndef Vector2_H
#define Vector2_H

#include <windows.h>
#include <cmath>

class Vector2
{
public:
	Vector2();
	Vector2(float x, float y);
	Vector2(float d[2]);

	Vector2 operator+(const Vector2& other)const;
	Vector2 operator-(const Vector2& other)const;
	Vector2 operator-();

	operator POINT();

	void operator+=(const Vector2& other);
	void operator-=(const Vector2& other);
	void operator*=(const Vector2& other);
	void operator*=(float s);
	void operator/=(float s);

	float Length();
	Vector2& Normalize();
	Vector2& Rotate(float angle);

	float Dot(const Vector2& other);

	Vector2& Reflect(const Vector2& normal);
	Vector2 Perp();

	// Data members.
	float x;
	float y;
};

// Define with left hand and right hand sides reversed
// so we can write both v*s and s*v.
Vector2 operator*(const Vector2& v, float s);
Vector2 operator*(float s, const Vector2& v);
Vector2 operator/(const Vector2& v, float s);
Vector2 operator/(float s, const Vector2& v);

#endif // Vector3_H