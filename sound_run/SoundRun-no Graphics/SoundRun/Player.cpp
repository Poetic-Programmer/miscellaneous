#include "Player.h"


Player::Player()
:	
	m_frameTime(0),
	m_active(true),
	m_speed(0),
	m_boundingCircle(0, 0, 0, 0.5f)
{

}

Player::~Player()
{	
	SafeDelete(m_3DAudioSound);
}


void Player::Initialize(SoundEngine& engine)
{
	m_position.x = 0;
	m_position.y = 0.8f;
	m_position.z = 2;

	m_linearTime = 2.0f;

	SetUpSound(engine);
}

void Player::SetUpSound(SoundEngine& engine)
{
	m_3DAudioSound = new X3DAudioSound(L"Sound Effects/footsteps-4-mono.wav");
	m_3DAudioSound->Initialize(engine);
	m_3DAudioSound->SetVolume(1);
}

void Player::MoveRight()
{
	m_previousState = m_currentState;
	m_currentState = MovingRight;

	if(m_currentState != m_previousState)
	{
		m_linearCurve.Clear();
		m_linearCurve.AddElement(m_position.x, m_position.y, m_position.z, 0);
		m_linearCurve.AddElement(1.0f, m_position.y, m_position.z, m_linearTime);
		m_linearTime = 0;
	}
}

void Player::MoveLeft()
{
	m_previousState = m_currentState;
	m_currentState = MovingLeft;

	if(m_currentState != m_previousState)
	{
		m_linearCurve.Clear();
		m_linearCurve.AddElement(m_position.x, m_position.y, m_position.z, 0);
		m_linearCurve.AddElement(-1.0f, m_position.y, m_position.z, m_linearTime);
		m_linearTime = 0;
	}
}

void Player::MoveToCentre()
{
	m_previousState = m_currentState;
	m_currentState = MovingCentre;

	if(m_currentState != m_previousState)
	{
		m_linearCurve.Clear();
		m_linearCurve.AddElement(m_position.x, m_position.y, m_position.z, 0);
		m_linearCurve.AddElement(0.0f, m_position.y, m_position.z, m_linearTime);
		m_linearTime = 0;
	}
}

void Player::SetActive(bool active)
{
	m_active = active;

	if(m_active)
	{
		if(!m_3DAudioSound->IsPlaying())
			m_3DAudioSound->PlaySound();
	}
	else
	{
		if(m_3DAudioSound->IsPlaying())
			m_3DAudioSound->StopSound();
	}
}

Circle Player::GetBoundingCircle()
{
	return m_boundingCircle;
}

void Player::Update(float deltaTime, SoundEngine& engine)
{
	if(m_isMoving)
	{
		//m_position.x += m_speed * deltaTime;
	}
	m_linearTime += deltaTime;

	if(m_linearTime >= 0.5f)
		m_linearTime = 0.5f;

	m_position.x = m_linearCurve.Evaluate(m_linearTime).x;

	//m_position.x += m_speed * deltaTime;
	//m_frameTime += deltaTime;
	if(m_active)
	{
		if(!m_3DAudioSound->IsPlaying())
			m_3DAudioSound->PlaySound();
	}
	else
	{
		if(m_3DAudioSound->IsPlaying())
			m_3DAudioSound->StopSound();
	}
	
	m_3DAudioSound->SetEmittierPosition(-m_position.x, 0, 0, engine);
	m_boundingCircle.SetPosition(m_position.x, m_position.y, m_position.z);
}

void Player::MovePlayer(float speed)
{
	m_speed = speed;
	m_isMoving = true;
}

void Player::CenterPlayer()
{
	m_isMoving = false;
}