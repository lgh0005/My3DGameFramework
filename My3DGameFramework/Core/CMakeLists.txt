# 0. 라이브러리 이름
set(MODULE_NAME Core) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # Common
    "Common/Aliases.h"
    "Common/Constants.h"
    "Common/Defines.h"
    "Common/Enums.h"
    "Common/Types.h"

    # Debug
    "Debug/Logger.h"       "Debug/Logger.cpp"

    # Interfaces
    "Interfaces/Importer.h"       "Interfaces/Importer.cpp"

    # Managers
    "Managers/TimeManager.h"       "Managers/TimeManager.cpp"

    # Runtime
    "Runtime/Application.h"       "Runtime/Application.cpp"   "Runtime/Application.inl"

    # Utilities
    "Utilities/AssetFormat.h"       "Utilities/AssetFormat.cpp"
    "Utilities/AssetUtils.h"       "Utilities/AssetUtils.cpp"
    "Utilities/Utils.h"       "Utilities/Utils.cpp"    "Utilities/Utils.inl") 

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
    glad::glad
    glm::glm
    glfw

    spdlog::spdlog
)

# 6. PCH (Precompiled Header) 설정
# 옵션 A: 이 라이브러리만의 개별 PCH를 사용할 경우
target_precompile_headers(${LIB_NAME} PRIVATE "CorePch.h")

# INFO : 이후에는 다른 lib에서는 이 옵션 B를 이용해서 중복되는
# 헤더 선언을 피할 수 있도록 만들어야 한다.
# 옵션 B: CoreLib에 정의된 PCH를 재사용할 경우 (빌드 속도 최적화)
# target_precompile_headers(${LIB_NAME} REUSE_FROM CoreLib)

# 7. 컴파일 설정
target_compile_definitions(${LIB_NAME} 

PUBLIC
    WINDOW_WIDTH=1200
    WINDOW_HEIGHT=800
    WINDOW_NAME="My3DGameFramework"
)