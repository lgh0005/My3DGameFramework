#include "EnginePch.h"
#include "MaterialImporter.h"

DECLARE_DEFAULTS_IMPL(MaterialImporter)

ResourceDescPtr MaterialImporter::Import
(
	const ImportContext& ctx, 
	AssetRegistry& registry
)
{
    // 1. 아주 가벼운 설계도 생성
    auto desc = MaterialDescPtr(new MaterialDesc(ctx.virtualPath, ctx.assetName));

    // 2. 장부에 등록
    registry[ctx.assetName] = desc;

    LOG_INFO("MaterialImporter: Registered -> {} [{}]", ctx.assetName, ctx.virtualPath);
    return desc;
}
