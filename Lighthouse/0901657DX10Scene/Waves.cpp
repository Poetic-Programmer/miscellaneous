//=======================================================================================
// Waves.cpp by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================
#include "Waves.h"
#include "Vertex.h"
#include "Effects.h"
#include <algorithm>
#include <vector>

Waves::Waves()
: mNumRows(0), mNumCols(0), mNumVertices(0), mNumFaces(0), 
  mK1(0.0f), mK2(0.0f), mK3(0.0f), mTimeStep(0.0f), mSpatialStep(0.0f),
  mPrevSolution(0), mCurrSolution(0), mNormals(0), md3dDevice(0), mVB(0), mIB(0),
  mWaterMapRV(0), mSpecMapRV(0)
{
	D3DXMatrixIdentity(&mWorldMatrix);
	D3DXMatrixIdentity(&mWVP);
}

Waves::~Waves()
{
	delete[] mPrevSolution;
	delete[] mCurrSolution;
	delete[] mNormals;

	ReleaseCOM(mVB);
	ReleaseCOM(mIB);

	//ReleaseCOM(mWaterMapRV);
	//ReleaseCOM(mSpecMapRV);
}

void Waves::init(ID3D10Device* device, DWORD m, DWORD n, float dx, float dt, float speed, float damping)
{
	md3dDevice = device;

	mWaterMapRV = GetTextureMgr().createTex(L"Textures/water2a.dds");
	mSpecMapRV = GetTextureMgr().createTex(L"Textures/default_spec.dds");
	BuildGeometryBuffers(m, n, dx, dt, speed, damping);

	mWaterTexOffset = D3DXVECTOR2(0.0f, 0.0f);
}

void Waves::update(float dt)
{
	// Animate water texture as a function of time.
	mWaterTexOffset.y += 0.1f*dt;
	mWaterTexOffset.x = 0.25f*sinf(2.0f*mWaterTexOffset.y); 

	static float t = 0;

	// Accumulate time.
	t += dt;

	// Only update the simulation at the specified time step.
	if( t >= mTimeStep )
	{
		// Only update interior points; we use zero boundary conditions.
		for(DWORD i = 1; i < mNumRows-1; ++i)
		{
			for(DWORD j = 1; j < mNumCols-1; ++j)
			{
				// After this update we will be discarding the old previous
				// buffer, so overwrite that buffer with the new update.
				// Note how we can do this inplace (read/write to same element) 
				// because we won't need prev_ij again and the assignment happens last.

				// Note j indexes x and i indexes z: h(x_j, z_i, t_k)
				// Moreover, out +z axis goes "down"; this is just to 
				// keep consistent with our row indices going down.

				mPrevSolution[i*mNumCols+j].y = 
					mK1*mPrevSolution[i*mNumCols+j].y +
					mK2*mCurrSolution[i*mNumCols+j].y +
					mK3*(mCurrSolution[(i+1)*mNumCols+j].y + 
					     mCurrSolution[(i-1)*mNumCols+j].y + 
					     mCurrSolution[i*mNumCols+j+1].y + 
						 mCurrSolution[i*mNumCols+j-1].y);
			}
		}

		// We just overwrite the previous buffer with the new data, so
		// this data needs to become the current solution and the old
		// current solution becomes the new previous solution.
		std::swap(mPrevSolution, mCurrSolution);

		t = 0.0f; // reset time

		// Compute normals using finite difference scheme.
		for(DWORD i = 1; i < mNumRows-1; ++i)
		{
			for(DWORD j = 1; j < mNumCols-1; ++j)
			{
				// TODO: Double check formula.  Find two tangent vectors, one in +x and one in -z
				// direction.  Then take cross product to get normal.  Use finite difference to
				// compute tangents.  

				float l = mCurrSolution[i*mNumCols+j-1].y;
				float r = mCurrSolution[i*mNumCols+j+1].y;
				float t = mCurrSolution[(i-1)*mNumCols+j-1].y;
				float b = mCurrSolution[(i+1)*mNumCols+j+1].y;
				mNormals[i*mNumCols+j].x = -r+l;
				mNormals[i*mNumCols+j].y = 2*mSpatialStep;
				mNormals[i*mNumCols+j].z = b-t;

				D3DXVec3Normalize(&mNormals[i*mNumCols+j], &mNormals[i*mNumCols+j]);
			}
		}


		// Update the vertex buffer with the new solution.

		float width = (mNumCols-1)*mSpatialStep;
		float depth = (mNumRows-1)*mSpatialStep;

		float du = 1.0f / (mNumCols-1);
		float dv = 1.0f / (mNumRows-1);

		VertexType4* v = 0;
		HR(mVB->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&v ));

		for(DWORD i = 0; i < mNumRows; ++i)
		{
			for(DWORD j = 0; j < mNumCols; ++j)
			{
				v[i*mNumCols+j].Pos    = mCurrSolution[i*mNumCols+j];
				v[i*mNumCols+j].Normal = mNormals[i*mNumCols+j];
				v[i*mNumCols+j].TexC.x = j*du;
				v[i*mNumCols+j].TexC.y = i*dv;
			}
		}

		mVB->Unmap();
	}
}

void Waves::disturb(DWORD i, DWORD j, float magnitude)
{
	// Don't disturb boundaries.
	assert(i > 1 && i < mNumRows-2);
	assert(j > 1 && j < mNumCols-2);

	float halfMag = 0.5f*magnitude;

	mCurrSolution[i*mNumCols+j].y     += magnitude;
	mCurrSolution[i*mNumCols+j+1].y   += halfMag;
	mCurrSolution[i*mNumCols+j-1].y   += halfMag;
	mCurrSolution[(i+1)*mNumCols+j].y += halfMag;
	mCurrSolution[(i-1)*mNumCols+j].y += halfMag;
}
	
void Waves::draw()
{
	//md3dDevice->IASetInputLayout(InputLayout::PosNormalTex);
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	// set constants		

	D3DXMATRIX view = GetCamera().view();
	D3DXMATRIX proj = GetCamera().proj();
	mWVP = mWorldMatrix * view * proj;
	TextureAlpha::fxWVPVar->SetMatrix((float*)&mWVP);
	TextureAlpha::fxWorldVar->SetMatrix((float*)&mWorldMatrix);

	//mfxEyePosVar->SetRawValue(&GetCamera().position(), 0, sizeof(D3DXVECTOR3));
	//mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));

	// Scale texture coordinates by 5 units to map [0,1]-->[0,5]
	// so that the texture repeats five times in each direction.

	// Scale and translate the texture.
	D3DXMATRIX S;
	D3DXMatrixScaling(&S, 5.0f, 5.0f, 1.0f);

	D3DXMATRIX T;
	D3DXMatrixTranslation(&T, mWaterTexOffset.x, mWaterTexOffset.y, 0.0f);
	D3DXMATRIX waterTexMtx = S*T;

    D3D10_TECHNIQUE_DESC techDesc;
    TextureAlpha::Tech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		ID3D10EffectPass* pass = TextureAlpha::Tech->GetPassByIndex(p);
        TextureAlpha::Tech->GetPassByIndex( p )->Apply(0);
    
		UINT stride = sizeof(VertexType4);
		UINT offset = 0;

		//mfxTexMtxVar->SetMatrix((float*)&texScaleMatrix);
		TextureAlpha::fxTexMtxVar->SetMatrix((float*)&waterTexMtx);
		TextureAlpha::fxDiffuseMapVar->SetResource(mWaterMapRV);
		TextureAlpha::fxSpecMapVar->SetResource(mSpecMapRV);
		pass->Apply(0);

		md3dDevice->OMSetBlendState(states::TransparentBS, blendFactor, 0xffffffff);
		md3dDevice->IASetInputLayout(InputLayout::PosNormalDiffSpec);
		md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
		md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);
	}
}

void Waves::BuildGeometryBuffers(DWORD m, DWORD n, float dx, float dt, float speed, float damping)
{
	mNumRows  = m;
	mNumCols  = n;

	mNumVertices = m*n;
	mNumFaces    = (m-1)*(n-1)*2;

	mTimeStep    = dt;
	mSpatialStep = dx;

	float d = damping*dt+2.0f;
	float e = (speed*speed)*(dt*dt)/(dx*dx);
	mK1     = (damping*dt-2.0f)/ d;
	mK2     = (4.0f-8.0f*e) / d;
	mK3     = (2.0f*e) / d;

	mPrevSolution = new D3DXVECTOR3[m*n];
	mCurrSolution = new D3DXVECTOR3[m*n];
	mNormals      = new D3DXVECTOR3[m*n];

	// Generate grid vertices in system memory.

	float halfWidth = (n-1)*dx*0.5f;
	float halfDepth = (m-1)*dx*0.5f;
	for(DWORD i = 0; i < m; ++i)
	{
		float z = halfDepth - i*dx;
		for(DWORD j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx;

			mPrevSolution[i*n+j] = D3DXVECTOR3(x, 0.0f, z);
			mCurrSolution[i*n+j] = D3DXVECTOR3(x, 0.0f, z);
			mNormals[i*n+j]      = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		}
	}
 
	// Create the vertex buffer.  Note that we allocate space only, as
	// we will be updating the data every time step of the simulation.

    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(VertexType4) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    HR(md3dDevice->CreateBuffer(&vbd, 0, &mVB));


	// Create the index buffer.  The index buffer is fixed, so we only 
	// need to create and set once.

	std::vector<DWORD> indices(mNumFaces*3); // 3 indices per face

	// Iterate over each quad.
	int k = 0;
	for(DWORD i = 0; i < m-1; ++i)
	{
		for(DWORD j = 0; j < n-1; ++j)
		{
			indices[k]   = i*n+j;
			indices[k+1] = i*n+j+1;
			indices[k+2] = (i+1)*n+j;

			indices[k+3] = (i+1)*n+j;
			indices[k+4] = i*n+j+1;
			indices[k+5] = (i+1)*n+j+1;

			k += 6; // next quad
		}
	}

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}