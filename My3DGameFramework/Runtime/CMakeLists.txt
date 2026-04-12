# 0. 라이브러리 이름
set(MODULE_NAME Runtime) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES
    
    # pch
    "RuntimePch.cpp"

    # Application
    "Application/Application.h"     "Application/Application.cpp"

    # Managers
    "Managers/LevelManager.h"       "Managers/LevelManager.cpp"       "Managers/LevelManager.inl"
    #"Managers/SceneManager.h"      "Managers/SceneManager.cpp"       "Managers/SceneManager.inl"
    "Managers/GUIManager.h"        "Managers/GUIManager.cpp"

    # Editor
    "Editor/GUIWindow.h"            "Editor/GUIWindow.cpp"

    # Level
    #"Level/Level.h"     "Level/Level.cpp"

    # Managers
    #"Managers/ImguiManager.h"     "Managers/ImguiManager.cpp"
    #"Managers/LevelManager.h"     "Managers/LevelManager.cpp"    "Application/LevelManager.inl"
    #"Managers/SceneManager.h"     "Managers/SceneManager.cpp"    "Application/SceneManager.inl"

    # Pipelines
    #"Common/CullingPass.h"            "Common/CullingPass.cpp"
    #"Common/PostProcessPass.h"        "Common/PostProcessPass.cpp"
    #"Common/ShadowPass.h"             "Common/ShadowPass.cpp"
    #"Common/SkyboxPass.h"             "Common/SkyboxPass.cpp"
    #"Common/SSAOPass.h"               "Common/SSAOPass.cpp"
    ##"Common/JoltDebugGizmoPass.h"               "Common/JoltDebugGizmoPass.cpp"
    ##"Common/UIScreenSpacePass.h"               "Common/UIScreenSpacePass.cpp"
    ##"Common/UIWorldSpacePass.h"               "Common/UIWorldSpacePass.cpp"

    #"PostProcessing/PostProcessEffect.h"               "PostProcessing/PostProcessEffect.cpp"

    #"PostProcessing/Bloom/GaussianBloomEffect.h"               "PostProcessing/Bloom/GaussianBloomEffect.cpp"
    #"PostProcessing/Bloom/KawaseBloomEffect.h"                 "PostProcessing/Bloom/KawaseBloomEffect.cpp"
    #"PostProcessing/Bloom/LensFlareEffect.h"                   "PostProcessing/Bloom/LensFlareEffect.cpp"
    #"PostProcessing/Bloom/DOFEffect.h"                         "PostProcessing/Bloom/DOFEffect.cpp"

    #"PostProcessing/Display/DisplayMappingEffects.h"           "PostProcessing/Display/DisplayMappingEffects.cpp"

    #"PostProcessing/World/MotionBlurEffect.h"                  "PostProcessing/World/MotionBlurEffect.cpp"
    #"PostProcessing/World/OutlineEffect.h"                     "PostProcessing/World/OutlineEffect.cpp"
    #"PostProcessing/World/WorldSpaceEffects.h"                 "PostProcessing/World/WorldSpaceEffects.cpp"

    #"SRP/StandardGlobalUniforms.h"            "SRP/StandardGlobalUniforms.cpp"
    #"SRP/StandardRenderPipeline.h"            "SRP/StandardRenderPipeline.cpp"
    #"SRP/RenderPasses/StandardGeometryPass.h"            "SRP/RenderPasses/StandardGeometryPass.cpp"
    #"SRP/RenderPasses/StandardDeferredLightingPass.h"            "SRP/RenderPasses/StandardDeferredLightingPass.cpp"

    #"URP/UniversalGlobalUniforms.h"                               "URP/UniversalGlobalUniforms.cpp"
    #"URP/UniversalRenderPipeline.h"                               "URP/UniversalRenderPipeline.cpp"
    #"URP/RenderPasses/UniversalGeometryPass.h"                    "URP/RenderPasses/UniversalGeometryPass.cpp"
    #"URP/RenderPasses/UniversalDeferredLightingPass.h"            "URP/RenderPasses/UniversalDeferredLightingPass.cpp"

    # RuntimUtils
    
    # Scene
    #"Scene/Scene.h"                 "Scene/Scene.cpp"                       "Scene/Scene.inl"
    #"Scene/ComponentRegistry.h"     "Scene/ComponentRegistry.cpp"           "Scene/ComponentRegistry.inl"
    #"Scene/SceneRegistry.h"         "Scene/SceneRegistry.cpp"
    #"Scene/GameObjectRegistry.h"     "Scene/GameObjectRegistry.cpp"
)

# 2. 빌드 타겟 생성 및 별칭 등록
add_library(${LIB_NAME} STATIC ${SOURCES})
add_library(${TARGET_NAME} ALIAS ${LIB_NAME})

# 3. Visual Studio 필터 설정
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# 4. Include 경로 설정
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 5. 의존성 링크 : miniaudio.h
target_link_libraries(${LIB_NAME} 

PUBLIC
    Engine::Core
    Engine::Frameworks
)

# 6. PCH (Precompiled Header) 설정
target_precompile_headers(${LIB_NAME} PRIVATE "RuntimePch.h")