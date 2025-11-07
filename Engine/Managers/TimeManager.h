#pragma once

class TimeManager
{
	DECLARE_SINGLE(TimeManager)

public:
	void Init();
	void Update();

	float GetDeltaTime() const { return m_deltaTime; }
	float GetFPS() const	   { return m_fps; }
	double GetTime() const	   { return m_currentTime; }

private:
	void CalculateFPS();

	double m_lastTime = 0.0;
	double m_currentTime = 0.0;
	float m_deltaTime = 0.0f;

	double m_fpsLastTime = 0.0;
	int32 m_frameCount = 0;
	float m_fps = 0.0f;
};