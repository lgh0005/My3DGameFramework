#include "EnginePch.h"
#include "FontImporter.h"

DECLARE_DEFAULTS_IMPL(FontImporter)
//REGISTER_IMPORTER(".ttf", FontImporter)
//REGISTER_IMPORTER(".otf", FontImporter)

ResourceDescPtr FontImporter::Import
(
	const ImportContext& ctx,
	AssetRegistry& registry
)
{
	// 1. 이미 동일한 이름의 폰트 설계도가 있는지 확인
	if (registry.find(ctx.assetName) != registry.end())
		return registry[ctx.assetName];

	// 2. FontDesc 생성
	auto desc = FontDescPtr(new FontDesc(ctx.virtualPath, ctx.assetName));

	// 3. 폰트 기본 설정
	// TODO: 나중에 폰트 파일의 메타데이터를 읽어 기본 사이즈를 결정할 수도 있습니다.
	desc->fontSize = 24;

	// 4. 장부에 등록
	registry[ctx.assetName] = desc;

	LOG_INFO("FontImporter: Imported -> {} [{}]", ctx.assetName, ctx.virtualPath);
	return desc;
}