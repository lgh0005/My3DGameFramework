#include "FrameworkPch.h"
#include "AssetManager.h"
#include "Managers/ThreadManager.h"
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
		LockScope lock(m_syncMutex);
		m_inputQueue.push_back(asset);
	}

	void AssetManager::Update()
	{
		// 1. 입력 큐의 내용을 작업 큐로 이동
		{
			LockScope lock(m_syncMutex);
			if (m_inputQueue.empty() && m_workQueue.empty()) return;

			if (!m_inputQueue.empty())
			{
				m_workQueue.insert(m_workQueue.end(), m_inputQueue.begin(), m_inputQueue.end());
				m_inputQueue.clear();
			}
		}

		// 2. 작업 수행 (메인 스레드)
		Vector<AssetPtr> pendingList;

		for (auto& asset : m_workQueue)
		{
			EAssetState state = asset->GetState();

			// 에셋은 Loaded(2) 혹은 Syncing(3) 상태일 때 리소스들을 감시합니다.
			if (state == EAssetState::Loaded || state == EAssetState::Syncing)
			{
				bool bAllReady = true;
				for (const auto& res : asset->GetResources())
				{
					if (res->GetState() != EResourceState::Ready) // 리소스가 4가 아니면
					{
						bAllReady = false;
						break;
					}
				}

				if (bAllReady)
				{
					asset->SetState(EAssetState::Ready); // 드디어 4!

				}
				else
				{
					// 아직 리소스가 준비 안 됐다면 다시 대기열에 추가
					pendingList.push_back(asset);

					// [꿀팁] 만약 상태가 Loaded(2)였다면 Syncing(3)으로 바꿔서 
					// "나 지금 자식들 기다리는 중이야"라고 표시해 주는 것도 좋습니다.
					if (state == EAssetState::Loaded) asset->SetState(EAssetState::Syncing);
				}
			}
		}

		// 3. 남은 일감 스왑
		m_workQueue.swap(pendingList);
	}
}