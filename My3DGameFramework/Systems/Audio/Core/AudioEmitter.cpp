#include "AudioPch.h"
#include "Utils/MemoryUtils.h"
#include "AudioEmitter.h"

namespace MGF3D
{
	AudioEmitter::AudioEmitter(ma_engine& engine)
		: m_audioEngine(engine)
	{
		MemoryUtils::MemZero(m_sound);
	}

	AudioEmitter::~AudioEmitter()
	{
		Shutdown();
	}

	bool AudioEmitter::Init(Ptr<ma_data_source> pDataSource, Ptr<ma_sound_group> pGroup)
	{
		Shutdown();
		if (!pDataSource) return false;

		// 이미 로드된 pDataSource(Buffer 혹은 Stream)를 연결
		ma_result result = ma_sound_init_from_data_source
		(
			&m_audioEngine,
			pDataSource,
			0,
			pGroup,
			&m_sound
		);
		if (result != MA_SUCCESS)
		{
			MGF_LOG_ERROR("AudioEmitter: Failed to initialize sound from data source.");
			return false;
		}

		return true;
	}

	void AudioEmitter::Shutdown()
	{
		ma_sound_uninit(&m_sound);
	}

	void AudioEmitter::Play() { ma_sound_start(&m_sound); }

	void AudioEmitter::Stop()
	{
		ma_sound_stop(&m_sound);
		ma_sound_seek_to_pcm_frame(&m_sound, 0);
	}

	void AudioEmitter::Pause() 
	{
		ma_sound_stop(&m_sound); 
	}

	void AudioEmitter::SetLooping(bool loop)
	{
		ma_sound_set_looping(&m_sound, CommonUtils::Select(loop, MA_TRUE, MA_FALSE));
	}

	bool AudioEmitter::IsPlaying() const
	{
		return (bool)ma_sound_is_playing(&m_sound);
	}

	void AudioEmitter::SetVolume(float volume) 
	{
		ma_sound_set_volume(&m_sound, volume); 
	}

	void AudioEmitter::SetPitch(float pitch) 
	{
		ma_sound_set_pitch(&m_sound, pitch); 
	}

	void AudioEmitter::SetPosition(const vec3& pos)
	{
		ma_sound_set_position(&m_sound, pos.x, pos.y, pos.z);
	}

	void AudioEmitter::SetVelocity(const vec3& vel)
	{
		ma_sound_set_velocity(&m_sound, vel.x, vel.y, vel.z);
	}

	void AudioEmitter::SetSpatialization(bool enabled)
	{
		ma_sound_set_spatialization_enabled(&m_sound, CommonUtils::Select(enabled, MA_TRUE, MA_FALSE));
	}
}