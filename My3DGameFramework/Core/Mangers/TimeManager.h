#pragma once

class TimeManager
{
	DECLARE_SINGLE(TimeManager)
	DECLARE_NONINSTANTIABLE(TimeManager)

public:
	void Init();
	void Update();

	float GetDeltaTime() const { return m_deltaTime; }
	float GetFPS() const	   { return m_fps; }
	double GetTime() const	   { return m_currentTime; }

	float GetFixedDeltaTime() const { return m_fixedDeltaTime; }
	void SetFixedDeltaTime(float fixedDt) { m_fixedDeltaTime = fixedDt; }
	bool CheckFixedUpdate();

private:
	void CalculateFPS();

	double m_lastTime = 0.0;
	double m_currentTime = 0.0;
	float m_deltaTime = 0.0f;

	double m_fpsLastTime = 0.0;
	int32 m_frameCount = 0;
	float m_fps = 0.0f;

	float m_fixedDeltaTime = 1.0f / 60.0f; // 기본값 60hz (0.01666...)
	double m_accumulator = 0.0;            // 시간이 쌓이는 누적기
};