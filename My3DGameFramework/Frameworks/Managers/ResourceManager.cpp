#include "FrameworkPch.h"
#include "ResourceManager.h"
#include "Sources/Resource.h"
#include "Managers/ThreadManager.h"

namespace MGF3D
{
	ResourceManager::ResourceManager() = default;
	ResourceManager::~ResourceManager() = default;

	void ResourceManager::Shutdown()
	{
		{
			LockScope lock(m_mutex);
			m_namedCache.clear();
		}
		{
			LockScope lock(m_syncMutex);
			m_inputQueue.clear();
			m_workQueue.clear();
		}
	}

	void ResourceManager::RegisterSync(ResourcePtr resource)
	{
		if (!resource) return;
		LockScope lock(m_syncMutex);
		m_inputQueue.push_back(resource);
	}

	void ResourceManager::Update()
	{
		// 1. 임계 구역 최소화: 포인터(벡터 내부 구조)만 스왑하고 즉시 락 해제
		{
			LockScope lock(m_syncMutex);
			if (m_inputQueue.empty() && m_workQueue.empty()) return;

			if (m_workQueue.empty()) m_workQueue.swap(m_inputQueue);
			else
			{
				m_workQueue.insert(m_workQueue.end(), m_inputQueue.begin(), m_inputQueue.end());
				m_inputQueue.clear();
			}
		}

		// 2. 락이 해제된 상태에서 작업 수행 (Main Thread)
		if (m_workQueue.empty()) return;

		// 이번 프레임에 완료되지 못해 다음 프레임으로 넘길 리소스들
		Vector<ResourcePtr> pendingList;

		for (auto& resource : m_workQueue)
		{
			EResourceState state = resource->GetState();

			switch (state)
			{
				case EResourceState::Loaded: 
					resource->SetState(EResourceState::Syncing);
					[[fallthrough]];

				case EResourceState::Syncing:
					if (resource->OnSyncCreate())
						resource->SetState(EResourceState::Ready);
					else
					{
						// 실패가 아니라면 의존성 대기 중인 것으로 판단
						if (resource->GetState() != EResourceState::Failed)
							pendingList.push_back(resource);
					}
					break;

				case EResourceState::Ready:
				case EResourceState::Failed:
					break;

				default:
					// Loading 등 아직 Loaded 상태가 아닌 경우
					pendingList.push_back(resource);
					break;
			}
		}

		// 3. 작업 큐 갱신 (남은 일감들을 다음 프레임의 m_workQueue로 사용)
		m_workQueue.swap(pendingList);
	}
}
