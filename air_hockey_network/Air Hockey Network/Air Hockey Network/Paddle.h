#ifndef PADDLE_H
#define PADDLE_H

#include "Vector3.h"
#include "Vector2.h"
#include "D3DUtil.h"
#include "GeometryGenerator.h"
#include "Vertex.h"
#include "Effects.h"

class Camera;
class DirectionLight;

class Paddle
{
public:
	Paddle();
	~Paddle();
	void Initialize(ID3D11Device* device);
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);

	void SetPosition(const Vector3& vec);
private:
	void BuildGeometryBuffers(ID3D11Device* device);

private:
	Vector3 m_position3D;
	Vector2 m_position2D;

	ID3D11Buffer* m_vb;
	ID3D11Buffer* m_ib;
	ID3D11ShaderResourceView* m_paddleTexSRV;		
	Material m_paddleMaterial;	
	XMFLOAT4X4 m_paddleWorldMatrix;	

	int m_paddleVertexOffset;
	int m_handleVertexOffset;

	UINT m_paddleIndexOffset;
	UINT m_handleIndexOffset;

	UINT m_paddleIndexCount;
	UINT m_handleIndexCount;
};

#endif // Paddle.h