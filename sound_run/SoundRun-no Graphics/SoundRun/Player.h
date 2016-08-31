#ifndef PLAYER_H
#define PLAYER_H

#include <Windows.h>
#include <X3DAudio.h>

#include "D3DUtility.h"
#include "GeometryGenerator.h"
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

	void Initialize(SoundEngine& engine);
	void SetUpSound(SoundEngine& engine);
	void Update(float deltaTime, SoundEngine& engine);

	void MovePlayer(float speed);
	void CenterPlayer();

	void MoveRight();
	void MoveLeft();
	void MoveToCentre();
	void SetActive(bool active);
	Circle GetBoundingCircle();

private:
	enum PlayerState
	{
		MovingRight,
		MovingLeft,
		MovingCentre
	};

	PlayerState m_currentState;
	PlayerState m_previousState;

	UINT m_indexCount;

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