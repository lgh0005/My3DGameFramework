#include "AudioPch.h"
#include "AudioEar.h"

namespace MGF3D
{
	AudioEar::AudioEar(ma_engine& engine, uint32 idx)
		: m_audioEngine(engine), m_listenerIndex(idx) { }

	AudioEar::~AudioEar() = default;

	void AudioEar::Update(const vec3& position, const vec3& forward, const vec3& up)
	{
		// 1. 위치 설정
		ma_engine_listener_set_position(&m_audioEngine, m_listenerIndex, position.x, position.y, position.z);
	
		// 2. 바라보는 방향(Forward) 설정
		ma_engine_listener_set_direction(&m_audioEngine, m_listenerIndex, forward.x, forward.y, forward.z);

		// 3. 머리 위 방향(Up) 설정 (보통 0, 1, 0)
		ma_engine_listener_set_world_up(&m_audioEngine, m_listenerIndex, up.x, up.y, up.z);
	}

	void AudioEar::SetEnabled(bool enabled)
	{
		ma_engine_listener_set_enabled(&m_audioEngine, m_listenerIndex, enabled ? MA_TRUE : MA_FALSE);
	}

	bool AudioEar::IsEnabled() const
	{
		return (bool)ma_engine_listener_is_enabled(&m_audioEngine, m_listenerIndex);
	}
}