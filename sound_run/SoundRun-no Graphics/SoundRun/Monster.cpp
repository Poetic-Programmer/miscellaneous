#include "Monster.h"

Monster::Monster()
:	
	m_frameTime(0),
	m_active(false),
	m_boundingCircle(20, 20, 20, 0.5f)
{
	m_isEating = false;
}

Monster::~Monster()
{
	SafeDelete(m_3DAudioSound);
}


void Monster::Initialize(SoundEngine& engine)
{
	SetUpSound(engine);
	SetRandomPath();
}

void Monster::SetUpSound(SoundEngine& engine)
{
	m_3DAudioSound = new X3DAudioSound(L"Sound Effects/Monster_Gigante-Doberman-1334685792-mono.wav");
	m_3DAudioSound->Initialize(engine);
	m_3DAudioSound->SetVolume(2);
}

void Monster::SetMonsterCatch(bool mCatch)
{
	m_isEating = mCatch;
}

void Monster::SetLinearRightPath()
{
	m_linearCurve.Clear();
	m_linearCurve.AddElement(MaxRight, Height, MaxDepth, 0);
	m_linearCurve.AddElement(MinRight, Height, MaxDepth, HalfActiveTime);
	m_linearCurve.AddElement(MaxRight, Height, -MaxDepth, ActiveTime);
	m_frameTime = 0;
}

void Monster::SetLinearLeftPath()
{
	m_linearCurve.Clear();
	m_linearCurve.AddElement(MaxLeft, Height, MaxDepth, 0);
	m_linearCurve.AddElement(MinLeft, Height, MaxDepth, HalfActiveTime);
	m_linearCurve.AddElement(MaxLeft, Height, -MaxDepth, ActiveTime);
	m_frameTime = 0;
}

void Monster::SetLinearMiddlePath()
{
	m_linearCurve.Clear();
	m_linearCurve.AddElement(0, Height, MaxDepth, 0);
	m_linearCurve.AddElement(0, Height, -MaxDepth, ActiveTime);
	m_frameTime = 0;
}

void Monster::SetRightToLeftPath()
{
	m_linearCurve.Clear();
	m_linearCurve.AddElement(MaxRight, Height, MaxDepth, 0);
	m_linearCurve.AddElement(MinLeft, Height, 0, HalfActiveTime);
	m_linearCurve.AddElement(MaxLeft,  Height, -MaxDepth, ActiveTime);
	m_frameTime = 0;
}

void Monster::SetLeftToRightPath()
{
	m_linearCurve.Clear();
	m_linearCurve.AddElement(MaxLeft, Height, MaxDepth, 0);
	m_linearCurve.AddElement(MinRight, Height, 0, HalfActiveTime);
	m_linearCurve.AddElement(MaxRight,  Height, -MaxDepth, ActiveTime);
	m_frameTime = 0;
}

void Monster::SetRightToCenterPath()
{
	m_linearCurve.Clear();
	m_linearCurve.AddElement(MaxRight, Height, MaxDepth, 0);
	m_linearCurve.AddElement(0, Height, 0, HalfActiveTime);
	m_linearCurve.AddElement(0,  Height, -MaxDepth, ActiveTime);
	m_frameTime = 0;
}

void Monster::SetLeftToCenterPath()
{
	m_linearCurve.Clear();
	m_linearCurve.AddElement(MaxLeft, Height, MaxDepth, 0);
	m_linearCurve.AddElement(0, Height, 0, HalfActiveTime);
	m_linearCurve.AddElement(0,  Height, -MaxDepth, ActiveTime);
	m_frameTime = 0;
}

void Monster::SetRightToLeftToCenterPath()
{
	m_linearCurve.Clear();
	m_linearCurve.AddElement(MaxRight, Height, MaxDepth, 0);
	m_linearCurve.AddElement(MaxLeft, Height, (MaxDepth * 0.5f), QuarterActiveTime);
	m_linearCurve.AddElement(0,  Height, 0, HalfActiveTime);
	m_linearCurve.AddElement(MaxRight,  Height, -(MaxDepth * 0.5f), (HalfActiveTime + QuarterActiveTime));
	m_linearCurve.AddElement(MaxLeft,  Height, -MaxDepth, ActiveTime);
	m_frameTime = 0;
}

void Monster::SetLeftToRightToCenterPath()
{
	m_linearCurve.Clear();
	m_linearCurve.AddElement(MaxLeft, Height, MaxDepth, 0);
	m_linearCurve.AddElement(MaxRight, Height, (MaxDepth * 0.5f), QuarterActiveTime);
	m_linearCurve.AddElement(0,  Height, 0, HalfActiveTime);
	m_linearCurve.AddElement(MaxLeft,  Height, -(MaxDepth * 0.5f), (HalfActiveTime + QuarterActiveTime));
	m_linearCurve.AddElement(MaxRight,  Height, -MaxDepth, ActiveTime);
	m_frameTime = 0;
}


Circle Monster::GetBoundingCircle()
{
	return m_boundingCircle;
}

void Monster::SetActive(bool active)
{
	m_active = active;

	if(m_active)
	{
		if(!m_3DAudioSound->IsPlaying())
		{
			m_3DAudioSound->PlaySound();
		}
	}
	else
	{
		if(m_3DAudioSound->IsPlaying())
			m_3DAudioSound->StopSound();
	}
}

void Monster::SetRandomPath()
{
	int randNum = 0 + rand() % ((9) + 1);

	switch(randNum)
	{
	case 1:
		SetLinearRightPath();
		break;
	case 2:
		SetLinearLeftPath();
		break;
	case 3:
		SetLinearMiddlePath();
		break;
	case 4:
		SetRightToLeftPath();
		break;
	case 5:
		SetLeftToRightPath();
		break;
	case 6:SetRightToCenterPath();
		break;
	case 7:
		SetLeftToCenterPath();
		break;
	case 8: 
		SetRightToLeftToCenterPath();
		break;
	case 9:
		SetLeftToRightToCenterPath();
		break;
	case 10:
		break;
	}
}

void Monster::Update(float deltaTime, SoundEngine& engine)
{
	srand(unsigned long(time(0)));

	m_frameTime += deltaTime;

	if(m_isEating)
	{
		if(m_3DAudioSound->IsPlaying())
			m_3DAudioSound->StopSound();
	}
	else
	{
		if(!m_3DAudioSound->IsPlaying())
		{
			m_3DAudioSound->PlaySound();
		}
	}

	m_3DAudioSound->SetEmittierPosition(-m_position.x * 1.5f, (m_position.y * 0.5f), (m_position.z * 0.08f), engine);

	Vector3 temp(0, 0, 0);
	temp = m_linearCurve.Evaluate(m_frameTime);

	m_position.x = temp.x;
	m_position.y = temp.y;
	m_position.z = temp.z;

	m_boundingCircle.SetPosition(m_position.x, m_position.y, m_position.z);

	if(m_active)
	{
		if(m_frameTime >= ActiveTime)
		{
			m_frameTime = 0;	
			SetRandomPath();
		}
	}
}

bool Monster::IsActive()
{
	return m_active;
}