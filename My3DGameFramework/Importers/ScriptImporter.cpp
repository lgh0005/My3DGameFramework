#include "EnginePch.h"
#include "ScriptImporter.h"

DECLARE_DEFAULTS_IMPL(ScriptImporter)

ResourceDescPtr ScriptImporter::Import
(
	const ImportContext& ctx,
	AssetRegistry& registry
)
{
	auto desc = ScriptDescPtr(new ScriptDesc(ctx.virtualPath, ctx.assetName));
	registry[ctx.assetName] = desc;
	LOG_INFO("ScriptImporter: Registered -> {} [{}]", ctx.assetName, ctx.virtualPath);
	return desc;
}
