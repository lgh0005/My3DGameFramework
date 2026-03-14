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

		// TODO : 이후에 이건 카메라나
		// 아니면 그것과 관련된 인스턴스를 넘기는 걸
		// 고려해봐야 함
		// 이후에 Core단에 Spatial에 구현된 멤버들을 적절히 써야 할 필요가 있음.
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