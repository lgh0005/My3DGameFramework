#include "FrameworkPch.h"
#include "AssetManager.h"
#include "Managers/ThreadManager.h"
#include "Sources/Asset.h"
#include "Sources/Resource.h"

namespace MGF3D
{
	AssetManager::AssetManager() = default;
	AssetManager::~AssetManager() = default;

	void AssetManager::Shutdown()
	{
		LockScope lock(m_cacheMutex);
		m_assetCache.clear();
		MGF_LOG_INFO("AssetManager Shutdown. Cache cleared.");
	}

	AssetPtr AssetManager::GetAsset(const String& path)
	{
		PathHash pathHash = PathHash(path);

		LockScope lock(m_cacheMutex);
		auto it = m_assetCache.find(pathHash);
		if (it != m_assetCache.end()) return it->second;
		return nullptr;
	}

	void AssetManager::ProcessCPULoadTask(AssetPtr asset)
	{
		// 1. 파일 I/O 및 파싱
		if (asset->Load())
		{
			asset->SetState(EAssetState::Loaded);

			// 2. CPU 작업 성공 시, GPU 큐로 바통 터치
			MGF_THREAD.PushGPUTask([this, gpuAsset = asset]() { ProcessGPUSyncTask(gpuAsset); });
		}
		else
		{
			// 실패 시 상태만 변경하고 파이프라인 종료
			asset->SetState(EAssetState::Failed);
		}
	}

	void AssetManager::ProcessGPUSyncTask(AssetPtr asset)
	{
		bool bAllReady = true;

		// 1. 소속된 모든 Resource의 GPU 메모리 할당 요청
		for (const auto& resource : asset->GetResources())
		{
			if (resource && resource->GetState() == EResourceState::Loaded)
			{
				if (resource->OnSyncCreate()) resource->SetState(EResourceState::Ready);
				else
				{
					resource->SetState(EResourceState::Failed);
					bAllReady = false;
				}
			}
		}

		// 2. 최종 상태 결정
		asset->SetState(CommonUtils::Select(bAllReady, EAssetState::Ready, EAssetState::Failed));
	}
}