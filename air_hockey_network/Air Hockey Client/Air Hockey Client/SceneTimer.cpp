#include <Windows.h>
#include "SceneTimer.h"

SceneTimer::SceneTimer()
:	m_secondsPerCount(0.0), m_deltaTime(-1.0), m_baseTime(0),
	m_pausedTime(0), m_prevTime(0), m_currTime(0), m_stopped(false)
{
	__int64 countPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countPerSec);
	m_secondsPerCount = 1.0 / (double)countPerSec;
}

// Return total time since Reset(), not counting stop time.
float SceneTimer::TotalTime()const
{
	if(m_stopped)
	{
		return (float)(((m_stopTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
	}
	else
	{
		return (float)(((m_currTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
	}
}

float SceneTimer::DeltaTime()const
{
	return (float)m_deltaTime;
}

void SceneTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_baseTime = currTime;
	m_prevTime = currTime;
	m_stopTime = 0;
	m_stopped = false;
}

void SceneTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if(m_stopped)
	{
		m_pausedTime += (startTime - m_stopTime);

		m_prevTime = startTime;
		m_stopTime = 0;
		m_stopped = false;
	}
}

void SceneTimer::Stop()
{
	if(!m_stopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_stopTime = currTime;
		m_stopped = true;
	}
}

void SceneTimer::Tick()
{
	if(m_stopped)
	{
		m_deltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_currTime = currTime;

	m_deltaTime = (m_currTime - m_prevTime) * m_secondsPerCount;

	m_prevTime = m_currTime;

	if(m_deltaTime < 0.0)
	{
		m_deltaTime = 0.0;
	}
}