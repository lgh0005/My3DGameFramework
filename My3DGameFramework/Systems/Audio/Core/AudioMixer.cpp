#include "AudioPch.h"
#include "AudioMixer.h"

namespace MGF3D
{
	AudioMixer::AudioMixer(ma_engine& engine) : m_audioEngine(engine) { }
	AudioMixer::~AudioMixer() = default;

	bool AudioMixer::Init()
	{
		ma_result result = ma_sound_group_init(&m_audioEngine, 0, nullptr, &m_group);
		if (result != MA_SUCCESS)
		{
			MGF_LOG_ERROR("AudioMixer: Failed to initialize sound group.");
			return false;
		}

		return true;
	}

	void AudioMixer::Shutdown()
	{
		ma_sound_group_uninit(&m_group);
	}

	void AudioMixer::SetVolume(float volume)
	{
		ma_sound_group_set_volume(&m_group, volume);
	}

	float AudioMixer::GetVolume() const
	{
		return ma_sound_group_get_volume(&m_group);
	}

	ma_sound_group* AudioMixer::GetNativeGroup()
	{
		return &m_group;
	}
}