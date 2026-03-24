#include "CorePch.h"
#include "ResourceManager.h"
#include "Managers/TaskManager.h"

namespace MGF3D
{
	ResourceManager::ResourceManager() = default;
	ResourceManager::~ResourceManager() = default;

	bool ResourceManager::Init()
	{
		// TODO : 여기에 초기 부팅 시 스캔할 가상 경로들을 추가할 수 있습니다.
		// ex) PathManager를 통해 @Assets 폴더를 스캔하여 m_assetRegistry 채우기
		return true;
	}

	void ResourceManager::Update()
	{
		// [Main Thread] 
		// 로딩 스레드에서 작업을 마친 리소스들을 메인 스레드(GPU 컨텍스트 소유)로 확정(Commit)시킵니다.
		for (auto& pair : m_resources)
		{
			ResourcePtr& res = pair.second;
			if (!res) continue;

			// 로딩 스레드에서 OnLoad를 마치고 "Commit 대기" 중인 녀석들을 찾습니다.
			if (res->GetState() == ResourceState::WaitingCommit)
			{
				MGF_LOG_INFO("ResourceManager: Committing resource -> {}", res->GetName().CStr());

				// 1. 메인 스레드에서 자원 확정 (예: GPU 버퍼 생성 및 데이터 업로드)
				if (res->OnCommit())
				{
					// 2. 성공 시 사용 가능 상태로 변경
					res->SetState(ResourceState::Ready);

					// 3. 로딩용 임시 메모리 정리
					res->OnRelease();
					MGF_LOG_INFO("ResourceManager: Resource Ready! -> {}", res->GetName().CStr());
				}
				else
				{
					MGF_LOG_ERROR("ResourceManager: Failed to commit resource -> {}", res->GetName().CStr());
					res->SetState(ResourceState::Failed);
				}
			}
		}
	}

	void ResourceManager::Shutdown()
	{
		MGF_LOG_INFO("ResourceManager: Shutting down.");

		// 순서대로 정리: 리소스 -> 설계도 -> 로더
		m_resources.Release();
		m_assetRegistry.Release();
		m_loaders.Release();
	}

	void ResourceManager::RegisterLoader(StringHash typeID, IResourceLoaderUPtr loader)
	{
		if (!loader) return;

		if (m_loaders.Find(typeID))
		{
			MGF_LOG_WARN("ResourceManager: Loader for TypeID {:x} already registered. Overwriting...", (uint32)typeID);
			m_loaders.Remove(typeID);
		}

		m_loaders.Insert(typeID, std::move(loader));
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
		auto loaderPtr = m_loaders.Find(desc->typeID);
		if (!loaderPtr)
		{
			MGF_LOG_ERROR("ResourceManager: No loader for TypeID {:x}", (uint32)desc->typeID);
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