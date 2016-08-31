#include "Circle.h"

Circle::Circle(float x, float y, float z, float radius)
	: m_position(x, y, z), m_radius(radius)
{
}

Circle::~Circle()
{
}

void Circle::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

bool Circle::CheckCircleCollision(const Circle& other)
{
	float distSquared = (other.m_position.x - m_position.x) * (other.m_position.x - m_position.x) + 
		(other.m_position.y - m_position.y) * (other.m_position.y - m_position.y) + 
		(other.m_position.z - m_position.z) * (other.m_position.z - m_position.z);

	float radiusSquared = (m_radius + other.m_radius) * (m_radius + other.m_radius);

	if( distSquared < radiusSquared)
		return true;
	else
		return false;
}