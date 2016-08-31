//***************************************************************************************
// 'MohaineDesert Class' by Michael Dunleavy          Last Edit: 03/05/2013
//
// This class is responsible for setting up the terrain initialization as well as
// providing access to the terrain class for the purposes of code readability.
//***************************************************************************************

#ifndef MOHAINEDESERT_H
#define MOHAINEDESERT_H

#include "D3DApp.h"
#include "D3DUtil.h"
#include "Noise.h"
#include "Vertex.h"
#include "Terrain.h"

class Camera;
struct DirectionalLights;

class MohaineDesert
{
public:
	MohaineDesert();
	~MohaineDesert();

	void Initialize(ID3D11Device* device, ID3D11DeviceContext* dc);
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* dc, Camera& cam, DirectionalLight lights[3]);

	float GetHeight(float x, float y);
private:
	Terrain m_terrain;
};
#endif // MohainDesert.h