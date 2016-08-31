#ifndef NOISE_H
#define NOISE_H

//***************************************************************************************
// 'Noise Class' by Michael Dunleavy          Last Edit: 03/05/2013
//
// This class is responsible for setting up and implementing the resources needed for
// simple noise functions.  The noise functions implemented in this class are currently
// only 2D, 3D noise functions will be added later.  
//***************************************************************************************

#include <vector>

#include "D3DUtil.h"
#include "Vector2.h"

class Noise
{
public:
	Noise();
	~Noise();
	Noise& operator =(const Noise& other);
	void Initialize();


	float PerlinNoise2D(float x, float y, int octaves);
	float RawNoise2D(float x, float y, int octaves);
private:
	Noise(const Noise& other);

	float PerlinNoise2D(float x, float y);
	float RawNoise2D(float x, float y);	

	void FillPermutationTable();
	void FillGradientValues();

	float Lerp(float t, float a, float b);
	float Curve(float t);

private:
	#define Base 0x100
	#define BaseMask 0xff

	std::vector<int> m_permutationTable;
	std::vector<Vector2> m_gradients2D;
};
#endif // Noise.h