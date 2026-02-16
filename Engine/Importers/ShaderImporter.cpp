#include "EnginePch.h"
#include "ShaderImporter.h"

DECLARE_DEFAULTS_IMPL(ShaderImporter)
REGISTER_IMPORTER(".vert", ShaderImporter)
REGISTER_IMPORTER(".frag", ShaderImporter)
REGISTER_IMPORTER(".tesc", ShaderImporter)
REGISTER_IMPORTER(".tese", ShaderImporter)
REGISTER_IMPORTER(".geom", ShaderImporter)
REGISTER_IMPORTER(".comp", ShaderImporter)

ResourceDescPtr ShaderImporter::Import(const ImportContext& ctx, AssetRegistry& registry)
{
    // 1. 확장자 추출 (소문자로 변환하여 비교)
    std::string ext = fs::path(ctx.filePath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // 2. 컴퓨트 셰이더 처리 (.comp)
    if (ext == ".comp")
    {
        auto desc = ComputeProgramDescPtr(new ComputeProgramDesc(ctx.virtualPath, ctx.assetName));
        registry[ctx.assetName] = desc;
        LOG_INFO("ShaderImporter: Compute Shader Detected -> {}", ctx.assetName);
        return desc;
    }

    // 3. 그래픽스 셰이더 병합 로직 (.vert, .frag, .tesc, .tese, .geom)
    GraphicsProgramDescPtr graphicsDesc = nullptr;
    auto it = registry.find(ctx.assetName);
    if (it != registry.end())
    {
        if (it->second->GetDescType() == ResourceType::GraphicsProgram)
            graphicsDesc = std::static_pointer_cast<GraphicsProgramDesc>(it->second);
        else
        {
            LOG_ERROR("ShaderImporter: Name collision for '{}'. Registered as different type.", ctx.assetName);
            return nullptr;
        }
    }

    // 장부에 없으면 새로 생성
    if (!graphicsDesc)
    {
        graphicsDesc = GraphicsProgramDescPtr(new GraphicsProgramDesc("", "", ctx.assetName));
        registry[ctx.assetName] = graphicsDesc;
    }

    // 4. 확장자에 따라 경로 주입 (이미 생성된 desc에 조각을 맞춥니다)
    if      (ext == ".vert") graphicsDesc->vsPath = ctx.virtualPath;
    else if (ext == ".frag") graphicsDesc->fsPath = ctx.virtualPath;
    else if (ext == ".tesc") graphicsDesc->tcPath = ctx.virtualPath;
    else if (ext == ".tese") graphicsDesc->tePath = ctx.virtualPath;
    else if (ext == ".geom") graphicsDesc->gsPath = ctx.virtualPath;

    LOG_INFO("ShaderImporter: Attached stage [{}] to program [{}]", ext, ctx.assetName);
    return graphicsDesc;
}

