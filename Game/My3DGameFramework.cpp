#include "pch.h"
#include "My3DGameFramework.h"
#include "AppRegistry.h"

DECLARE_DEFAULTS_IMPL(My3DGameFramework)

My3DGameFrameworkUPtr My3DGameFramework::Create(int32 width, int32 height, const std::string& title)
{
    auto app = My3DGameFrameworkUPtr(new My3DGameFramework());
    if (!app->Init(width, height, title)) return nullptr;
    return std::move(app);
}

bool My3DGameFramework::Init(int32 width, int32 height, const std::string& title)
{
    if (!Super::Init(width, height, title)) return false;
    return true;
}

void My3DGameFramework::OnInit()
{
    // 0. SRP 전용 씬
    RegisterLevel<DevScene, StandardRenderPipeline>
    (
        "DevLevel",     // Level Name (Run할 때 사용)
        "DevScene",     // Scene Key
        "Standard"      // Pipeline Key
    );

    // 1. URP 전용 씬
    RegisterLevel<PBRScene, UniversalRenderPipeline>
    (
        "PBRLevel",
        "PBRScene",
        "Universal"
    );
}
