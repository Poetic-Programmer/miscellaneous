#ifndef LINEARCURVE_H
#define LINEARCURVE_H

#include "D3DUtility.h"
#include "Vector3.h"

class LinearCurve
{
public:
	LinearCurve();
	~LinearCurve();

	void AddElement(float x, float y, float z, float time);
	void Clear();

	Vector3 Evaluate(float time);
private:
	std::vector<Vector3> m_positions;
	std::vector<float> m_times;
	int m_count;
};

#endif