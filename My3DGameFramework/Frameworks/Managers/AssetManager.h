#pragma once
#include "Thread/LockFreeQueue.h"

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
		void Update();
		void Shutdown();

	public:
		template<typename T, typename... Args>
		std::shared_ptr<T> LoadAssetAsync(const String& virtualPath, Args&&... args);
		AssetPtr GetAsset(const String& virtualPath);

	private:
		void ProcessCPULoadTask(AssetPtr asset);
		void RegisterSync(AssetPtr asset);

	private:
		mutable Mutex m_cacheMutex;
		HashMap<PathHash, AssetPtr> m_assetCache;
		MGFLockFreeQueue<AssetPtr> m_pendingQueue;
	};
}

#include "Managers/AssetManager.inl"