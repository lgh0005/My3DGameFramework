#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(AudioEngine)
	MGF_CLASS_PTR(AudioMixer)

	class AudioManager
	{
		MGF_DECLARE_SINGLE(AudioManager)

	private:
		AudioManager();
		~AudioManager();

	public:
		bool Init();
		void Shutdown();

	public:
		bool CreateChannel(const SString& name);
		Ptr<AudioMixer> GetMixer(const SString& name);
		void SetChannelVolume(const SString& name, float volume);

	private:
		AudioEngineUPtr m_engine;
		SMap<SString, AudioMixerUPtr> m_channels;
	};
}
