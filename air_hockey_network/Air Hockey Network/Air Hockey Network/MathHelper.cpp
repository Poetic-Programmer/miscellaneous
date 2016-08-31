#include "MathHelper.h"
#include <float.h>
#include <cmath>

const float MathHelper::Infinity = FLT_MAX;
const float MathHelper::Pi       = 3.1415926535f;
const float MathHelper::DegreesToRadians = 3.1415926535f / 180.0f;
const float MathHelper::RadiansToDegrees = 180.0f / 3.1415926535f;
const Vector2 MathHelper::X_Axis_2D = Vector2(1, 0);
const Vector2 MathHelper::Y_Axis_2D = Vector2(1, 0);

float MathHelper::AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	// Quadrant 1 or 4
	if(x >= 0.0f)
	{
		theta = atanf(y / x);

		if(theta < 0.0f)
			theta += 2.0f * Pi;
	}
	// Quadrant 2 or 3
	else
		theta = atanf(y / x) + Pi;

	return theta;
}