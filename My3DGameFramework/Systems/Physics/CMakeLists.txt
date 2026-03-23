# 0. 라이브러리 이름
set(MODULE_NAME Physics) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # Module    
    "PhysicsModule.h"       "PhysicsModule.cpp"

    # Defines
    "Defines/DefineManager.h"

    # Common
    "Common/Types.h"
    "Common/JPHMath.h"         "Common/JPHMath.cpp"        "Common/JPHMath.inl"
    "Common/JoltConstants.h"   "Common/JoltConstants.cpp"

    # Debug
    "Debug/JoltDiagnostic.h"                 "Debug/JoltDiagnostic.cpp"

    # Managers
    "Managers/PhysicsManager.h"               "Managers/PhysicsManager.cpp"
    "Managers/PhysicsQueryManager.h"               "Managers/PhysicsQueryManager.cpp"
    "Managers/CollisionManager.h"               "Managers/CollisionManager.cpp"
    "Managers/JoltJobManager.h"               "Managers/JoltJobManager.cpp"
    "Managers/JoltMemoryManager.h"            "Managers/JoltMemoryManager.cpp"

    # Memory
    "Memory/JoltAllocator.h"         "Memory/JoltAllocator.cpp"
    "Memory/JoltMemoryPool.h"         "Memory/JoltMemoryPool.cpp"

    # Pointer
    "Pointer/JoltRefFactory.h"      "Pointer/JoltRefFactory.cpp"        "Pointer/JoltRefFactory.inl"

    # Queries
    # "Queries/IPhysicsQuery.h"         "Queries/IPhysicsQuery.cpp"
    # "Queries/OverlapQuery.h"         "Queries/OverlapQuery.cpp"
    # "Queries/RaycastQuery.h"         "Queries/RaycastQuery.cpp"
    # "Queries/ShapeCastQuery.h"         "Queries/ShapeCastQuery.cpp"

    # Raycast
    "Raycast/JoltRay.h"                "Raycast/JoltRay.cpp"
    "Raycast/JoltRaycastHit.h"         "Raycast/JoltRaycastHit.cpp"

    # Body
    # "Body/RigidbodyConstraint.h"              "Body/RigidbodyConstraint.cpp"

    # Collision
    "Collision/CollisionBulider.h"              "Collision/CollisionBulider.cpp"
    "Collision/JoltFilterSet.h"              "Collision/JoltFilterSet.cpp"

    # Events
    # "Events/CollisionEvent.h"           "Events/CollisionEvent.cpp"
    # "Events/CollisionDispatchQueue.h"   "Events/CollisionDispatchQueue.cpp"
    # "Events/ActiveListener.h"           "Events/ActiveListener.cpp"
    # "Events/ContactListener.h"          "Events/ContactListener.cpp"

    # Utils
    "Utils/JoltMathBridge.h"            "Utils/JoltMathBridge.cpp"        "Utils/JoltMathBridge.inl"
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
    Engine::Core
)

# 6. PCH (Precompiled Header) 설정
# 옵션 A: 이 라이브러리만의 개별 PCH를 사용할 경우
target_precompile_headers(${LIB_NAME} PRIVATE "PhysicsPch.h")

# 옵션 B: CoreLib에 정의된 PCH를 재사용할 경우 (빌드 속도 최적화)
# target_precompile_headers(${LIB_NAME} REUSE_FROM CoreLib)