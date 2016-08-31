#include "LinearCurve.h"

LinearCurve::LinearCurve()
	:m_count(0)
{
}

LinearCurve::~LinearCurve()
{
}

void LinearCurve::AddElement(float x, float y, float z, float time)
{
	m_positions.push_back(Vector3(x, y, z));
	m_times.push_back(time);
	m_count++;
}

void LinearCurve::Clear()
{
	m_count = 0;
	m_positions.clear();
	m_times.clear();
}

Vector3 LinearCurve::Evaluate(float time)
{
	if(m_count < 2)
	{
		return Vector3(0, 0, 0);
	}

	if(time <= m_times[0])
	{
		return m_positions[0];
	}
	else if(time >= m_times[m_count - 1])
	{
		return m_positions[m_count - 1];
	}
	else
	{
		int i = 0;
		for(i = 0; i < m_count - 1; ++i)
		{
			if(time < m_times[i + 1])
				break;
		}

		float t0 = m_times[i];
		float t1 = m_times[i + 1];
		float u = (time - t0) / (t1 - t0);

		Vector3 temp(0, 0, 0);

		temp.x = ((1-u) * m_positions[i].x) + (u * m_positions[i + 1].x);
		temp.y = ((1-u) * m_positions[i].y) + (u * m_positions[i + 1].y);
		temp.z = ((1-u) * m_positions[i].z) + (u * m_positions[i + 1].z);

		return temp;
	}
}
