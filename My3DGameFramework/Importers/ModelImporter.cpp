#include "EnginePch.h"
#include "ModelImporter.h"

DECLARE_DEFAULTS_IMPL(ModelImporter)
//REGISTER_IMPORTER(".mymodel", ModelImporter)

ResourceDescPtr ModelImporter::Import
(
	const ImportContext& ctx, 
	AssetRegistry& registry
)
{
	// 1. 이미 동일한 이름의 에셋 설계도가 등록되어 있는지 확인
	if (registry.find(ctx.assetName) != registry.end())
		return registry[ctx.assetName];

	// 2. ModelDesc 생성
	auto desc = ModelDescPtr(new ModelDesc(ctx.virtualPath, ctx.assetName));

	// 3. 모델 고유 설정
	// (TODO: 나중에 메타데이터 파일이 있다면 여기서 파싱)
	// 예: desc->generateTangents = true;
	// 예: desc->importAnimations = true;

	// 4. 장부에 등록하여 이후 중복 생성을 방지합니다.
	registry[ctx.assetName] = desc;

	LOG_INFO("ModelImporter: Imported -> {} [{}]", ctx.assetName, ctx.virtualPath);
	return desc;
}
