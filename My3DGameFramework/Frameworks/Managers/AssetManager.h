#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(Asset)

	class AssetManager
	{
		MGF_DECLARE_SINGLE(AssetManager)

	private:
		AssetManager();
		~AssetManager();

	public:
		void Shutdown();

	public:
		template<typename T> 
		std::shared_ptr<T> LoadAssetAsync(const String& virtualPath);
		AssetPtr GetAsset(const String& virtualPath);

	private:
		void ProcessCPULoadTask(AssetPtr asset);
		void ProcessGPUSyncTask(AssetPtr asset);

	private:
		mutable Mutex m_cacheMutex;
		HashMap<PathHash, AssetPtr> m_assetCache;
	};
}

#include "Managers/AssetManager.inl"