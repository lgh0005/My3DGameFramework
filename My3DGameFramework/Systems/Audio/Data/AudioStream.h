#pragma once
#include "AudioDataSource.h"

namespace MGF3D
{
	MGF_CLASS_PTR(AudioStream)
	class AudioStream
	{
	public:
		AudioStream(ma_engine& engine);
		~AudioStream();

	public:
		bool Load(const String& path);
		ma_data_source* GetData();

	private:
		AudioDataSource m_source;
	};
}