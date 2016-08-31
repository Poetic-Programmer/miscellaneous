#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include <X3DAudio.h>

#include "D3DUtility.h"
#include "Camera.h"
#include "LightHelper.h"
#include "RenderStates.h"
#include "Effects.h"
#include "XAudioController.h"
#include "SoundEngine.h"
#include "Wave.h"
#include "XAudioSound.h"
#include "WaveManager.h"
#include "X3DAudioSound.h"
#include "Player.h"
#include "Monster.h"

struct DirectionalLight;

const float GameRunTime = 30;
class Game
{
public:
	Game();

	~Game();

	bool Initialize(ID3D11Device* device);

	void Update(float deltaTime);

	void UpdateMainMenu(float deltaTime);
	void UpdateGame(float deltaTime);

	void PlayGameOver();
	void PlayGameWon();

	void Render(ID3D11DeviceContext* dc);

	void OnResize(float fov, float aspect, float near, float far);

	void CheckCollisions();

	void CheckGameOver();
	void CheckGameWon();

	void RestartGame();
	bool IsGameOver();
	void UpdateMonster(float deltaTime);
private:
	void InitX3DAudio ();
	void InitListener();
	void InitEmitter();
	void InitDSP();
	void Apply3D();

//--- Helper function to create the XASound object from MySound.wav with filter enabled.
	XAudioSound* LoadXASound ();
private:
	Camera m_camera;
	DirectionalLight m_directionLight[3];
	Monster m_monster;
	Player m_player;

	SoundEngine m_soundEngine;

	XAudioSound* m_rainSFX;
	XAudioSound* m_windSFX;
	XAudioSound* m_gameOverSFX;
	XAudioSound* m_gameEndingSound1;
	XAudioSound* m_gameEndingSound2;
	XAudioSound* m_music;

	X3DAudioSound* m_trafficSFX;

	//--- X3DAudio elements.
	X3DAUDIO_HANDLE mX3DInstance;
	X3DAUDIO_LISTENER mListener;
	X3DAUDIO_EMITTER mEmitter;
	X3DAUDIO_DSP_SETTINGS mDSPSettings; 

	float m_totalTime;
	LinearCurve m_trafficPath;
	bool m_gameOver;
};

#endif