#pragma once
#include <miniaudio.h>

namespace MGF3D
{
	MGF_CLASS_PTR(AudioDataSource)
		class AudioDataSource
	{
	public:
		AudioDataSource(ma_engine& engine);
		~AudioDataSource();

		bool Load(const String& filePath, uint32 flags);
		void Unload();

		ma_data_source* GetDataSource() { return (ma_data_source*)&m_dataSource; }

	private:
		ma_engine& m_audioEngine;
		ma_resource_manager_data_source m_dataSource;
	};
}
