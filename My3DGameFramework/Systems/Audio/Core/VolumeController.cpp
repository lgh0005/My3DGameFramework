#include "AudioPch.h"
#include "VolumeController.h"

namespace MGF3D
{
	VolumeController::VolumeController(ma_engine& engine) : m_audioEngine(engine) { }
	VolumeController::~VolumeController() = default;

	void VolumeController::SetVolume(float volume)
	{
		ma_engine_set_volume(&m_audioEngine, volume);
	}

	float VolumeController::GetVolume()
	{
		return ma_engine_get_volume(&m_audioEngine);
	}
}