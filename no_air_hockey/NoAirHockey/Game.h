#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include "Sprite.h"
#include "Rect.h"
#include "Circle.h"
#include <ctime>
#include <cstdlib>

class Game
{
public:
	Game(HINSTANCE hAppInst, HWND hMainWnd, Vector2 centerWindow);
	~Game();

	void pause();
	void unpause();

	void update(float deltaTime);
	void updateUranus (float deltaTime);
	void updateMoon   (float deltaTime);
	void updateJupiter(float deltaTime);
	void updateMeteors(float deltaTime);
	void resetMeteors();

	void render(HDC hBackBufferDC, HDC hSpriteDC);

	bool collisionResponse(Sprite * planet);
private:
	HINSTANCE mhAppInst;
	HWND      mhMainWnd;
	Vector2   mCenterWindow;

	bool    mPaused;
	bool mShowTitle;

	const float mMaxSpeed;
	const float mUranusSpeed;

	float mRecoverTime;
	int mMeteorTimer;

	int mUScore;
	int mJScore;
	int mMXSpeed;
	int mMYSpeed;

	Sprite* mBackGround;
	Sprite* mMoon;
	Sprite* mUranus;
	Sprite* mJupiter;
	Sprite* mMeteor[30];
	Sprite* mBlackHole[2];
	Sprite* mPausedScreen;

	POINT mLastMousePos;
	POINT mCurrMousePos;

	Rect mScreenBounds;
	Rect mJupiterBounds;
	Rect mUranusBounds;
	Rect mJupiterBH;
	Rect mUranusBH;
};

#endif