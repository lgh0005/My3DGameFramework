# 0. 라이브러리 이름
set(MODULE_NAME Physics) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # Managers
    "Managers/PhysicsManager.h"               "Managers/PhysicsManager.cpp"

    # Physics Core / Listeners / Interfaces
    "Physics/BPLayerInterface.h"              "Physics/BPLayerInterface.cpp"
    "Physics/PhysicsBodyActivationListener.h" "Physics/PhysicsBodyActivationListener.cpp"
    "Physics/PhysicsContactListener.h"        "Physics/PhysicsContactListener.cpp"
    "Physics/PhysicsCallbacks.h"              "Physics/PhysicsCallbacks.cpp"
    "Physics/PhysicsConfig.h"                 "Physics/PhysicsConfig.cpp"
    "Physics/PhysicsFilters.h"                "Physics/PhysicsFilters.cpp"
    "Physics/RigidbodyConstraint.h"           "Physics/RigidbodyConstraint.cpp"

    # Jolt Debug / Gizmos
    "Physics/JoltDebugRenderer.h"             "Physics/JoltDebugRenderer.cpp"
    "Physics/JoltGizmo.h"                     "Physics/JoltGizmo.cpp"
    "Physics/GizmoCommand.h"                  "Physics/GizmoCommand.cpp"
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
    Jolt::Jolt
)

# 6. PCH (Precompiled Header) 설정
# 옵션 A: 이 라이브러리만의 개별 PCH를 사용할 경우
# target_precompile_headers(${LIB_NAME} PRIVATE "AiPch.h")

# 옵션 B: CoreLib에 정의된 PCH를 재사용할 경우 (빌드 속도 최적화)
# target_precompile_headers(${LIB_NAME} REUSE_FROM CoreLib)