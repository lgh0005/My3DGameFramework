#include "AudioPch.h"
#include "AudioStream.h"

namespace MGF3D
{
	AudioStream::AudioStream(ma_engine& engine)
		: m_source(engine) { }

	AudioStream::~AudioStream() = default;

	bool AudioStream::Load(const String& path)
	{
		return m_source.Load(path, MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM);
	}

	ma_data_source* AudioStream::GetData()
	{
		return m_source.GetDataSource();
	}
}