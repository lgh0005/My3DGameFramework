# 0. 라이브러리 이름
set(MODULE_NAME Application) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES
    
    # pch
    "ApplicationPch.cpp"

    # Application
    "Application/Application.h"     "Application/Application.cpp"

    # Managers
    "Managers/LevelManager.h"       "Managers/LevelManager.cpp"       "Managers/LevelManager.inl"

    # Interfaces
    "Interfaces/Cores.h"        "Interfaces/Cores.cpp"
    "Interfaces/Scenes.h"       "Interfaces/Scenes.cpp"      "Interfaces/Scenes.inl"
    "Interfaces/Levels.h"       "Interfaces/Levels.cpp"
    "Interfaces/Pipelines.h"    "Interfaces/Pipelines.cpp"
    "Interfaces/Inputs.h"       "Interfaces/Inputs.cpp"
    "Interfaces/Entity.h"       "Interfaces/Entity.cpp"      "Interfaces/Entity.inl"

    # Level
    "Level/Level.h"     "Level/Level.cpp"
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
    Engine::Runtime
    Engine::Systems
)

# 6. PCH (Precompiled Header) 설정
target_precompile_headers(${LIB_NAME} PRIVATE "ApplicationPch.h")