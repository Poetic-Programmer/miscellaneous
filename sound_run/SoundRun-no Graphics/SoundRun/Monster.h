#ifndef IMonster_H
#define IMonster_H

#include <Windows.h>
#include <X3DAudio.h>

#include "D3DUtility.h"
#include "LinearCurve.h"
#include "XAudioSound.h"
#include "WaveManager.h"
#include "SoundEngine.h"
#include "X3DAudioSound.h"
#include "Circle.h"
#include <cstdlib>
#include <ctime>


class SoundEngine;
class Camera;
struct DirectionalLight;

const float Height = 0;

const float MaxRight = 1;
const float MaxLeft  = -1;
const float MinRight = 1;
const float MinLeft = -1;

const float MaxDepth = 100;
const float ActiveTime = 10;
const float HalfActiveTime = ActiveTime * 0.5f;
const float QuarterActiveTime = HalfActiveTime * 0.5f;


class Monster
{
public:
	Monster();
	~Monster();

	void Initialize(SoundEngine& engine);
	void SetUpSound(SoundEngine& engine);
	void Update(float deltaTime, SoundEngine& engine);

	void SetMonsterCatch(bool mCatch);

	void SetRandomPath();
	void SetActive(bool active);
	bool IsActive();
	Circle GetBoundingCircle();
private:

	void SetLinearRightPath();
	void SetLinearLeftPath();
	void SetLinearMiddlePath();
	void SetRightToLeftPath();
	void SetLeftToRightPath();
	void SetRightToCenterPath();
	void SetLeftToCenterPath();
	void SetRightToLeftToCenterPath();
	void SetLeftToRightToCenterPath();
	//void InitEmitter();
private:

	XMFLOAT3 m_position;
	float m_frameTime;
	bool m_active;
	float m_endTime;
	LinearCurve m_linearCurve;
	Circle m_boundingCircle;
	X3DAudioSound* m_3DAudioSound;
	bool m_isEating;
};

#endif