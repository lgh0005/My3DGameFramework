# 0. 라이브러리 이름
set(MODULE_NAME Core) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # Common
    "Common/Defines.h"
    "Common/Types.h"
    "Common/Constants.h"
    "Common/Nullable.h"
    "Common/Pointers.h"
    "Common/Random.h"
    "Common/Sync.h"

    "Common/Math.h"       "Common/Math.cpp"     "Common/Math.inl"
    "Common/Clock.h"    "Common/Clock.cpp"
    "Common/Random.h"    "Common/Random.cpp"

    # Debug
    "Debug/Asserter.h"           "Debug/Asserter.cpp"
    "Debug/Logger.h"             "Debug/Logger.cpp"      "Debug/Logger.inl" 
    "Debug/ProfileScope.h"        "Debug/ProfileScope.cpp"
    "Debug/RealTimer.h"           "Debug/RealTimer.cpp"
    "Debug/MemoryProfiler.h"      "Debug/MemoryProfiler.cpp"

    # Managers
    "Managers/ThreadManager.h"      "Managers/ThreadManager.cpp"
    "Managers/MemoryManager.h"      "Managers/MemoryManager.cpp"
    "Managers/NameManager.h"        "Managers/NameManager.cpp"

    # Memory
    "Memory/LinearMemoryPool.h"           "Memory/LinearMemoryPool.cpp"
    "Memory/LinearAllocator.h"           "Memory/LinearAllocator.cpp"      "Memory/LinearAllocator.inl" 
    "Memory/SlabMemoryPool.h"           "Memory/SlabMemoryPool.cpp"
    "Memory/SlabAllocator.h"           "Memory/SlabAllocator.cpp"      "Memory/SlabAllocator.inl" 

    # Naming
    "Naming/MGFName.h"            "Naming/MGFName.cpp"

    # Stream
    "Stream/Stream.h"            "Stream/Stream.cpp"
    "Stream/FilePath.h"          "Stream/FilePath.cpp" 
    "Stream/FileStream.h"        "Stream/FileStream.cpp" 

    # Containers
    "Containers/IContainer.h"       "Containers/IContainer.cpp"
    "Containers/TMap.h"       "Containers/TMap.cpp"   "Containers/TMap.inl"
    "Containers/TSet.h"       "Containers/TSet.cpp"   "Containers/TSet.inl"
    "Containers/TVector.h"       "Containers/TVector.cpp"   "Containers/TVector.inl"
    "Containers/TString.h"       "Containers/TString.cpp"   "Containers/TString.inl"

    "Containers/Slab/SVector.h"     "Containers/Slab/SVector.cpp"
    "Containers/Slab/SSet.h"     "Containers/Slab/SSet.cpp"
    "Containers/Slab/SMap.h"     "Containers/Slab/SMap.cpp"
    "Containers/Slab/SString.h"     "Containers/Slab/SString.cpp"

    "Containers/Linear/LVector.h"     "Containers/Linear/LVector.cpp"
    "Containers/Linear/LSet.h"     "Containers/Linear/LSet.cpp"
    "Containers/Linear/LMap.h"     "Containers/Linear/LMap.cpp"
    "Containers/Linear/LString.h"     "Containers/Linear/LString.cpp"

    # Hasing
    "Hashing/IHashFunctor.h"          "Hashing/IHashFunctor.cpp"
    "Hashing/StringHash.h"            "Hashing/StringHash.cpp"

    # Runtime
    #"Runtime/Application.h"       "Runtime/Application.cpp"   "Runtime/Application.inl"

    # Utils
    "Utils/CoreUtils.h"           "Utils/CoreUtils.cpp"         "Utils/CoreUtils.inl"
    "Utils/MemoryUtils.h"           "Utils/MemoryUtils.cpp"         "Utils/MemoryUtils.inl"
    "Utils/StringUtils.h"           "Utils/StringUtils.cpp"         "Utils/StringUtils.inl"
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
        glm::glm
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

# 7. 리소스 배포 (Resource Deployment)
## TODO : 경로는 Engine/BuiltIn, Engine/Config에 있는 걸 배포해야 함.
add_custom_command(TARGET CoreLib POST_BUILD
    COMMENT "Deploying Engine Centralized Content to Bin..."

    # 1. Config (Engine)
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/Config"
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${OUTPUT_DIR}/Config/Engine" ## TODO : 이거 경로도 확인 필요
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Config"
            "${OUTPUT_DIR}/Config"
)