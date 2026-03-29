#pragma once
#include <miniaudio.h>
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
	MGF_CLASS_PTR(VolumeController)
	MGF_CLASS_PTR(AudioEar)

	MGF_CLASS_PTR(AudioEngine)
	class AudioEngine : public PoolAlloc
	{
	public:
		AudioEngine();
		~AudioEngine();

	public:
		bool Init();
		void Shutdown();

		Ptr<ma_engine> GetNativeEngine();
		Ptr<VolumeController> GetVolumeController() const;
		Ptr<AudioEar> GetEar() const;

	private:
		ma_engine m_audioEngine;

		VolumeControllerUPtr m_volumeController;
		AudioEarUPtr m_ear;
	};
}