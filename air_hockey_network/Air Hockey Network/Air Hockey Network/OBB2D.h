#ifndef OBB2D_H
#define OBB2D_H

#include "Vector2.h"
#include "MathHelper.h"
class OBB2D
{
public:
	OBB2D(Vector2 position, Vector2 xAxis, Vector2 yAxis, float width, float height);
	~OBB2D();

	Vector2 ClosestPoint(Vector2 point);
	void SetPosition(float x, float y);
	void SetAngle(float angle);

private:
	Vector2 m_centerPosition;
	Vector2 m_axis[2];
	float m_halfValues[2];
	int m_axisIndex;
};

#endif // OBB2D.h