#ifndef CIRCLE_H
#define CIRCLE_H

#include "Vector3.h"

class Circle
{
public:
	Circle(float x, float y, float z, float radius);
	~Circle();

	void SetPosition(float x, float y, float z);
	bool CheckCircleCollision(const Circle& other);
private:
	Vector3 m_position;
	float m_radius;
};

#endif