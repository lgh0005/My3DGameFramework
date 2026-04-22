#pragma once
#include <miniaudio.h>

namespace MGF3D
{
	MGF_CLASS_PTR(AudioEmitter)
	class AudioEmitter
	{
	public:
		AudioEmitter(ma_engine& engine);
		~AudioEmitter();

	public:
		bool Init(ma_data_source* pDataSource, ma_sound_group* pGroup = nullptr);
		void Shutdown();

	public:
		void Play();
		void Stop();
		void Pause();
		void SetLooping(bool loop);
		bool IsPlaying() const;

	public:
		void SetPosition(const vec3& pos);
		void SetVelocity(const vec3& vel);
		void SetSpatialization(bool enabled);

	public:
		void SetVolume(float volume);
		void SetPitch(float pitch);

	private:
		ma_engine& m_audioEngine;
		ma_sound   m_sound { nullptr };
	};
}