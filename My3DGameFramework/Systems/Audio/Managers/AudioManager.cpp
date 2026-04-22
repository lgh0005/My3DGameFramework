#include "AudioPch.h"
#include "AudioManager.h"
#include "Core/AudioEngine.h"
#include "Core/AudioMixer.h"

namespace MGF3D
{
	AudioManager::AudioManager() = default;
	AudioManager::~AudioManager() = default;

	bool AudioManager::Init()
	{
		// 1. 오디오 엔진(miniaudio) 생성 및 초기화
		m_engine = MakeUnique<AudioEngine>();
		if (!m_engine->Init())
		{
			MGF_LOG_ERROR("AudioManager: Failed to initialize AudioEngine.");
			return false;
		}

		// 2. 기본 채널들 생성 (Master, BGM, SFX 정도는 기본으로 지원)
		CreateChannel("Master");
		CreateChannel("BGM");
		CreateChannel("SFX");

		MGF_LOG_INFO("AudioManager: System initialized with dynamic channels.");
		return true;
	}

	void AudioManager::Shutdown()
	{
		// 1. 모든 채널(Mixer) 해제
		for (auto& pair : m_channels)
		{
			if (pair.second)
				pair.second->Shutdown();
		}
		m_channels.clear();

		// 2. 엔진 종료
		if (m_engine) 
			m_engine->Shutdown();

		MGF_LOG_INFO("AudioManager: Dynamic system shutdown complete.");
	}

	bool AudioManager::CreateChannel(const String& name)
	{
		// 이름 해시값 추출
		StringHash nameHash = StringHash(name);

		// 이미 존재하는 채널인지 확인
		if (m_channels.find(nameHash) != m_channels.end())
		{
			MGF_LOG_WARN("AudioManager: Channel '{}' already exists.", name.c_str());
			return false;
		}

		// 새로운 믹서 생성 (엔진의 네이티브 포인터 전달)
		auto newMixer = MakeUnique<AudioMixer>(*m_engine->GetNativeEngine());
		if (!newMixer->Init())
		{
			MGF_LOG_ERROR("AudioManager: Failed to init Mixer for channel '{}'", name.c_str());
			return false;
		}

		// 3. 기본 볼륨 초기화
		newMixer->SetVolume(1.0f);

		// 4. 맵에 등록 (소유권 이전)
		bool result = m_channels.insert({nameHash, std::move(newMixer)}).second;

		if (result)
			MGF_LOG_INFO("AudioManager: New channel '{}' created with default volume (100%).", name.c_str());
		
		return result;
	}

	AudioMixer* AudioManager::GetMixer(const String& name)
	{
		auto it = m_channels.find(StringHash(name));
		if (it != m_channels.end()) return it->second.get();
		return nullptr;
	}

	void AudioManager::SetChannelVolume(const String& name, float volume)
	{
		auto pMixer = GetMixer(name);
		if (pMixer) pMixer->SetVolume(volume);
	}
}