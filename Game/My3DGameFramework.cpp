#include "pch.h"
#include "My3DGameFramework.h"

// [사용자 정의 컨텐츠 헤더들]
#include "SRP/StandardRenderPipeline.h"
#include "SRPSample/DevScene/DevScene.h"

My3DGameFramework::My3DGameFramework() = default;

void My3DGameFramework::OnInit()
{
    // 0. SRP 전용 씬
    RegisterLevel<DevScene, StandardRenderPipeline>
    (
        "DevLevel",     // Level Name (Run할 때 사용)
        "DevScene",     // Scene Key
        "Standard"      // Pipeline Key
    );

    // 1. TODO : URP 전용 씬을 만들어서 추가해야 함.
}
