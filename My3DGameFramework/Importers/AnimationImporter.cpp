#include "EnginePch.h"
#include "AnimationImporter.h"

DECLARE_DEFAULTS_IMPL(AnimationImporter)
//REGISTER_IMPORTER(".myanim", AnimationImporter)

ResourceDescPtr AnimationImporter::Import
(
    const ImportContext& ctx,
    AssetRegistry& registry
)
{
    // 1. 이미 동일한 이름의 애니메이션 설계도가 있는지 확인
    if (registry.find(ctx.assetName) != registry.end())
        return registry[ctx.assetName];

    // 2. AnimationDesc 생성
    auto desc = AnimationDescPtr(new AnimationDesc(ctx.virtualPath, ctx.assetName));

    // 3. 애니메이션 고유 설정 (필요 시 추가)
    // 예: desc->isLooping = true;

    // 4. 장부에 등록
    registry[ctx.assetName] = desc;

    LOG_INFO("AnimationImporter: Imported -> {} [{}]", ctx.assetName, ctx.virtualPath);
    return desc;
}