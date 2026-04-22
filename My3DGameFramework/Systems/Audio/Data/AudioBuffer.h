#pragma once
#include "AudioDataSource.h"

namespace MGF3D
{
	MGF_CLASS_PTR(AudioBuffer)
	class AudioBuffer
	{
	public:
		AudioBuffer(ma_engine& engine);
		~AudioBuffer();

	public:
		bool Load(const String& path);
		ma_data_source* GetData();

	private:
		AudioDataSource m_source;
	};
}