#ifndef SCENETIMER_H
#define SCENETIMER_H

class SceneTimer
{
public:
	SceneTimer();

	float TotalTime()const; //seconds
	float DeltaTime()const; //seconds

	void Reset();
	void Start();
	void Stop();
	void Tick();

private:
	double m_secondsPerCount;
	double m_deltaTime;

	__int64 m_baseTime;
	__int64 m_pausedTime;
	__int64 m_stopTime;
	__int64 m_prevTime;
	__int64 m_currTime;

	bool m_stopped;
};

#endif //GameTimer