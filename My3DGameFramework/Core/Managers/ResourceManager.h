#pragma once
#include "Resource/Resource.h"
#include "Resource/IResourceLoader.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Resource)
	MGF_CLASS_PTR(ResourceDesc)

	class ResourceManager
	{
		MGF_DECLARE_SINGLE(ResourceManager)

	private:
		ResourceManager();
		~ResourceManager();

	public:
		// [Main Thread] WaitingCommit 상태의 리소스를 OnCommit 시키는 역할
		void Update();
		void Shutdown();

		// 1. 로더 등록 (예: GraphicsModule에서 TextureLoader를 등록)
		void RegisterLoader(const Ptr<MGFType> type, IResourceLoaderUPtr loader);

		// 2. 리소스 가져오기 (없으면 등록된 Descriptor를 기반으로 로드 시도)
		template<typename T> SharedPtr<T> Get(const MGFName& resourceName);

		// 3. 리소스 수동 등록
		void AddResource(const ResourcePtr& resource);

		// 4. 비동기 리소스 로드 요청
		ResourcePtr LoadResourceAsync(IResourceDescriptorUPtr desc);

	private:
		SMap<Ptr<const MGFType>, IResourceLoaderUPtr> m_loaders;
		SMap<StringHash, ResourcePtr> m_resources;

		Mutex m_commitMutex;
		SVector<ResourcePtr> m_commitQueue;
	};
}

#include "Managers/ResourceManager.inl"