#ifndef SKYDOME_H
#define SKYDOME_H

#include "d3dUtil.h"
#include "Tech.h"
#include "TextureMgr.h"

class Sky
{
public:
	Sky();
	~Sky();

	void init(ID3D10Device* device, float radius);

	void draw();

private:
	Sky(const Sky& rhs);
	Sky& operator=(const Sky& rhs);

private:
	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	ID3D10ShaderResourceView* mCubeMapRV;

	UINT mNumIndices;
 
	ID3D10EffectShaderResourceVariable* mfxCubeMapVar;
};

#endif // SKY_H