# 0. 라이브러리 이름
set(APP_NAME LibTester)

# 1. 소스 파일 목록 정의
set(LIB_TEST_SOURCE

    # Entry Point
    "main.cpp"

    # Scenes
    "Scenes/_TestScene/TestScene.h"     "Scenes/_TestScene/TestScene.cpp"

    # Scripts
    "Scripts/CameraController.h"     "Scripts/CameraController.cpp"
)

# 2. Visual Studio 필터 자동 생성
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIB_TEST_SOURCE})

# 3. 실행 파일 생성 (타겟 이름을 LibTester로 변경)
# 이렇게 하면 기본적으로 LibTester.exe가 생성됩니다.
add_executable(${APP_NAME} ${LIB_TEST_SOURCE})

# 4. 콘솔 앱 설정 (std::cout 출력 확인용)
set_target_properties(${APP_NAME} PROPERTIES WIN32_EXECUTABLE FALSE)

# 5. Include 및 PCH 설정
target_precompile_headers(${APP_NAME} PRIVATE "GamePch.h")
target_include_directories(${APP_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 6. 라이브러리 링크
# Engine::Core의 기능을 사용하기 위해 링크합니다.
target_link_libraries(${APP_NAME} 

PRIVATE 
    Engine::Core
    Engine::Systems
    Engine::Runtime
    Engine::Application
)

# 7. 리소스 배포 (Resource Deployment)
add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMENT "Deploying Engine Centralized Content to Bin..."

    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/Assets"
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${OUTPUT_DIR}/Assets/Game"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
            "${OUTPUT_DIR}/Assets/Game"
)