#pragma once
#include <miniaudio.h>

namespace MGF3D
{
	MGF_CLASS_PTR(VolumeController)
	MGF_CLASS_PTR(AudioEar)

	MGF_CLASS_PTR(AudioEngine)
	class AudioEngine
	{
	public:
		AudioEngine();
		~AudioEngine();

	public:
		bool Init();
		void Shutdown();

		ma_engine* GetNativeEngine();
		VolumeController* GetVolumeController() const;
		AudioEar* GetEar() const;

	private:
		ma_engine m_audioEngine;
		VolumeControllerUPtr m_volumeController;
		AudioEarUPtr m_ear;
	};
}