#pragma once
#include <miniaudio.h>

namespace MGF3D
{
	MGF_CLASS_PTR(VolumeController)
	class VolumeController
	{
	public:
		VolumeController(ma_engine& engine);
		~VolumeController();

	public:
		void SetVolume(float volume);
		float GetVolume();

	private:
		ma_engine& m_audioEngine;
	};
}