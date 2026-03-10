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
		bool Init();
		void Update(); // [Main Thread] WaitingCommit 상태의 리소스를 OnCommit 시키는 역할
		void Shutdown();

		// 1. 로더 등록 (예: GraphicsModule에서 TextureLoader를 등록)
		void RegisterLoader(StringHash typeID, IResourceLoaderUPtr loader);

		// 2. 리소스 가져오기 (없으면 등록된 Descriptor를 기반으로 로드 시도)
		template<typename T> SharedPtr<T> Get(const MGFName& assetName);

		// 3. 리소스 수동 등록
		void AddResource(const SharedPtr<Resource>& resource);

	private:
		ResourcePtr LoadResourceAsync(const Ptr<IResourceDescriptor> desc);

		SMap<StringHash, IResourceLoaderUPtr> m_loaders;
		SMap<StringHash, ResourcePtr> m_resources;
		SMap<StringHash, IResourceDescriptorUPtr> m_assetRegistry;
	};
}

#include "Managers/ResourceManager.inl"