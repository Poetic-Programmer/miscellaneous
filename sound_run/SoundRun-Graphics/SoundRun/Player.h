#ifndef PLAYER_H
#define PLAYER_H

#include <Windows.h>
#include <X3DAudio.h>

#include "D3DUtility.h"
#include "GeometryGenerator.h"
#include "Vertex.h"
#include "Effects.h"
#include "LinearCurve.h"
#include "XAudioSound.h"
#include "WaveManager.h"
#include "SoundEngine.h"
#include "X3DAudioSound.h"
#include "Circle.h"

class Camera;
class SoundEngine;

class Player
{
public:
	Player();
	~Player();

	void Initialize(ID3D11Device* device, SoundEngine& engine);
	void SetUpSound(SoundEngine& engine);
	void Update(float deltaTime, SoundEngine& engine);
	void Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);

	void MovePlayer(float speed);
	void CenterPlayer();

	void MoveRight();
	void MoveLeft();
	void MoveToCentre();
	void SetActive(bool active);
	Circle GetBoundingCircle();
private:
	void InitializeGeometryBuffer(ID3D11Device* device);
	//void InitEmitter();
private:
	enum PlayerState
	{
		MovingRight,
		MovingLeft,
		MovingCentre
	};

	PlayerState m_currentState;
	PlayerState m_previousState;

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	UINT m_indexCount;
	ID3D11ShaderResourceView* m_roadTexSRV;

	Material m_material;

	XMFLOAT4X4 m_world;

	XMFLOAT2   m_texOffset;
	XMFLOAT4X4 m_texTransform;

	XMFLOAT3 m_position;

	float m_speed;
	float m_frameTime;
	bool m_active;
	float m_endTime;
	float m_linearTime;

	LinearCurve m_linearCurve;

	X3DAudioSound* m_3DAudioSound;
	Circle m_boundingCircle;
	bool m_isMoving;
};

#endif