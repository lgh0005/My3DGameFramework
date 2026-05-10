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
		LockScope lock(m_mutex);
		m_namedCache.clear();
	}

	void ResourceManager::RegisterSync(ResourcePtr resource)
	{
		if (!resource) return;

		// 1. 이미 Ready이거나 Failed이면 GPU에 넘길 필요 없음
		auto state = resource->GetState();
		if (state == EResourceState::Ready || state == EResourceState::Failed) return;

		// 2. 상태를 Syncing(GPU 작업 대기)으로 변경
		resource->SetState(EResourceState::Syncing);

		// 3. GPU 워커 스레드 큐로 작업 던지기
		MGF_THREAD.PushGPUTask
		(
			[resource]()
			{
				// 이 내부 코드는 GPU 워커 스레드에서 실행됩니다.
				if (resource->OnSyncCreate())
				{
					glFlush();
					resource->SetState(EResourceState::Ready);
				}

				// 실패 시 다른 스레드에서 무한 대기하는 것을 막기 위해 상태 변경
				else
				{
					if (resource->GetState() != EResourceState::Failed)
						MGF_RESOURCE.RegisterSync(resource);

					else MGF_LOG_ERROR("Resource permanently failed.");
				}
			}
		);
	}
}
