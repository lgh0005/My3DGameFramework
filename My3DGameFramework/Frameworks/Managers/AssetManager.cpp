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
			for (auto& resource : asset->GetResources())
				MGF_RESOURCE.RegisterSync(resource);
			asset->SetState(EAssetState::Loaded);
			RegisterSync(asset);
		}
		else asset->SetState(EAssetState::Failed);
	}

	void AssetManager::RegisterSync(AssetPtr asset)
	{
		if (!asset) return;
		m_pendingQueue.Push(asset);
	}

	void AssetManager::Update()
	{
		// 1. 락 없이 원자적 연산 1번으로 모든 일감을 뜯어옴
		auto* current = m_pendingQueue.Pop();
		if (current == nullptr) return;

		// 2. 작업 수행 (메인 스레드)
		Vector<AssetPtr> pendingList;

		// 2. 내 소유가 된 연결 리스트 순회
		while (current != nullptr)
		{
			AssetPtr asset = current->data;
			EAssetState state = asset->GetState();

			switch (state)
			{
				case EAssetState::Loaded:
				{
					asset->SetState(EAssetState::Syncing);
					[[fallthrough]];
				}

				case EAssetState::Syncing:
				{
					bool bAllReady = true;
					for (const auto& res : asset->GetResources())
					{
						if (res->GetState() != EResourceState::Ready)
						{
							bAllReady = false;
							break;
						}
					}

					if (bAllReady) asset->SetState(EAssetState::Ready);
					else pendingList.push_back(asset);
					break;
				}

				case EAssetState::Ready:
				case EAssetState::Failed:
					break;

				default:
				{
					pendingList.push_back(asset);
					break;
				}
			}

			// 3. 다 쓴 빈 노드는 메인 스레드에서 즉시 메모리 해제
			auto* temp = current;
			current = current->next;
			delete temp;
		}

		// 4. 다음 프레임에 다시 봐야 하는 에셋들을 큐에 다시 삽입
		for (auto& pendingAsset : pendingList)
			m_pendingQueue.Push(pendingAsset);
	}
}