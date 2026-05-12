# 0. 라이브러리 이름
set(MODULE_NAME Runtime) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES
    
    # pch
    "RuntimePch.cpp"

    # Module
    "RuntimeModule.h"              "RuntimeModule.cpp"

    # Defines
    "Defines/DefineManagers.h"

    # Managers
    "Managers/SceneManager.h"      "Managers/SceneManager.cpp"       "Managers/SceneManager.inl"
    "Managers/GUIManager.h"        "Managers/GUIManager.cpp"

    # Editor
    "Editor/GUIWindow.h"            "Editor/GUIWindow.cpp"

    # RenderPipeline & RenderPass
    "Pipelines/MGFRenderPipeline.h"                          "Pipelines/MGFRenderPipeline.cpp"

    "Pipelines/RenderPasses/MGFShadowPass.h"                 "Pipelines/RenderPasses/MGFShadowPass.cpp"
    "Pipelines/RenderPasses/MGFGeometryPass.h"               "Pipelines/RenderPasses/MGFGeometryPass.cpp"
    "Pipelines/RenderPasses/MGFDeferredLightingPass.h"       "Pipelines/RenderPasses/MGFDeferredLightingPass.cpp"
    "Pipelines/RenderPasses/MGFSkyboxPass.h"                 "Pipelines/RenderPasses/MGFSkyboxPass.cpp"
    "Pipelines/RenderPasses/MGFSSAOPass.h"                   "Pipelines/RenderPasses/MGFSSAOPass.cpp"
    "Pipelines/RenderPasses/MGFSkyboxPass.h"                 "Pipelines/RenderPasses/MGFSkyboxPass.cpp"
    "Pipelines/RenderPasses/MGFPostProcessingPass.h"         "Pipelines/RenderPasses/MGFPostProcessingPass.cpp"

    # Managers
    "Managers/GUIManager.h"     "Managers/GUIManager.cpp"
    "Managers/SceneManager.h"     "Managers/SceneManager.cpp"    "Managers/SceneManager.inl"
    
    # Scene
    "Scene/Scene.h"                 "Scene/Scene.cpp"
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
    imgui::imgui
    imguizmo::imguizmo

PUBLIC
    Engine::Core
    Engine::Frameworks
)

# 6. PCH (Precompiled Header) 설정
target_precompile_headers(${LIB_NAME} PRIVATE "RuntimePch.h")