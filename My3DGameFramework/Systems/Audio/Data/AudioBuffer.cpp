#include "AudioPch.h"
#include "AudioBuffer.h"

namespace MGF3D
{
	AudioBuffer::AudioBuffer(ma_engine& engine)
		: m_source(engine) { }

	AudioBuffer::~AudioBuffer() = default;

	bool AudioBuffer::Load(const MGFPath& path)
	{
		return m_source.Load(path, MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE);
	}

	Ptr<ma_data_source> AudioBuffer::GetData()
	{
		return m_source.GetDataSource();
	}
}