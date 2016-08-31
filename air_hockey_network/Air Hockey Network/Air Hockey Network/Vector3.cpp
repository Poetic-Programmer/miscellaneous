#include "Vector3.h"
#include <cmath>

Vector3::Vector3()
: x(0.0f), y(0.0f), z(0.0f)
{
}

Vector3::Vector3(float x, float y, float z)
: x(x), y(y), z(z)
{
}

Vector3::Vector3(float d[3])
: x(d[0]), y(d[1]), z(d[2])
{
}

Vector3::Vector3(const POINT& p)
: x((float)p.x), y((float)p.y), z(0.0f)
{
}

Vector3 Vector3::operator+(const Vector3& other)const
{
	Vector3 result;
	result.x = x + other.x;
	result.y = y + other.y;
	result.z = z + other.z;
	return result;
}

Vector3 Vector3::operator-(const Vector3& other)const
{
	Vector3 result;
	result.x = x - other.x;
	result.y = y - other.y;
	result.z = z - other.z;
	return result;
}

Vector3 Vector3::operator-()
{
	Vector3 result;
	result.x = -x;
	result.y = -y;
	result.z = -z;
	return result;
}

Vector3::operator POINT()
{
	POINT p = {(int)x, (int)y};
	return p;
}

void Vector3::operator+=(const Vector3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
}

void Vector3::operator-=(const Vector3& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
}

void Vector3::operator*=(const Vector3& other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
}

void Vector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
}

void Vector3::operator/=(float s)
{
	// Assumes s != 0.
	s /= s;
	y /= s;
}

float Vector3::length()
{
	return sqrtf(x*x + y*y + z*z);
}

Vector3& Vector3::rotate(float angle)
{
	float xnew = x * cosf(angle) - y * sinf(angle);
	float ynew = y * cosf(angle) + x * sinf(angle);

	x = xnew;
	y = ynew;
	
	return *this;
}

Vector3& Vector3::normalize()
{
	// Assumes len != 0.
	float len = length();

	x /= len;
	y /= len;
	z /= len;

	return *this;
}

float Vector3::dot(const Vector3& other)
{
	return x*other.x + y*other.y + z*other.z;
}

Vector3& Vector3::reflect(const Vector3& normal, float c)
{
	Vector3 I = *this;

	*this = c * (2.0f * (-I.dot(normal)) * normal + I);

	return *this;
}

Vector3 operator*(const Vector3& v, float s)
{
	Vector3 result;

	result.x = v.x * s;
	result.y = v.y * s;
	result.z = v.z * s;

	return result;
}

Vector3 operator*(float s, const Vector3& v)
{
	Vector3 result;

	result.x = v.x * s;
	result.y = v.y * s;
	result.z = v.z * s;

	return result;
}

Vector3 operator/(const Vector3& v, float s)
{
	Vector3 result;

	// Assumes s != 0.
	result.x = v.x / s;
	result.y = v.y / s;
	result.z = v.z / s;

	return result;
}

Vector3 operator/(float s, const Vector3& v)
{
	Vector3 result;

	// Assumes s != 0.
	result.x = v.x / s;
	result.y = v.y / s;
	result.z = v.z / s;

	return result;
}