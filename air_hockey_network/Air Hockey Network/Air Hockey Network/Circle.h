#ifndef Circle_H
#define Circle_H

#include "Vector2.h"

class Circle
{
public:
	Circle();
	Circle(float x, float y, float radius);
	~Circle();

	void SetCenterPosition(float x, float y);

	Vector2 GetCenterPosition();
	float GetRadius();
	float GetRadiusSuqared();

private:
	float m_radius;
	Vector2 m_centerPosition;
};

#endif // Circle.h