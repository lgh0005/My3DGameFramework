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
		// 1. 파일 I/O 및 파싱
		if (!asset->Load())
		{
			asset->SetState(EAssetState::Failed);
			return;
		}

		// 2. 파싱 완료 — Loaded 상태로 전이
		asset->SetState(EAssetState::Loaded);

		const auto& resources = asset->GetResources();

		// 3. 리소스가 없으면 바로 Ready
		if (resources.empty())
		{
			asset->SetState(EAssetState::Ready);
			return;
		}

		// 4. 리소스들 GPU 작업 등록
		for (auto& resource : resources)
			MGF_RESOURCE.RegisterSync(resource);

		// 5. GPU 작업 대기 상태로 전이
		asset->SetState(EAssetState::Syncing);
	}
}