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
		bool CreateChannel(const String& name);
		AudioMixer* GetMixer(const String& name);
		void SetChannelVolume(const String& name, float volume);

	private:
		AudioEngineUPtr m_engine;
		HashMap<StringHash, AudioMixerUPtr> m_channels;
	};
}
