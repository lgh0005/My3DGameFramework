# 하위 모듈 추가

# Level 1: 가장 기초
add_subdirectory(CoreLib)

# Level 2: Core에 의존하는 기초 시스템들
add_subdirectory(WindowLib)
add_subdirectory(PhysicsLib)
add_subdirectory(ScriptingLib)

# Level 3: 윈도우/기초에 의존하는 그래픽 및 AI
add_subdirectory(GraphicsLib)
add_subdirectory(AILib)

# Level 4: 모든 시스템을 조립하는 게임 로직 레이어
add_subdirectory(SceneLib)

# Level 5: 에셋 변환 (Assimp 등 무거운 라이브러리 격리)
add_subdirectory(ImportLib)

# Level 6: 엔진의 최종 통합 패키지 (Application)
add_subdirectory(EngineLib)


# 컴파일 정의
target_compile_definitions(Engine PUBLIC
    WINDOW_WIDTH=1200
    WINDOW_HEIGHT=800
    WINDOW_NAME="My3DGameFramework"
)

# 리소스 배포 (Resource Deployment)
add_custom_command(TARGET Engine POST_BUILD
    COMMENT "Deploying Engine Centralized Content to Bin..."

    # 1. Config (Engine)
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/Config"
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${OUTPUT_DIR}/Config/Engine"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Config"
            "${OUTPUT_DIR}/Config"

    # 2. Assets (Engine)
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/Assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/BuiltIn"
            "${OUTPUT_DIR}/Assets/BuiltIn"
)
