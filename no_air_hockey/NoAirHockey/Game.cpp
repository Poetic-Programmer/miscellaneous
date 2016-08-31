#include "Game.h"
#include<cstdio>
#include "resource.h"

Game::Game(HINSTANCE hAppInst, HWND hMainWnd, Vector2 centerWnd)
: mMaxSpeed(300.0f), mUranusSpeed(200.0f)
{
	//seed random number generator
	srand((unsigned)(time(0)));

	mhAppInst = hAppInst;
	mhMainWnd = hMainWnd;
	mCenterWindow = centerWnd;

	mPaused = true; //Game starts paused
	mShowTitle = true;
	mMeteorTimer = 0; 

	mUScore = 0;
	mJScore = 0;
	mMXSpeed = 10;
	mMYSpeed = -10;
	/////////////////////////////////////////////////////////////////////////////
	//Set up Objects
	/////////////////////////////////////////////////////////////////////////////
	Circle bc;
	Vector2 pos(500.0f, 500.0f);
	Vector2 vel(0.0f, 0.0f);

	mBackGround = new Sprite(hAppInst, IDB_BITMAP11, IDB_BITMAP12,  bc, pos, vel);
	mPausedScreen = new Sprite(hAppInst, IDB_BITMAP13, IDB_BITMAP12,  bc, pos, vel);

	bc.c = pos;
	bc.r = 35.0f;
	mMoon  = new Sprite(hAppInst, IDB_BITMAP9, IDB_BITMAP8,  bc, pos, vel);

	pos.mX = 700;
	pos.mY = 200;

	bc.c = pos;
	bc.r = 60.0f;
	mUranus     = new Sprite(hAppInst, IDB_BITMAP15, IDB_BITMAP14,  bc, pos, vel);

	pos.mX = 100;
	pos.mY = 100;
	bc.c = pos;
	bc.r = 85.0f;
	mJupiter    = new Sprite(hAppInst, IDB_BITMAP5, IDB_BITMAP4, bc, pos, vel);

	pos.mX = 100.0f;
	pos.mY = -100.0f;
	bc.c.mX = pos.mX;
	bc.c.mY = pos.mY - 20;
	bc.r = 20.0f;
	for(int i = 0; i < 30; ++i)
		mMeteor[i] = new Sprite(hAppInst, IDB_BITMAP7, IDB_BITMAP6, bc, pos, vel);

	resetMeteors();
	//////////////////////////////////////////////////////////////////////////
	//BLACK HOLE:  Goals for the game
	//////////////////////////////////////////////////////////////////////////
	
	pos.mX = 100.0f;
	pos.mY = 100.0f;
	bc.c.mX = pos.mX;
	bc.c.mY = pos.mY - 20;
	bc.r = 40.0f;
	mBlackHole[0] = new Sprite(hAppInst, IDB_BITMAP3, IDB_BITMAP2, bc, pos, vel);

	pos.mX = 850.0f;
	pos.mY = 750.0f;
	bc.c.mX = pos.mX - 10;
	bc.c.mY = pos.mY - 40;
	bc.r = 40.0f;
	mBlackHole[1] = new Sprite(hAppInst, IDB_BITMAP3, IDB_BITMAP2, bc, pos, vel);
	

	///////////////////////////////////////////////////////////////////////////////
	//Set up boundarys of the Game
	///////////////////////////////////////////////////////////////////////////////
	mJupiterBounds = Rect(  100,   0, 500, 800);
	mUranusBounds  = Rect(500,   0, 1000, 640);
	mScreenBounds  = Rect(  0,   0, 1000, 750);
	mJupiterBH     = Rect(  0, 0,  150, 150);
	mUranusBH      = Rect(850, 650, 1000, 800);
}

Game::~Game()
{
	delete mBackGround;
	delete mPausedScreen;
	delete mUranus;
	delete mMoon;
	delete mJupiter;
	for(int i = 0; i < 30; ++i)
		delete mMeteor[i];
	for(int i = 0; i < 2; ++i)
		delete mBlackHole[i];
}

void Game::pause()
{
	mPaused = true;
	mShowTitle = true;

	ReleaseCapture();

	ShowCursor(true);
}

void Game::unpause()
{
	POINT p = mJupiter->mPosition;
	ClientToScreen(mhMainWnd, &p);

	SetCursorPos(p.x, p.y);
	GetCursorPos(&mLastMousePos);

	mPaused = false;
	mShowTitle = false;

	SetCapture(mhMainWnd);

	ShowCursor(false);
}

void Game::render(HDC hBackBufferDC, HDC hSpriteDC)
{
	if(!mShowTitle)
	{
		mBackGround->render(hBackBufferDC, hSpriteDC);
		for(int i = 0; i < 2; ++i)
			mBlackHole[i]->render(hBackBufferDC, hSpriteDC);
		for(int i = 0; i < 30; ++i)
			mMeteor[i]->render(hBackBufferDC, hSpriteDC);
		mUranus->render(hBackBufferDC, hSpriteDC);
		mMoon->render(hBackBufferDC, hSpriteDC);
		mJupiter->render(hBackBufferDC, hSpriteDC);

		char score[20];
		sprintf_s(score, "Jupiter: %d", mJScore); 
		SetBkMode(hBackBufferDC, TRANSPARENT);
		
		TextOut(hBackBufferDC, 10, 10, score, (int)strlen(score));
		sprintf_s(score, "Uranus: %d", mUScore);
		TextOut(hBackBufferDC, 700, 700, score, (int)strlen(score));
	}
	else
	{
		mPausedScreen->render(hBackBufferDC, hSpriteDC);
	}
}

void Game::updateJupiter(float deltaTime)
{
	GetCursorPos(&mCurrMousePos);

	int dx = mCurrMousePos.x - mLastMousePos.x;
	int dy = mCurrMousePos.y - mLastMousePos.y;

	Vector2 dp((float)dx, (float)dy);

	mJupiter->mVelocity = dp / deltaTime;

	mJupiter->update(deltaTime);

	mJupiterBounds.forceInside(mJupiter->mBoundingCircle);
	mJupiter->mPosition = mJupiter->mBoundingCircle.c;

	mLastMousePos = mJupiter->mPosition;

	ClientToScreen(mhMainWnd, &mLastMousePos);
	SetCursorPos(mLastMousePos.x, mLastMousePos.y);
}

void Game::update(float deltaTime)
{
	if(!mPaused)
	{
		updateJupiter(deltaTime);
		updateUranus(deltaTime);
		updateMoon(deltaTime);
		updateMeteors(deltaTime);

		if(mRecoverTime > 0.0f)
			mRecoverTime -= deltaTime;
	
	}

}

bool Game::collisionResponse(Sprite * planet)
{
	Vector2 normal;
	/////////////////////////////////////////////
	//check moon has hit your character or AI
	/////////////////////////////////////////////
	if(planet->mBoundingCircle.hits(mMoon->mBoundingCircle, normal))
	{
		mMoon->mPosition = mMoon->mBoundingCircle.c;

		//planets velocity relative to meteor
		Vector2 relVel = planet->mVelocity - mMoon->mVelocity;

		//
		float impulseMag = relVel.dot(normal);

		if(impulseMag >= 0.0f)
		{
			Vector2 impulse = impulseMag * normal;

			mMoon->mVelocity  += 2.0f * impulse;

			return true;
		}
	}
	
	for(int i = 0; i < 30; ++i)
	{
		if(mMeteor[i]->mBoundingCircle.hits(mMoon->mBoundingCircle, normal))
		{
			mMoon->mPosition = mMoon->mBoundingCircle.c;

			//planets velocity relative to meteor
			Vector2 relVel = mMeteor[i]->mVelocity - mMoon->mVelocity;

			//
			float impulseMag = relVel.dot(normal);

			if(impulseMag >= 0.0f)
			{
				Vector2 impulse = impulseMag * normal;

				mMoon->mVelocity  += 2.0f * impulse;

				return true;
			}
		}
	}
	
	return false;
}

void Game::updateMoon(float deltaTime)
{
	collisionResponse(mJupiter);
	//collisionResponse(mMeteor);

	if(collisionResponse(mUranus))
		mRecoverTime = 0.1f;

	if( mMoon->mVelocity.length() >= mMaxSpeed )
		mMoon->mVelocity.normalize() *= mMaxSpeed;

	Circle meteorCircle = mMoon->mBoundingCircle;
	if( meteorCircle.c.mX - meteorCircle.r < mScreenBounds.mMinPoint.mX )
		mMoon->mVelocity.mX *= -1.0f;
	if( meteorCircle.c.mX + meteorCircle.r > mScreenBounds.mMaxPoint.mX )
		mMoon->mVelocity.mX *= -1.0f;
	if( meteorCircle.c.mY - meteorCircle.r < mScreenBounds.mMinPoint.mY )
		mMoon->mVelocity.mY *= -1.0f;
	if( meteorCircle.c.mY + meteorCircle.r > mScreenBounds.mMaxPoint.mY )
		mMoon->mVelocity.mY *= -1.0f;

	// Make sure puck stays inbounds of the gameboard.
	mScreenBounds.forceInside(mMoon->mBoundingCircle);
	mMoon->mPosition = mMoon->mBoundingCircle.c;
	mMoon->update(deltaTime);

	//SCORE
	if(mJupiterBH.isPointInside(mMoon->mPosition))
	{
		mUScore++;
		mMoon->mPosition = Vector2(500, 400);
		mMoon->mVelocity = Vector2(0.0f, 0.0f);
		mMoon->mBoundingCircle.c = Vector2(500, 400);
		pause();
	}

	if(mUranusBH.isPointInside(mMoon->mPosition))
	{
		mJScore++;
		mMoon->mPosition = Vector2(500, 400);
		mMoon->mVelocity = Vector2(0.0f, 0.0f);
		mMoon->mBoundingCircle.c = Vector2(500, 400);
		pause();
	}
}

void Game::updateUranus(float deltaTime)
{
	if( mRecoverTime <= 0.0f )
	{
		// Is the puck in red's boundary? If yes, then
		// move the red paddle directly toward the puck.
		if( mUranusBounds.isPointInside(mMoon->mPosition) )
		{
		// Vector directed from paddle to puck.
			Vector2 uranusVelocity = mMoon->mPosition - mUranus->mPosition;
			uranusVelocity.normalize();
			uranusVelocity *= mUranusSpeed;
			mUranus->mVelocity = uranusVelocity;
		}
		// If no, then move the red paddle to the point (700, 200).
		else
		{
			Vector2 uranusVelocity = Vector2(700, 600) - mUranus->mPosition;
			if(uranusVelocity.length() > 5.0f)
			{
				uranusVelocity.normalize();
				uranusVelocity *= mUranusSpeed;
				mUranus->mVelocity = uranusVelocity;
			}
			// Within 5 units--close enough.
			else
				mUranus->mVelocity = Vector2(0.0f, 0.0f);
		}
	// Update the red paddle's position.
	mUranus->update(deltaTime);

	// Make sure the red paddle stays inbounds.
	mUranusBounds.forceInside(mUranus->mBoundingCircle);
	mUranus->mPosition = mUranus->mBoundingCircle.c;
	}
}

void Game::resetMeteors()
{
	Circle bc;
	Vector2 pos(500.0f, 500.0f);
	Vector2 vel(0.0f, 0.0f);

	for(int i = 0; i < 8; ++i)
		delete mMeteor[i];

	
	for(int  i = 0; i < 30; ++i)
	{
		mMXSpeed = -400 + rand() % ((400 + 1) + 400);
		mMYSpeed = -200 + rand() % ((200 + 1) + 200);
		pos.mX = (float)1 + rand() % ((1200 + 1) - 1);
		pos.mY = (float)-300 + rand() % ((-100 + 1) + 300);
		vel.mX = (float)mMXSpeed;
		vel.mY = (float)mMYSpeed;
		bc.c = pos;
		bc.r = 35.0f;
		mMeteor[i]  = new Sprite(mhAppInst, IDB_BITMAP7, IDB_BITMAP6,  bc, pos, vel);
	}
}

void Game::updateMeteors(float deltaTime)
{
	for(int i = 0; i < 30; ++i)
	{
		mMeteor[i]->update(deltaTime);

		if( mMeteor[i]->mVelocity.length() >= mMaxSpeed )
			mMeteor[i]->mVelocity.normalize() *= mMaxSpeed;

		if(mMeteor[i]->mPosition.mX >= 1000)
			mMeteor[i]->mPosition.mX = 1;
		if(mMeteor[i]->mPosition.mX <= 0)
			mMeteor[i]->mPosition.mX = 999;

		if(mMeteor[i]->mPosition.mY >= 800)
			mMeteor[i]->mPosition.mY = 1;
		if(mMeteor[i]->mPosition.mY <= 0)
			mMeteor[i]->mPosition.mY = 799;
	}


}