#include "Circle.h"

Circle::Circle()
	: m_centerPosition(0, 0), m_radius(0)
{}

Circle::Circle(float x, float y, float radius)
	: m_centerPosition(x, y), m_radius(radius)
{}

Circle::~Circle()
{

}

void Circle::SetCenterPosition(float x, float y)
{
	m_centerPosition.x = x;
	m_centerPosition.y = y;
}

Vector2 Circle::GetCenterPosition()
{
	return m_centerPosition;
}

float Circle::GetRadius()
{
	return m_radius;
}

float Circle::GetRadiusSuqared()
{
	return m_radius * m_radius;
}