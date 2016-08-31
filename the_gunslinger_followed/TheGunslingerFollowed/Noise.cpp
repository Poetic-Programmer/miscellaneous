#include "Noise.h"
#include "MathHelper.h"

Noise::Noise()
{
	m_permutationTable.resize(Base + Base + 2);
	m_gradients2D.resize(Base + Base + 2);
}

Noise::~Noise()
{

}

Noise& Noise::operator=(const Noise& other)
{
	if(this == &other)
		return *this;

	this->m_permutationTable = other.m_permutationTable;
	this->m_gradients2D      = other.m_gradients2D;

	return *this;
}

void Noise::Initialize()
{
	FillPermutationTable();
	FillGradientValues();
}

float Noise::PerlinNoise2D(float x, float y, int octaves)
{
	float frequency = 0.05f / octaves;
	float amplitude = 1.0f;
	float output = 0.0f;

	for (int i = 1; i < octaves + 1; ++i)
	{
		output += amplitude * PerlinNoise2D(x * frequency, y * frequency);	
		amplitude = 0.5f/float(i);
		frequency *= 2.0f;
	}

	return output;
}

float Noise::RawNoise2D(float x, float y, int octaves)
{
	float output = 0.0f;
	float frequency = 0.05f;
	float amplitude = 1.0f;

	
	for(int i = 1; i < octaves ; ++i)
	{
		output += amplitude * RawNoise2D(x * frequency, y  * frequency);
		
		amplitude *= 0.5f;
		frequency *= 1.5f;
	}

	return output;
}

float Noise::PerlinNoise2D(float x, float y)
{
	int x0, x1, y0, y1, b00, b01, b10, b11;
	float rx0, rx1, ry0, ry1, q1, q2, sx, sy, a, b, u, v;
	register int i, j;

	// Find the enclosing basis points around our input x value
	x0 = ((int)x) & BaseMask;
	x1 = (x0+1) & BaseMask;

	// Calculate the distance to each basis point along x
	rx0 = x - (int)x;
	rx1 = rx0 - 1.0f;

	// Find the enclosing basis points around our input y value
	y0 = ((int)y) & BaseMask;
	y1 = (y0+1) & BaseMask;

	// Calculate the distance to each basis point along y
	ry0 = y - (int)y;
	ry1 = ry0 - 1.0f;

	// Permute the gradient indices using one permutation for
	// each input axis.
	i = m_permutationTable[x0];
	j = m_permutationTable[x1];

	b00 = m_permutationTable[i + y0];
	b10 = m_permutationTable[j + y0];
	b01 = m_permutationTable[i + y1];
	b11 = m_permutationTable[j + y1];

	// Get a smoothed interpolation input
	sx = Curve(rx0);
	sy = Curve(ry0);

	// Perform dot product with the gradient vectors to find our two input
	// values for the interpolation
	q1 = m_gradients2D[b00].x; q2 = m_gradients2D[b00].y; u = (rx0 * q1 + ry0 * q2);
	q1 = m_gradients2D[b10].x; q2 = m_gradients2D[b10].y; v = (rx1 * q1 + ry0 * q2);
	a = Lerp(sx, u, v);

	q1 = m_gradients2D[b01].x; q2 = m_gradients2D[b01].y; u = (rx0 * q1 + ry1 * q2);
	q1 = m_gradients2D[b11].x; q2 = m_gradients2D[b11].y; v = (rx1 * q1 + ry1 * q2);
	b = Lerp(sx, u, v);

	// Return the interpolated value
	return(Lerp(sy, a, b));
}

float Noise::RawNoise2D(float x, float y)
{
    // Noise contributions from the three corners
    float n0, n1, n2;

    // Skew the input space to determine which simplex cell we're in
    float F2 = 0.5f * (sqrtf(3.0f) - 1.0f);
    // Hairy factor for 2D
    float s = (x + y) * F2;
    int i = (int)(floorf( x + s ));
    int j = (int)(floorf( y + s ));

    float G2 = (3.0f - sqrtf(3.0f)) / 6.0f;
    float t = (i + j) * G2;
    // Unskew the cell origin back to (x,y) space
    float X0 = i - t;
    float Y0 = j - t;
    // The x,y distances from the cell origin
    float x0 = x-X0;
    float y0 = y-Y0;

    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
    if(x0 > y0)
	{
		// lower triangle, XY order: (0,0)->(1,0)->(1,1)
		i1 = 1; 
		j1 = 0;
	} 
    else
	{
		// upper triangle, YX order: (0,0)->(0,1)->(1,1)
		i1 = 0; 
		j1 = 1;
	} 

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6
    float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
    float y1 = y0 - j1 + G2;
    float x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
    float y2 = y0 - 1.0f + 2.0f * G2;

    // Work out the hashed gradient indices of the three simplex corners
    int ii = i & 255;
    int jj = j & 255;
    int gi0 = m_permutationTable[ii + m_permutationTable[jj]] % 12;
    int gi1 = m_permutationTable[ii + i1 + m_permutationTable[jj + j1]] % 12;
    int gi2 = m_permutationTable[ii + 1 + m_permutationTable[jj + 1]] % 12;

    // Calculate the contribution from the three corners
    float t0 = 0.5f - x0 * x0 - y0 * y0;
    if(t0<0) n0 = 0.0;
    else 
	{
        t0 *= t0;
		n0 = t0 * t0 * MathHelper::DotProduct(m_gradients2D[gi0], x0, y0);
    }

    float t1 = 0.5f - x1 * x1 - y1 * y1;
    if(t1 < 0) 
	{
		n1 = 0.0;
	}

    else 
	{
        t1 *= t1;
        n1 = t1 * t1 * MathHelper::DotProduct(m_gradients2D[gi1], x1, y1);
    }

    float t2 = 0.5f - x2 * x2 - y2 * y2;

    if(t2 < 0) 
	{
		n2 = 0.0;
	}
    else 
	{
        t2 *= t2;
        n2 = t2 * t2 * MathHelper::DotProduct(m_gradients2D[gi2], x2, y2);
    }

    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 70.0f * (n0 + n1 + n2);
}

void Noise::FillPermutationTable()
{
	std::srand(0x01); // seed

	int i, j, k; //Counters

	// Fill with numbers between 0 and 255
	for(i = 0; i < Base; ++i)
	{
		m_permutationTable[i] = i;
	}

	// Randomly mix the numbers
	while(--i)
	{
		k = m_permutationTable[i];
		m_permutationTable[i] = m_permutationTable[j = rand() % Base];
		m_permutationTable[j] = k;
	}

	// Create duplicate
	for(i = 0; i < Base + 2; ++i)
	{
		m_permutationTable[Base + i] = m_permutationTable[i];
	}
}

void Noise::FillGradientValues()
{
	std::srand(0x01); // seed

	int i; //Counter

	// Create random gradient normals
	for(i = 0; i < Base; ++i)
	{
		m_gradients2D[i].x = (float)((rand() % (Base + Base)) - Base) / Base;
		m_gradients2D[i].y = (float)((rand() % (Base + Base)) - Base) / Base;
		m_gradients2D[i].Normalize();
	}

	// Create duplicate
	for(i = 0; i < Base + 2; ++i)
	{
		m_gradients2D[i + Base] = m_gradients2D[i];
	}
}

float Noise::Lerp(float t, float a, float b)
{
	return(a + t * (b - a));
}

float Noise::Curve(float t)
{
	return(t * t * (3.0f - 2.0f * t));
}