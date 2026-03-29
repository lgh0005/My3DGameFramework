#pragma once
#include <miniaudio.h>
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
	MGF_CLASS_PTR(AudioMixer)
	class AudioMixer : public PoolAlloc
	{
	public:
		AudioMixer(ma_engine& engine);
		~AudioMixer();

	public:
		bool Init();
		void Shutdown();

		void SetVolume(float volume);
		float GetVolume() const;

		Ptr<ma_sound_group> GetNativeGroup();

	private:
		ma_engine& m_audioEngine;
		ma_sound_group m_group;
	};
}