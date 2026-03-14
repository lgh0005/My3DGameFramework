#include "AudioPch.h"
#include "AudioDataSource.h"

namespace MGF3D
{
	AudioDataSource::AudioDataSource(ma_engine& engine)
		: m_audioEngine(engine)
	{
		CommonUtils::MemZero(m_dataSource);
	}

	AudioDataSource::~AudioDataSource()
	{
		Unload();
	}

	bool AudioDataSource::Load(const MGFPath& filePath, uint32 flags)
	{
		Unload();

		ma_resource_manager* pResourceManager = ma_engine_get_resource_manager(&m_audioEngine);
		if (!pResourceManager) return false;

		// miniaudio 리소스 매니저를 통한 데이터 소스 초기화
		ma_result result = ma_resource_manager_data_source_init
		(
			pResourceManager,
			filePath.GetCStr(),
			flags,
			nullptr,
			&m_dataSource
		);

		if (result != MA_SUCCESS)
		{
			MGF_LOG_ERROR("AudioDataSource: Failed to load {}", filePath.GetCStr());
			return false;
		}

		return true;
	}

	void AudioDataSource::Unload()
	{
		ma_resource_manager_data_source_uninit(&m_dataSource);
	}
}