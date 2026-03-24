#include "CorePch.h"
#include "AssetManager.h"
#include "Identity/MGFType.h"
#include "Managers/TaskManager.h"

namespace MGF3D
{
	AssetManager::AssetManager() = default;
	AssetManager::~AssetManager() = default;

	bool AssetManager::Init()
	{
		// TODO
		// 1. 에셋 경로 폴더 디렉터리를 보면서 비동기로 모든 에셋을
		// 한 번에 로드한다.
		return true;
	}

	void AssetManager::Shutdown()
	{
		m_assets.Release();
		m_loaders.Release();
	}

	void AssetManager::RegisterLoader(const Ptr<MGFType> type, IAssetLoaderUPtr loader)
	{
		if (!type || !loader) return;
		m_loaders.Insert(type, std::move(loader));
	}

	// TODO : 이 부분은 inl에 그냥 곧바로 쓰는 편이 좋을 듯 하다.
	AssetPtr AssetManager::InternalLoadAsync(IAssetDescriptorUPtr desc)
	{
		// 1. 적절한 로더 찾기
		auto loaderPtr = m_loaders.Find(desc->type);
		if (!loaderPtr)
		{
			MGF_LOG_ERROR("AssetManager: No loader for type '{}'", desc->type->name.CStr());
			return nullptr;
		}

		// 2. 에셋 인스턴스 생성
		AssetPtr newAsset = nullptr;
		if (!(*loaderPtr)->Load(newAsset, std::move(desc)))
			return nullptr;

		// 3. 비동기 태스크 생성 및 실행
		auto loadTask = MGF_TASK.AcquireTask([newAsset]()
			{
				MGF_LOG_INFO("AssetManager: Loading Asset -> {}", newAsset->GetName().CStr());
				if (!newAsset->OnLoad())
				{
					MGF_LOG_ERROR("AssetManager: Failed to OnLoad Asset -> {}", newAsset->GetName().CStr());
				}
			});

		MGF_TASK.PushTask(std::move(loadTask));
		return newAsset;
	}
}