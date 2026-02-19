# 0. 라이브러리 이름
set(MODULE_NAME AppFramework) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # Parsers
    "Parsers/JsonParser.h"      "Parsers/JsonParser.cpp"    "Parsers/JsonParser.inl"
    "Parsers/YamlParser.h"      "Parsers/YamlParser.cpp"    "Parsers/YamlParser.inl"

    # Pipelines
    "Pipelines/Common/CullingPass.h"  "Pipelines/Common/CullingPass.cpp"
    "Pipelines/Common/JoltDebugGizmoPass.h"  "Pipelines/Common/JoltDebugGizmoPass.cpp"
    "Pipelines/Common/PostProcessPass.h"  "Pipelines/Common/PostProcessPass.cpp"
    "Pipelines/Common/ShadowPass.h"  "Pipelines/Common/ShadowPass.cpp"
    "Pipelines/Common/SkyboxPass.h"  "Pipelines/Common/SkyboxPass.cpp"
    "Pipelines/Common/SSAOPass.h"  "Pipelines/Common/SSAOPass.cpp"
    "Pipelines/Common/UIScreenSpacePass.h"  "Pipelines/Common/UIScreenSpacePass.cpp"
    "Pipelines/Common/UIWorldSpacePass.h"  "Pipelines/Common/UIWorldSpacePass.cpp"

    "Pipelines/PostProcessing/PostProcessEffect.h"  "Pipelines/PostProcessing/PostProcessEffect.cpp"

    "Pipelines/PostProcessing/Bloom/DOFEffect.h"  "Pipelines/PostProcessing/Bloom/DOFEffect.cpp"
    "Pipelines/PostProcessing/Bloom/GaussianBloomEffect.h"  "Pipelines/PostProcessing/Bloom/GaussianBloomEffect.cpp"
    "Pipelines/PostProcessing/Bloom/KawaseBloomEffect.h"  "Pipelines/PostProcessing/Bloom/KawaseBloomEffect.cpp"
    "Pipelines/PostProcessing/Bloom/LensFlareEffect.h"  "Pipelines/PostProcessing/Bloom/LensFlareEffect.cpp"

    "Pipelines/PostProcessing/Display/DisplayMappingEffects.h"   "Pipelines/PostProcessing/Display/DisplayMappingEffects.cpp"

    "Pipelines/PostProcessing/World/MotionBlurEffect.h"   "Pipelines/PostProcessing/World/MotionBlurEffect.cpp"
    "Pipelines/PostProcessing/World/OutlineEffect.h"   "Pipelines/PostProcessing/World/OutlineEffect.cpp"
    "Pipelines/PostProcessing/World/WorldSpaceEffects.h"   "Pipelines/PostProcessing/World/WorldSpaceEffects.cpp"

    # Utilities
    "Utilities/LuaBindings.h"       "Utilities/LuaBindings.cpp"
)

# 2. 빌드 타겟 생성 및 별칭 등록
add_library(${LIB_NAME} STATIC ${SOURCES})
add_library(${TARGET_NAME} ALIAS ${LIB_NAME})

# 3. Visual Studio 필터 설정
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# 4. Include 경로 설정
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 5. 의존성 링크
target_link_libraries(${LIB_NAME} 

PUBLIC

PRIVATE
    nlohmann_json::nlohmann_json
    ryml::ryml
)

# 6. PCH (Precompiled Header) 설정
# 옵션 A: 이 라이브러리만의 개별 PCH를 사용할 경우
# target_precompile_headers(${LIB_NAME} PRIVATE "AiPch.h")

# 옵션 B: CoreLib에 정의된 PCH를 재사용할 경우 (빌드 속도 최적화)
# target_precompile_headers(${LIB_NAME} REUSE_FROM CoreLib)

# 7. 리소스 배포 (Resource Deployment)
## TODO : 경로는 Engine/BuiltIn, Engine/Config에 있는 걸 배포해야 함.
add_custom_command(TARGET AppFrameworkLib POST_BUILD
    COMMENT "Deploying Engine Centralized Content to Bin..."

    # 1. Config (Engine)
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/Config"
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${OUTPUT_DIR}/Config/Engine" ## TODO : 이거 경로도 확인 필요
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Config"
            "${OUTPUT_DIR}/Config"

    # 2. Assets (Engine)
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/Assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/BuiltIn"
            "${OUTPUT_DIR}/Assets/BuiltIn"
)