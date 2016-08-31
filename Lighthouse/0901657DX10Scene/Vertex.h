#ifndef VERTEX_H
#define VERTEX_H

#include "d3dUtil.h"

struct VertexType1
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR4 Color;
};

struct VertexType2
{
	VertexType2(){}
	VertexType2(const D3DXVECTOR3& p, const D3DXVECTOR3& n, const D3DXVECTOR3& t, const D3DXVECTOR2& uv)
		: Position(p), Normal(n), TangentU(t), TexC(uv){}
	VertexType2(
		float px, float py, float pz, 
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v)
		: Position(px,py,pz), Normal(nx,ny,nz),
		  TangentU(tx, ty, tz), TexC(u,v){}

	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR3 TangentU;
	D3DXVECTOR2 TexC;
};

struct VertexType3
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR3 Normal;
	D3DXCOLOR   Diffuse;
	D3DXCOLOR   Spec; // (r, g, b, specPower);
};

struct VertexType4
{
	VertexType4(){}
	VertexType4(float x, float y, float z, 
		float nx, float ny, float nz, 
		float u, float v)
		: Pos(x,y,z), Normal(nx,ny,nz), TexC(u,v){}

	D3DXVECTOR3 Pos;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 TexC;
};
#endif