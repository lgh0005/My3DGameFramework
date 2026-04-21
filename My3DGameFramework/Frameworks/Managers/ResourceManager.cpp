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
		
		m_pendingQueue.Clear();
	}

	void ResourceManager::RegisterSync(ResourcePtr resource)
	{
		if (!resource) return;
		m_pendingQueue.Push(resource);
	}

	void ResourceManager::Update()
	{
		// 1. 락 없이 원자적 연산 1번으로 모든 일감을 뜯어옴
		auto* current = m_pendingQueue.Pop();
		if (current == nullptr) return;

		Vector<ResourcePtr> pendingList;

		// 2. 내 소유가 된 연결 리스트 순회
		while (current != nullptr)
		{
			ResourcePtr resource = current->data;
			EResourceState state = resource->GetState();

			switch (state)
			{
				case EResourceState::Loaded:
				{
					resource->SetState(EResourceState::Syncing);
					[[fallthrough]];
				}

				case EResourceState::Syncing:
				{
					if (resource->OnSyncCreate()) resource->SetState(EResourceState::Ready);
					else
					{
						// 실패가 아니라면 의존성 대기 중인 것으로 판단
						if (resource->GetState() != EResourceState::Failed)
							pendingList.push_back(resource);
					}
					break;
				}

				case EResourceState::Ready:
				case EResourceState::Failed:
					break;

				default:
				{
					pendingList.push_back(resource);
					break;
				}
			}

			// 3. 다 쓴 빈 노드는 즉시 메모리 해제
			auto* temp = current;
			current = current->next;
			delete temp;
		}

		// 4. 다음 프레임에 다시 봐야 하는 리소스들을 큐에 다시 삽입
		for (auto& pendingResource : pendingList)
			m_pendingQueue.Push(pendingResource);
	}
}
