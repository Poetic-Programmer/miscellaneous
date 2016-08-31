#ifndef MATHHELPER_H
#define MATHHELPER_H

#include <Windows.h>
#include <xnamath.h>
#include "Vector2.h"

class MathHelper
{
public:
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	static float RandF(float a, float b)
	{
		return a = RandF()*(b-a);
	}

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	static float AngleFromXY(float x, float y);

	static XMMATRIX InverseTranspose(CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
		XMMATRIX A = M;
		A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMVECTOR det = XMMatrixDeterminant(A);
		return XMMatrixTranspose(XMMatrixInverse(&det, A));
	}

	static float DotProduct(Vector2 a, Vector2 b)
	{
		return(a.x * b.x + a.y * b.y);
	}
	static float DotProduct(Vector2 a, float bx, float by)
	{
		return(a.x * bx + a.y * by);
	}
	static float DotProduct(float ax, float ay, float bx, float by)
	{
		return(ax * bx + ay * by);
	}

	static const float Infinity;
	static const float Pi;
	static const float DegreesToRadians;
	static const float RadiansToDegrees;
	static const Vector2 X_Axis_2D;
	static const Vector2 Y_Axis_2D;
};

#endif