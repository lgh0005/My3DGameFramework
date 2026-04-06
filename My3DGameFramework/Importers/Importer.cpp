#include "EnginePch.h"
#include "Importer.h"

DECLARE_DEFAULTS_IMPL(Importer)

std::unordered_map<std::string, ImporterUPtr>& Importer::GetRegistry()
{
    static ImporterRegistry s_registry;
    return s_registry;
}

void Importer::Register(const std::string& ext, ImporterUPtr importer)
{
    std::string lowerExt = ext;
    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
    GetRegistry()[lowerExt] = std::move(importer);
}

ResourceDescPtr Importer::ImportAsset
(
    const std::string& ext,
    const ImportContext& ctx,
    AssetRegistry& registry
)
{
    std::string lowerExt = ext;
    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);

    auto& importers = GetRegistry();
    auto it = importers.find(lowerExt);

    if (it != importers.end())
        return it->second->Import(ctx, registry);

    return nullptr;
}