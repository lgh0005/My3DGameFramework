#pragma once
#include <miniaudio.h>
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
	MGF_CLASS_PTR(VolumeController)
	class VolumeController : public PoolAlloc
	{
	public:
		VolumeController(ma_engine& engine);
		virtual ~VolumeController() override;

	public:
		void SetVolume(float volume);
		float GetVolume();

	private:
		ma_engine& m_audioEngine;
	};
}