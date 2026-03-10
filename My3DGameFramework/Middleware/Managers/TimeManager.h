#pragma once

namespace MGF3D
{
	class TimeManager
	{
		MGF_DECLARE_SINGLE(TimeManager)

	public:
		TimeManager();
		~TimeManager();

	public:
		bool Init();
		void Shutdown();
		void Update();

		float GetDeltaTime() const { return m_deltaTime; }
		float GetFPS() const { return m_fps; }
		double GetTime() const { return m_currentTime; }

		float GetFixedDeltaTime() const { return m_fixedDeltaTime; }
		void SetFixedDeltaTime(float fixedDt) { m_fixedDeltaTime = fixedDt; }
		bool CheckFixedUpdate();

		float GetMaxDeltaTime() const { return m_maxDeltaTime; }
		void SetMaxDeltaTime(float maxDt) { m_maxDeltaTime = maxDt; }

	private:
		void CalculateFPS();

		double m_lastTime		{ 0.0 };
		double m_currentTime	{ 0.0 };
		float m_deltaTime		{ 0.0 };

		double m_fpsLastTime	{ 0.0 };
		int32 m_frameCount		{ 0 };
		float m_fps				{ 0.0 };

		float m_fixedDeltaTime	{ 1.0f / 60.0f };	// 기본값 60hz (0.01666...)
		float m_maxDeltaTime	{ 0.25f };          // 밀린 연산 방어선 (기본 0.25초); 0.25초 / 0.016초(60Hz) = 15
		double m_accumulator	{ 0.0 };            // 시간이 쌓이는 누적기
	};
}