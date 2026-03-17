#include "AudioPch.h"
#include "AudioModule.h"
#include "Managers/AudioManager.h"
#include "Core/AudioMixer.h"
#include "Core/AudioEngine.h"

namespace MGF3D
{
	AudioModule::AudioModule() = default;
	AudioModule::~AudioModule() = default;

	bool AudioModule::OnModuleInit()
	{
		MGF_LOG_INFO("[AudioModule] Booting up Audio Subsystems...");
		MGF_INIT_SYS(MGF_AUDIO.Init(), "AudioManager");
		MGF_LOG_INFO("[AudioModule] Audio Subsystems initialized successfully.");
		return true;
	}

	void AudioModule::OnShutDown()
	{
		MGF_LOG_INFO("[AudioModule] Shutting down Audio Subsystems...");
		MGF_SHUTDOWN_SYS(MGF_AUDIO.Shutdown(), "AudioManager");
		MGF_LOG_INFO("[AudioModule] Audio Subsystems shutdown completed.");
	}
}