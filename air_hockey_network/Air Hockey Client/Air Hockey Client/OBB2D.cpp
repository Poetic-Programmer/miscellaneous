#include "OBB2D.h"

OBB2D::OBB2D(Vector2 position, Vector2 xAxis, Vector2 yAxis, float width, float height)
{
	m_centerPosition = Vector2(position);
		
	m_axis[0] = Vector2(xAxis);
	m_axis[1] = Vector2(yAxis);
		
	m_halfValues[0] = width * 0.5f;
	m_halfValues[1] = height * 0.5f;
}

OBB2D::~OBB2D()
{

}

Vector2 OBB2D::ClosestPoint(Vector2 point)
{
	Vector2 temp;
	Vector2 result;
	temp.x = point.x - m_centerPosition.x;
	temp.y = point.y - m_centerPosition.y;
		
	float distance = 0;
	float prevDistance = 0;
	//Loop through each axis
	for(int i = 0; i < 2; ++i)
	{
		distance = temp.Dot(m_axis[i]);
			
		if(prevDistance < distance)
			m_axisIndex = 0;
		else
			m_axisIndex = 1;
			
		if(distance > m_halfValues[i]) distance = m_halfValues[i];
		if(distance < -m_halfValues[i]) distance = -m_halfValues[i];

		//result.add(m_axis[i].mul(distance));
		result.x += m_axis[i].x * distance;
		result.y += m_axis[i].y * distance;
		
		prevDistance = distance;
	}
	//result.x += m_centerPosition.x;
	//result.y += m_centerPosition.y;
		
	return result;
}

void OBB2D::SetPosition(float x, float y)
{
	m_centerPosition.x = x;
	m_centerPosition.y = y;
}

void OBB2D::SetAngle(float angle)
{
	Vector2 xAxisCpy = Vector2();

	xAxisCpy.x = m_axis[0].x;
	xAxisCpy.y = m_axis[0].y;
		
	//45 * MathHelper.DEGREES_TO_RADIANS;
	m_axis[0].x = (float) (MathHelper::X_Axis_2D.x * cosf(angle) + MathHelper::X_Axis_2D.y * sinf(angle));
	m_axis[0].y = (float) (-MathHelper::X_Axis_2D.x * sinf(angle) + MathHelper::X_Axis_2D.y * cosf(angle));
	m_axis[0].Normalize();
		
	m_axis[1] = xAxisCpy.Perp();
}