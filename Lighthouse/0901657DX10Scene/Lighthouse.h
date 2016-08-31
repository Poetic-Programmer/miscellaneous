#ifndef Lighthouse_H
#define Lighthouse_H

#include "Geometry.h"
#include "d3dUtil.h"
#include "Vertex.h"
#include "Mesh.h"
#include "TextureMgr.h"
#include "Effects.h"
#include "Camera.h"
#include "States.h"
#include "Tech.h"
#include "InputLayouts.h"

class Lighthouse
{
public:
	Lighthouse();
	~Lighthouse();

	void Init(ID3D10Device* device);
	void SetWorldMatrix(D3DXMATRIX world);

	void Draw();
private:
	void BuildGeometryBuffers();

private:
	float mBodyHeight;
	float mBodyBaseHeight;
	float mBodyHeadHeight;
	float mTopHeight;
	float mTopBaseHeight;
	float mTopHeadHeight;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	ID3D10ShaderResourceView* mStoneMapRV;
	ID3D10ShaderResourceView* mGlassMapRV;

	MeshType2 mBody;
	MeshType2 mBodyBase;
	MeshType2 mTop;
	MeshType2 mTopBase;
	MeshType2 mTopHead;

	D3DXMATRIX mWorldMatrix;
	D3DXMATRIX mBodyOffset;
	D3DXMATRIX mHeadOffset;
	D3DXMATRIX mBaseOffset;

	int mBodyVertexOffset;
	int mBodyBaseVertexOffset;
	int mTopVertexOffset;
	int mTopBaseVertexOffset;
	int mTopHeadVertexOffset;

	int mBodyIndexOffset;
	int mBodyBaseIndexOffset;
	int mTopIndexOffset;
	int mTopBaseIndexOffset;
	int mTopHeadIndexOffset;

	int mBodyIndexCount;
	int mBodyBaseIndexCount;
	int mTopIndexCount;
	int mTopBaseIndexCount;
	int mTopHeadIndexCount;

	D3DXMATRIX mWVP;
};

#endif //Lighthouse_H