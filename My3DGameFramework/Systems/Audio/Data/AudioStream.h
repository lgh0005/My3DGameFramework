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
		bool Load(const MGFPath& path);
		Ptr<ma_data_source> GetData();

	private:
		AudioDataSource m_source;
	};
}