#include "EnginePch.h"
#include "TextureImporter.h"

DECLARE_DEFAULTS_IMPL(TextureImporter)
//REGISTER_IMPORTER(".ktx", TextureImporter)

ResourceDescPtr TextureImporter::Import
(
	const ImportContext& ctx, 
	AssetRegistry& registry
)
{
	// 1. 이미 등록된 에셋인지 체크
	if (registry.find(ctx.assetName) != registry.end())
		return registry[ctx.assetName];

	// 2. 텍스처 생성 및 고유 설정
	auto desc = TextureDescPtr(new TextureDesc(ctx.virtualPath, ctx.assetName));
	desc->useMipMap = true;
	desc->sRGB = false;

	registry[ctx.assetName] = desc;
	return desc;
}
