#pragma once
#include "Asset/Asset.h"
#include "Asset/IAssetLoader.h"
#include "Asset/IAssetDescriptor.h"

namespace MGF3D
{
	MGF_CLASS_PTR(MGFType)

	class AssetManager
	{
		MGF_DECLARE_SINGLE(AssetManager)

	private:
		AssetManager();
		~AssetManager();

	public:
		bool Init();
		void Shutdown();

	public:
		void RegisterLoader(const Ptr<MGFType> type, IAssetLoaderUPtr loader);
		
		template<typename T> 
		SharedPtr<T> LoadAsync(const MGFPath& path, const MGFName& name);

	private:
		AssetPtr InternalLoadAsync(IAssetDescriptorUPtr desc);

		SMap<Ptr<MGFType>, IAssetLoaderUPtr> m_loaders;
		SMap<StringHash, AssetPtr> m_assets;
	};
}

#include "Managers/AssetManager.inl"