#ifndef AIRHOCKEYTABLE_H
#define AIRHOCKEYTABLE_H

#include "D3DUtil.h"
#include "GeometryGenerator.h"
#include "Vertex.h"
#include "Effects.h"

class Camera;
struct DirectionalLight;

class AirHockeyTable
{
public:
	AirHockeyTable();
	~AirHockeyTable();

	void Initialize(ID3D11Device* device);
	void Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);

private:
	void BuildGeometryBuffers(ID3D11Device* device);

private:
	ID3D11Buffer* m_vb;
	ID3D11Buffer* m_ib;

	ID3D11ShaderResourceView* m_baseTexSRV;	
	ID3D11ShaderResourceView* m_cusionTexSRV;
	ID3D11ShaderResourceView* m_legTexSRV;

	
	Material m_baseMaterial;
	Material m_cusionMaterial;
	Material m_legMaterial;
	
	XMFLOAT4X4 m_baseWorldMatrix;
	XMFLOAT4X4 m_cusionWorldMatrix[4];
	XMFLOAT4X4 m_legWorldMatrix[4];
	
	int m_baseVertexOffset;
	int m_cusionVertexOffset;
	int m_legVertexOffset;
	
	UINT m_baseIndexOffset;
	UINT m_cusionIndexOffset;
	UINT m_legIndexOffset;
	
	UINT m_baseIndexCount;
	UINT m_cusionIndexCount;
	UINT m_legIndexCount;
};

#endif // AirHockeyTable