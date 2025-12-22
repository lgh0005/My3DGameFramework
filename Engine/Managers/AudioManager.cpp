#include "EnginePch.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"
#include "AudioManager.h"

bool AudioManager::Init()
{
	// 1. 엔진 초기화
	ma_result result;
	result = ma_engine_init(NULL, &m_audioEngine);
	if (result != MA_SUCCESS)
	{
		SPDLOG_ERROR("Failed to initialize audio engine.");
		return false;
	}

	// 2. 사운드 그룹 초기화
	for (int i = 0; i < (int32)AudioType::MAX; ++i)
	{
		result = ma_sound_group_init
		(
			&m_audioEngine,
			0,    // flags
			NULL, // parent group (NULL = Engine Endpoint)
			&m_groups[i]
		);

		if (result != MA_SUCCESS)
		{
			SPDLOG_ERROR("Failed to initialize sound group index: {}", i);
			return false;
		}
	}

	return true;
}

void AudioManager::Clear()
{
	ma_engine_stop(&m_audioEngine);

	for (int i = 0; i < (int)AudioType::MAX; ++i)
	{
		// 초기화된 적이 있는 그룹만 해제 (혹시 모를 안전장치)
		// miniaudio는 uninit을 여러 번 호출해도 내부적으로 체크하지 않을 수 있으니
		// Init에서 성공했을 때만 확실히 해제하는 로직이면 더 좋습니다.
		// 여기선 그냥 순서대로 다 해제합니다.
		ma_sound_group_uninit(&m_groups[i]);
	}

	ma_engine_uninit(&m_audioEngine);
}

ma_sound_group* AudioManager::GetGroup(AudioType type)
{
	return &m_groups[(int32)type];
}

void AudioManager::SetGroupVolume(AudioType type, float volume)
{
	ma_sound_group* group = GetGroup(type);
	if (group)
	{
		ma_sound_group_set_volume(group, volume);
	}
}

void AudioManager::SetMasterVolume(float volume)
{
	ma_engine_set_volume(&m_audioEngine, volume);
}

