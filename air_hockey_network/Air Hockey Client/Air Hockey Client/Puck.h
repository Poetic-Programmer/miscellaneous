#ifndef PUCK_H
#define PUCK_H

#include "Vector3.h"
#include "Vector2.h"
#include "Circle.h"
#include "D3DUtil.h"
#include "GeometryGenerator.h"
#include "Vertex.h"
#include "Effects.h"

class Camera;
struct DirectionalLight;

class Puck
{
public:
	Puck();
	~Puck();
	void Initialize(ID3D11Device* device);
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);

	void AddForce(const Vector3 force);
	void SetForce(const Vector3 force);
	void SetPosition(const Vector3 pos);
	void SetVelocity(const Vector3 vel);
	Circle GetBounds() {return m_bounds;}
private:
	void BuildGeometryBuffers(ID3D11Device* device);

private:
	Vector3 m_position3D;
	Vector2 m_position2D;
	Vector3 m_velocity;
	Circle m_bounds;
	ID3D11Buffer* m_vb;
	ID3D11Buffer* m_ib;
	ID3D11ShaderResourceView* m_puckTexSRV;		
	Material m_puckMaterial;	
	XMFLOAT4X4 m_puckWorldMatrix;	
	int m_puckVertexOffset;	
	UINT m_puckIndexOffset;
	UINT m_puckIndexCount;
};

#endif