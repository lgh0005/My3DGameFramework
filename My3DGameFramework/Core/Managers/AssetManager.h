#pragma once
#include "Asset/Asset.h"
#include "Asset/IAssetImporter.h"
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
		void Update();
		void Shutdown();

	public:
		// 1. 로더 등록 (예: AssetModule에서 ModelLoader를 등록)
		void RegisterLoader(const Ptr<MGFType> type, IAssetImporterUPtr loader);
		
		// 2. 리소스 가져오기 (없으면 등록된 Descriptor를 기반으로 로드 시도)
		template<typename T> SharedPtr<T> Get(const MGFName& assetName);

		// 3. 에셋 수동 등록
		void AddAsset(const AssetPtr& asset);

	private:
		bool LoadExtensionMap(const MGFPath& configFileName = "FileExtension.json");
		void ScanDirectoryAsync(const MGFPath& virtualPath);
		AssetPtr LoadAssetAsync(IAssetDescriptorUPtr desc);

	private:
		SMap<Ptr<MGFType>, IAssetImporterUPtr> m_loaders;
		SMap<StringHash, AssetPtr> m_assets;
		SMap<StringHash, SString> m_extensionMap;

		Mutex m_mutex;
		SVector<AssetPtr> m_commitQueue;
	};
}

#include "Managers/AssetManager.inl"