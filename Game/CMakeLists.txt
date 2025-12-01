# 1. "Game"이라는 이름의 실행 파일(.exe)을 만듭니다.
add_executable(Game

    # Core
    pch.h pch.cpp 
    main.cpp

    # TEMP : 테스트 전용 개발 씬
    "DevScene/DevScene.h"       "DevScene/DevScene.cpp"

    "RenderPasses/SimpleRenderPass.h" "RenderPasses/SimpleRenderPass.cpp"
    "RenderPasses/InstancedRenderPass.h" "RenderPasses/InstancedRenderPass.cpp"
    "RenderPasses/EnvironmentRenderPass.h"  "RenderPasses/EnvironmentRenderPass.cpp"

    "Scripts/CameraController.h"  "Scripts/CameraController.cpp"

)

# 2. [PCH 설정] Game/pch.h를 Game 타겟 전용(PRIVATE) PCH로 사용합니다.
target_precompile_headers(Game PRIVATE pch.h)

# 3. [경로 설정] pch의 위치를 알리기 위해 필요합니다.
target_include_directories(Game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 3. [링크 설정] Game 타겟은 Engine 라이브러리가 필요합니다.
target_link_libraries(Game PRIVATE Engine)

# 4. Resource 폴더를 빌드 폴더에 덮어쓰기
set(RESOURCE_SRC_DIR "${CMAKE_SOURCE_DIR}/Resources")
set(RESOURCE_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/Resources")

file(GLOB_RECURSE RESOURCE_FILES "${RESOURCE_SRC_DIR}/*")

add_custom_target(
    CopyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${RESOURCE_SRC_DIR}" "${RESOURCE_DEST_DIR}"
    DEPENDS ${RESOURCE_FILES}
    COMMENT "Copying resources..."
)

add_dependencies(Game CopyResources)