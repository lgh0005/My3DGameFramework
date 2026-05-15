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

		// 1. 동기화 진입 시도
		if (!resource->TryBeginSync()) return;

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
					return;
				}

				if (resource->GetState() == EResourceState::Failed)
				{
					MGF_LOG_ERROR("Resource permanently failed.");
					return;
				}

				// 일시적 실패 시 다른 스레드에서 무한 대기하는 것을 막기 위해 상태 변경
				resource->SetState(EResourceState::Empty);
				MGF_RESOURCE.RegisterSync(resource);
			}
		);
	}
}
