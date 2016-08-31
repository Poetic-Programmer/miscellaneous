#ifndef ANCIENTRUIN_H
#define ANCIENTRUIN_H

#include "Vertex.h"
#include "Mesh.h"
#include "d3dApp.h"
#include "MathHelp.h"
#include "Geometry.h"
#include "Effects.h"
#include "States.h"
#include "Tech.h"
#include "d3dUtil.h"
#include "Camera.h"
#include "TextureMgr.h"
#include "InputLayouts.h"

const int NumColumns = 10;
class AncientRuin
{
public:
	AncientRuin();
	~AncientRuin();

	void Initialize(ID3D10Device* device);
	void Draw();
private:
	void BuildGeometryBuffers();

private:
	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	ID3D10ShaderResourceView* mFloorMapRV;
	ID3D10ShaderResourceView* mColumnMapRV;
	ID3D10ShaderResourceView* mSpecMapRV;

	MeshType2 mColumn;
	MeshType2 mFloor;
	MeshType2 mPath;

	D3DXMATRIX mColumnsWorldMatrix[10];
	D3DXMATRIX mFloorWorldMatrix;
	D3DXMATRIX mPathWorldMatrix;

	D3DXMATRIX mColumnsTextureMatrix;
	D3DXMATRIX mFloorTextureMatrix;
	D3DXMATRIX mPathTextureMatrix;

	int mColumnsVertexOffset;
	int mFloorVertexOffset;
	int mPathVertexOffset;

	int mColumnsIndexOffset;
	int mFloorIndexOffset;
	int mPathIndexOffset;

	int mColumnsIndexCount;
	int mFloorIndexCount;
	int mPathIndexCount;

	D3DXMATRIX mWVP;
};

#endif