#include "AssetPch.h"
#include "AssetModule.h"
#include "Managers/AssetManager.h"

#include "Asset/Asset.h"
#include "Shader/Shader.h"
#include "Shader/ShaderImporter.h"

namespace MGF3D
{
	AssetModule::AssetModule() = default;
	AssetModule::~AssetModule() = default;

	bool AssetModule::OnModuleInit()
	{
		MGF_LOG_INFO("AssetModule: Baking Asset Types...");

		// 1. 최상위 부모 타입부터 순서대로 베이킹
		Asset::InitializeType();

		// 2. 개별 에셋 타입들 베이킹
		Shader::InitializeType();
		MGF_ASSET.RegisterLoader
		(
			Shader::s_type.Get(),
			MakeUnique<ShaderImporter>()
		);

		MGF_LOG_INFO("AssetModule: All Asset Types Baked Successfully.");
		return true;
	}

	void AssetModule::OnShutDown()
	{
		MGF_LOG_INFO("AssetModule: Shutting down and clearing types...");

		// 해제는 생성의 역순으로 진행합니다.
		if (Shader::s_type) Shader::s_type.Reset();

		// 최상위 타입 해제
		if (Asset::s_type)  Asset::s_type.Reset();
	}
}