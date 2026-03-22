#include "AudioPch.h"
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#include "AudioEngine.h"
#include "Core/VolumeController.h"
#include "Core/AudioEar.h"

namespace MGF3D
{
	AudioEngine::AudioEngine() = default;
	AudioEngine::~AudioEngine()
	{
		Shutdown();
	}

	bool AudioEngine::Init()
	{
		// 1. 네이티브 엔진 초기화 (기본 오디오 디바이스 자동 선택)
		ma_result result = ma_engine_init(nullptr, &m_audioEngine);
		if (result != MA_SUCCESS)
		{
			MGF_LOG_ERROR("AudioEngine: Failed to initialize miniaudio engine.");
			return false;
		}

		// 2. 제어기 부품들 생성
		m_volumeController = MakeUnique<VolumeController>(m_audioEngine);
		m_ear = MakeUnique<AudioEar>(m_audioEngine, 0);

		MGF_LOG_INFO("AudioEngine: System initialized successfully.");
		return true;
	}

	void AudioEngine::Shutdown()
	{
		// 1. 부품들 먼저 해제
		m_volumeController.Reset();
		m_ear.Reset();

		// 2. 네이티브 엔진 해제
		ma_engine_uninit(&m_audioEngine);

		MGF_LOG_INFO("AudioEngine: System shutdown.");
	}

	Ptr<ma_engine> AudioEngine::GetNativeEngine()
	{
		return &m_audioEngine;
	}

	Ptr<VolumeController> AudioEngine::GetVolumeController() const
	{
		return m_volumeController.Get();
	}

	Ptr<AudioEar> AudioEngine::GetEar() const
	{
		return m_ear.Get();
	}


}