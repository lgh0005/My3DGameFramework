# 1. "Game"이라는 이름의 실행 파일(.exe)을 만듭니다.
add_executable(Game

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
    "URPSample/DevScene/PBRScene.h"    "URPSample/DevScene/PBRScene.cpp"
    "URPSample/RenderPasses/HDRRenderPass.h"   "URPSample/RenderPasses/HDRRenderPass.cpp"
    #"URPSample/RenderPasses/SphericalToCubePass.h"   "URPSample/RenderPasses/SphericalToCubePass.cpp"
    #"URPSample/RenderPasses/HDRSkyboxPass.h"    "URPSample/RenderPasses/HDRSkyboxPass.cpp"

  )

# 2. [PCH 설정] Game/pch.h를 Game 타겟 전용(PRIVATE) PCH로 사용합니다.
target_precompile_headers(Game PRIVATE pch.h)

# 3. [경로 설정] pch의 위치를 알리기 위해 필요합니다.
target_include_directories(Game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 3. [링크 설정] Game 타겟은 Engine 라이브러리가 필요합니다.
target_link_libraries(Game PRIVATE Engine)

# 4. 리소스 및 설정 병합 (순서 중요: Engine -> Game)
add_custom_command(TARGET Game POST_BUILD
    # [Step 1] 목적지 폴더 생성 (없으면 에러나니까 안전장치)
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Game>/Config"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Game>/Resources"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Game>/Resources/Shaders"

    # -----------------------------------------------------------------
    # [Step 2] 셰이더 병합 (Engine 기본값 -> Game 커스텀)
    # -----------------------------------------------------------------
    
    # 2-1. 엔진 필수 셰이더 복사 (Base)
    # Engine/Shaders 내용을 -> Bin/Resources/Shaders 로 복사
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Engine/Shaders"
    "$<TARGET_FILE_DIR:Game>/Resources/Shaders"

    # -----------------------------------------------------------------
    # [Step 3] 일반 리소스 병합 (Engine 리소스 -> Game 리소스)
    # -----------------------------------------------------------------

    # 3-1. 게임 리소스 전체 복사 (Override)
    # Root/Resources 내용을 -> Bin/Resources 로 복사
    # (만약 사용자가 커스텀 셰이더를 Resources/Shaders에 넣었다면 여기서 덮어씌워짐)
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Resources"
    "$<TARGET_FILE_DIR:Game>/Resources"

    # -----------------------------------------------------------------
    # [Step 4] 설정(Config) 병합 (Engine 기본값 -> Game 커스텀)
    # -----------------------------------------------------------------

    # 4-1. 엔진 기본 설정 복사 (Base)
    # Engine/Config -> Bin/Config
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Engine/Config"
    "$<TARGET_FILE_DIR:Game>/Config"

    # 4-2. 게임 전용 설정 복사 (Override)
    # Game/Config -> Bin/Config
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/Config"
    "$<TARGET_FILE_DIR:Game>/Config"

    # 로그 출력
    COMMENT "Deploying Assets: Merging Engine defaults with Game overrides..."
)
