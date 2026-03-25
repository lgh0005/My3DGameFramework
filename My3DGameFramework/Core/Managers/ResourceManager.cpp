#include "CorePch.h"
#include "ResourceManager.h"
#include "Managers/TaskManager.h"
#include "Identity/MGFType.h"

namespace MGF3D
{
	ResourceManager::ResourceManager() = default;
	ResourceManager::~ResourceManager() = default;

	void ResourceManager::Update()
	{
		// 1. 메인 스레드 전용 로컬 바구니
		SVector<ResourcePtr> readyToCommit;

		{
			// 2. 최소화한 임계 영역(Critical Section) : 포인터 교체
			MGF_LOCK_SCOPE(m_commitMutex);
			if (m_commitQueue.Empty()) return;
			readyToCommit.Swap(m_commitQueue);
		}

		// 3. 이제 락이 없는 상태에서 마음껏 OpenGL API를 호출합니다.
		for (auto& res : readyToCommit)
		{
			if (!res) continue;

			// OnCommit은 반드시 메인 스레드(GL Context)에서 실행됨이 보장됩니다.
			if (res->OnCommit())
			{
				res->SetState(ResourceState::Ready);
				res->OnRelease();
				MGF_LOG_INFO("ResourceManager: '{}' is now Ready.", res->GetName().CStr());
			}
			else
			{
				res->SetState(ResourceState::Failed);
				MGF_LOG_ERROR("ResourceManager: Failed to commit '{}'.", res->GetName().CStr());
			}
		}
	}

	void ResourceManager::Shutdown()
	{
		MGF_LOG_INFO("ResourceManager: Shutting down.");

		// 순서대로 정리: 리소스 -> 설계도 -> 로더
		m_resources.Release();
		m_loaders.Release();
	}

	void ResourceManager::RegisterLoader(const Ptr<MGFType> type, IResourceLoaderUPtr loader)
	{
		if (!loader) return;

		if (m_loaders.Find(type))
		{
			MGF_LOG_WARN("ResourceManager: Loader for '{}' already registered. Overwriting...", type->name.CStr());
			m_loaders.Remove(type);
		}

		m_loaders.Insert(type, std::move(loader));
	}

	void ResourceManager::AddResource(const SharedPtr<Resource>& resource)
	{
		if (!resource) return;

		StringHash key = resource->GetName().GetStringHash();
		if (m_resources.Find(key))
		{
			MGF_LOG_WARN("ResourceManager: Resource '{}' already exists in cache.", resource->GetName().CStr());
			return;
		}

		m_resources.Insert(key, resource);
	}

	ResourcePtr ResourceManager::LoadResourceAsync(IResourceDescriptorUPtr desc)
	{
		if (!desc) return nullptr;

		// 1. 적절한 로더 찾기
		auto loaderPtr = m_loaders.Find(desc->type);
		if (!loaderPtr)
		{
			MGF_LOG_ERROR("ResourceManager: No loader for TypeID {:x}", desc->type->name.CStr());
			return nullptr;
		}

		// [주의] 텍스쳐와 같은 OpenGL 컨텍스트에 종속된 작업은
		// 이쪽으로 넘기면 절대로 안된다. 순수 CPU 바운드 입출력을 요구하는
		// 무거운 작업을 이쪽으로 넘겨야 한다.
		// ex) 나쁜 예시: Texture, CubeTexture 등은 이쪽이 아닌 OnCommit으로,
		//	   좋은 예시: Material 로드를 위한 파싱, 모델 또는 애니메이션 로드를 위한 파싱 등

		// 2. 리소스 객체(껍데기) 생성
		// 로더의 Load 함수가 내부적으로 Resource 자식 객체를 '생성'만 하도록 설계되어 있어야 합니다.
		ResourcePtr newResource = nullptr;

		// 임시 Descriptor 전달을 위한 처리 (소유권 주의)
		if (!(*loaderPtr)->Load(newResource, std::move(desc)))
			return nullptr;

		// 3. 상태를 'Pending(대기)'으로 설정하고 캐시에 먼저 등록
		newResource->SetState(ResourceState::Pending);
		AddResource(newResource);

		// 4. [비동기 시작] TaskManager에게 무거운 작업(OnLoad)을 던집니다.
		auto loadTask = MGF_TASK.AcquireTask
		(
			// [WORKER THREAD 영역]
			[newResource]()
			{
				newResource->SetState(ResourceState::Loading);

				// 파일 I/O 및 파싱 (무거운 작업)
				if (newResource->OnLoad())
					newResource->SetState(ResourceState::WaitingCommit);
				else
				{
					MGF_LOG_ERROR("ResourceManager: Failed to OnLoad -> {}", newResource->GetName().CStr());
					newResource->SetState(ResourceState::Failed);
				}
			},

			// [COMPLETION 콜백 (Main Thread)]
			[newResource]()
			{
				MGF_LOG_INFO("ResourceManager: Async Load Task Finished for {}", newResource->GetName().CStr());
			}
		);

		// 5. 워커 스레드 큐에 투입
		MGF_TASK.PushTask(std::move(loadTask));

		// 껍데기 포인터를 즉시 반환 (IsReady()는 아직 false인 상태)
		return newResource;
	}
}