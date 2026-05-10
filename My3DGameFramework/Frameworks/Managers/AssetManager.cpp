#include "FrameworkPch.h"
#include "AssetManager.h"
#include "Managers/ResourceManager.h"
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
	}

	void AssetManager::ProcessCPULoadTask(AssetPtr asset)
	{
		// 1. 파일 I/O 및 파싱 (이 영역은 CPUWorker 스레드에서 실행됨)
		if (asset->Load())
		{
			const auto& resources = asset->GetResources();

			// 2. 파싱 완료된 리소스들을 GPU VRAM에 올리기 위해 GPUWorker로 전달
			for (auto& resource : resources)
			{
				MGF_RESOURCE.RegisterSync(resource);
			}

			// 3. 리소스가 없는 특수 에셋이라면 즉시 Ready, 아니면 GPU 업로드 대기(Syncing)
			if (resources.empty())
			{
				asset->SetState(EAssetState::Ready);
			}
			else
			{
				asset->SetState(EAssetState::Syncing);
			}
		}
		else
		{
			asset->SetState(EAssetState::Failed);
		}
	}
}