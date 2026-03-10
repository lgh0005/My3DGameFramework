#pragma once
#include "miniaudio.h"

namespace MGF3D
{
	class AudioManager
	{
		MGF_DECLARE_SINGLE(AudioManager)

	private:
		AudioManager();
		~AudioManager();

	public:
		bool Init();
		void Clear();
		ma_engine* GetEngine() { return &m_audioEngine; }

		// TODO : 그룹 : sfx, bgm 별로 음향 통제
		ma_sound_group* GetGroup(AudioType type);
		void SetGroupVolume(AudioType type, float volume);
		void SetMasterVolume(float volume);

	private:
		ma_engine m_audioEngine;
		ma_sound_group m_groups[(int32)AudioType::MAX];
	};
}
