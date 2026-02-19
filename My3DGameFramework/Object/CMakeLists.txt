# 0. 라이브러리 이름
set(MODULE_NAME Object) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # ECS
    "Entity/Archetype.h"             "Entity/Archetype.cpp"
    "Entity/Chunk.h"                 "Entity/Chunk.cpp"

    # Actor
    "Actor/Component.h"             "Actor/Component.cpp"
    "Actor/GameObject.h"            "Actor/GameObject.cpp"
    "Components/Transform.h"        "Components/Transform.cpp"

    # Managers
    "Managers/EntityManager.h"      "Managers/EntityManager.cpp"
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

)