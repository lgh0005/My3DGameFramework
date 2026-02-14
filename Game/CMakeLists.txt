# 소스 파일 목록
set(GAME_SOURCES

    # Core
    "pch.h"  "pch.cpp" 
    "main.cpp"

    # Application
    "My3DGameFramework.h"       "My3DGameFramework.cpp"

    # Standard Render Pipeline Sample Scene
    "SRPSample/DevScene/DevScene.h"       "SRPSample/DevScene/DevScene.cpp"
    "SRPSample/RenderPasses/SimpleRenderPass.h" "SRPSample/RenderPasses/SimpleRenderPass.cpp"
    "SRPSample/RenderPasses/InstancedRenderPass.h" "SRPSample/RenderPasses/InstancedRenderPass.cpp"
    "SRPSample/RenderPasses/EnvironmentRenderPass.h"  "SRPSample/RenderPasses/EnvironmentRenderPass.cpp"
    "SRPSample/Scripts/CameraController.h"  "SRPSample/Scripts/CameraController.cpp"
    "SRPSample/Scripts/PlayerController.h"  "SRPSample/Scripts/PlayerController.cpp"

    # Univeral Render Pipeline Sample Scene
    #"URPSample/DevScene/PBRScene.h"    "URPSample/DevScene/PBRScene.cpp"
    #"URPSample/RenderPasses/HDRRenderPass.h"   "URPSample/RenderPasses/HDRRenderPass.cpp"
)

# Visual Studio 필터 자동 생성
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GAME_SOURCES})

# 실행 파일 생성 및 VS 시작 프로젝트 설정
add_executable(Game ${GAME_SOURCES})

# Include 및 PCH 설정
target_precompile_headers(Game PRIVATE pch.h)
target_include_directories(Game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 라이브러리 링크
target_link_libraries(Game PRIVATE Engine)

# 리소스 배포
add_custom_command(TARGET Game POST_BUILD
    COMMENT "Deploying Game Centralized Content to Bin..."

    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/Config"
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${OUTPUT_DIR}/Config/Game"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Config"
            "${OUTPUT_DIR}/Config/Game"

    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/Assets"
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${OUTPUT_DIR}/Assets/Game"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
            "${OUTPUT_DIR}/Assets/Game"
)

