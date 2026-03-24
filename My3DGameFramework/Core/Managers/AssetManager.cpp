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
}