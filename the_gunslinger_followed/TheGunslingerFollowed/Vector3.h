#ifndef Vector3_H
#define Vector3_H

#include <windows.h>
#include <cmath>

class Vector3
{
public:
	Vector3();
	Vector3(float x, float y, float z);
	Vector3(float d[3]);
	Vector3(const POINT& p);

	Vector3 operator+(const Vector3& other)const;
	Vector3 operator-(const Vector3& other)const;
	Vector3 operator-();

	operator POINT();

	void operator+=(const Vector3& other);
	void operator-=(const Vector3& other);
	void operator*=(const Vector3& other);
	void operator*=(float s);
	void operator/=(float s);

	float length();
	Vector3& normalize();
	Vector3& rotate(float angle);

	float dot(const Vector3& other);

	Vector3& reflect(const Vector3& normal);

	// Data members.
	float x;
	float y;
	float z;
};

// Define with left hand and right hand sides reversed
// so we can write both v*s and s*v.
Vector3 operator*(const Vector3& v, float s);
Vector3 operator*(float s, const Vector3& v);
Vector3 operator/(const Vector3& v, float s);
Vector3 operator/(float s, const Vector3& v);

#endif // Vector3_H