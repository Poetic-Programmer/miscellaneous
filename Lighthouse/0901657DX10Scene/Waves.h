//=======================================================================================
// Waves.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================
#ifndef WAVES_H
#define WAVES_H

#include "d3dUtil.h"
#include "Camera.h"
#include "TextureMgr.h"
#include "States.h"
#include "Light.h"
#include "Tech.h"
#include "InputLayouts.h"

class Waves
{
public:
	Waves();
	~Waves();

 
	void init(ID3D10Device* device, DWORD m, DWORD n, float dx, float dt, float speed, float damping);
	void update(float dt);
	void disturb(DWORD i, DWORD j, float magnitude);
	void draw();

private:
	void BuildGeometryBuffers(DWORD m, DWORD n, float dx, float dt, float speed, float damping);

private:
	DWORD mNumRows;
	DWORD mNumCols;

	DWORD mNumVertices;
	DWORD mNumFaces;

	// Simulation constants we can precompute.
	float mK1;
	float mK2;
	float mK3;

	float mTimeStep;
	float mSpatialStep;

	D3DXVECTOR3* mPrevSolution;
	D3DXVECTOR3* mCurrSolution;
	D3DXVECTOR3* mNormals;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	ID3D10ShaderResourceView* mWaterMapRV;
	ID3D10ShaderResourceView* mSpecMapRV;

	D3DXMATRIX mWorldMatrix;
	D3DXMATRIX mWVP;

	// translate water tex-coords.
	D3DXVECTOR2 mWaterTexOffset;
};

#endif // WAVES_H