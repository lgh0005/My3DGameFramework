#pragma once
#include <miniaudio.h>

namespace MGF3D
{
	MGF_CLASS_PTR(AudioEar)
	class AudioEar
	{
	public:
		AudioEar(ma_engine& engine, uint32 idx = 0);
		~AudioEar();

	public:
		void Update
		(
			const vec3& position, 
			const vec3& forward, 
			const vec3& up
		);

		void SetEnabled(bool enabled);
		bool IsEnabled() const;

	private:
		ma_engine& m_audioEngine;
		uint32 m_listenerIndex;
	};
}