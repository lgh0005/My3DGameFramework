# 0. 라이브러리 이름
set(MODULE_NAME Core) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # Module
    "CoreModule.h"           "CoreModule.cpp"

    # Defines
    "Defines/DefineClass.h"
    "Defines/DefineSystem.h"
    "Defines/DefineManager.h"
    "Defines/DefineSingleton.h"
    "Defines/DefineModule.h"
    "Defines/DefinePointers.h"

    # Common
    "Common/Types.h"
    "Common/Constants.h"
    "Common/CommonUtils.h"     "Common/CommonUtils.cpp"     "Common/CommonUtils.inl"

    # Aliases
    "Aliases/Array.h"
    "Aliases/Nullable.h"       "Aliases/Nullable.cpp"        "Aliases/Nullable.inl"
    "Aliases/Delegates.h"
    "Aliases/Threading.h"

    # Math
    "Math/Math.h"       "Math/Math.cpp"     "Math/Math.inl"
    "Math/Random.h"     "Math/Random.cpp"

    # Debug
    "Debug/Clock.h"               "Debug/Clock.cpp"
    "Debug/Asserter.h"            "Debug/Asserter.cpp"
    "Debug/Logger.h"              "Debug/Logger.cpp"      "Debug/Logger.inl" 
    "Debug/ProfileScope.h"        "Debug/ProfileScope.cpp"
    "Debug/RealTimer.h"           "Debug/RealTimer.cpp"
    "Debug/MemoryProfiler.h"      "Debug/MemoryProfiler.cpp"

    # Entities
    #"Entities/ECS/IStorage.h"      "Entities/ECS/IStorage.cpp"  
    #"Entities/ECS/Storage.h"      "Entities/ECS/Storage.cpp"   "Entities/ECS/Storage.inl"
    #"Entities/ECS/Chunk.h"          "Entities/ECS/Chunk.cpp"   "Entities/ECS/Chunk.inl"

    "Entities/Entity/Waitables/IWaitable.h"           "Entities/Entity/Waitables/IWaitable.cpp"
    #"Entities/Entity/Object.h"                             "Entities/Entity/Object.cpp"
    #"Entities/Entity/Mechanic/Mechanic.h"                  "Entities/Entity/Mechanic/Mechanic.cpp"
    #"Entities/Entity/Mechanic/IMechanicDescriptor.h"       "Entities/Entity/Mechanic/IMechanicDescriptor.cpp"
    #"Entities/Entity/Component/Component.h"                "Entities/Entity/Component/Component.cpp"
    #"Entities/Entity/Component/IComponentDescriptor.h"     "Entities/Entity/Component/IComponentDescriptor.cpp"
    #"Entities/Entity/GameObject/GameObject.h"              "Entities/Entity/GameObject/GameObject.cpp"      "Entities/Entity/GameObject/GameObject.inl" 

    # Managers
    "Managers/AssetManager.h"       "Managers/AssetManager.cpp"
    "Managers/JobManager.h"      "Managers/JobManager.cpp"
    "Managers/MemoryManager.h"      "Managers/MemoryManager.cpp"
    "Managers/NameManager.h"        "Managers/NameManager.cpp"
    "Managers/EntityManager.h"        "Managers/EntityManager.cpp"
    "Managers/TaskManager.h"        "Managers/TaskManager.cpp"
    "Managers/TypeManager.h"         "Managers/TypeManager.cpp"
    "Managers/ResourceManager.h"        "Managers/ResourceManager.cpp"      "Managers/ResourceManager.inl" 
    "Managers/PathManager.h"        "Managers/PathManager.cpp"
    "Managers/StreamManager.h"        "Managers/StreamManager.cpp"      "Managers/StreamManager.inl" 

    # Memory
    "Memory/LinearMemoryPool.h"           "Memory/LinearMemoryPool.cpp"
    "Memory/LinearAllocator.h"           "Memory/LinearAllocator.cpp"      "Memory/LinearAllocator.inl" 
    "Memory/SlabMemoryPool.h"           "Memory/SlabMemoryPool.cpp"
    "Memory/SlabAllocator.h"           "Memory/SlabAllocator.cpp"      "Memory/SlabAllocator.inl" 

    # Module
    "Module/IModuleUnit.h"          "Module/IModuleUnit.cpp"        "Module/IModuleUnit.inl" 

    # Identity
    "Identity/MGFName.h"            "Identity/MGFName.cpp"
    "Identity/MGFPath.h"            "Identity/MGFPath.cpp"
    "Identity/MGFType.h"            "Identity/MGFType.cpp"

    # Parsers
    "Parsers/JsonParser.h"          "Parsers/JsonParser.cpp"          "Parsers/JsonParser.inl"
    "Parsers/YamlParser.h"          "Parsers/YamlParser.cpp"          "Parsers/YamlParser.inl"

    # Pointer
    "Pointer/PoolAlloc.h"             "Pointer/PoolAlloc.cpp"
    "Pointer/RefCount.h"             "Pointer/RefCount.cpp"
    "Pointer/SharedPtr.h"            "Pointer/SharedPtr.cpp"           "Pointer/SharedPtr.inl"
    "Pointer/UniquePtr.h"            "Pointer/UniquePtr.cpp"           "Pointer/UniquePtr.inl"
    "Pointer/SmartPtrCast.h"         "Pointer/SmartPtrCast.cpp"        "Pointer/SmartPtrCast.inl"
    "Pointer/SmartPtrFactory.h"      "Pointer/SmartPtrFactory.cpp"     "Pointer/SmartPtrFactory.inl"

    # Resource
    "Resource/IResourceDescriptor.h" "Resource/IResourceDescriptor.cpp"
    "Resource/IResourceLoader.h"     "Resource/IResourceLoader.cpp"   
    "Resource/Resource.h"            "Resource/Resource.cpp"

    # Asset
    "Asset/Asset.h"                  "Asset/Asset.cpp"
    "Asset/IAssetDescriptor.h"       "Asset/IAssetDescriptor.cpp"
    "Asset/IAssetImporter.h"           "Asset/IAssetImporter.cpp"

    # Spatial
    "Spatial/AABB.h"                "Spatial/AABB.cpp"
    "Spatial/Rect.h"                "Spatial/Rect.cpp"              "Spatial/Rect.inl"

    "Spatial/IBounding.h"         "Spatial/IBounding.cpp"
    "Spatial/BoundingBox.h"         "Spatial/BoundingBox.cpp"
    "Spatial/CullingPlane.h"         "Spatial/CullingPlane.cpp"
    "Spatial/BoundingSphere.h"         "Spatial/BoundingSphere.cpp"
    "Spatial/Frustum.h"             "Spatial/Frustum.cpp"
    "Spatial/Ray.h"                 "Spatial/Ray.cpp"
    "Spatial/RaycastHit.h"                 "Spatial/RaycastHit.cpp"
    "Spatial/IRaycastTarget.h"      "Spatial/IRaycastTarget.cpp"

    "Spatial/Position.h"            "Spatial/Position.cpp"
    "Spatial/Rotation.h"            "Spatial/Rotation.cpp"
    "Spatial/Scale.h"               "Spatial/Scale.cpp"
    "Spatial/CoordFrame.h"          "Spatial/CoordFrame.cpp"
    "Spatial/SpatialAxes.h"         "Spatial/SpatialAxes.cpp"
    "Spatial/ViewProjection.h"      "Spatial/ViewProjection.cpp"

    # Stream
    "Stream/Stream.h"               "Stream/Stream.cpp"
    "Stream/FileStream.h"           "Stream/FileStream.cpp"
    "Stream/MemoryStream.h"         "Stream/MemoryStream.cpp"
    "Stream/MemoryStreamBuffer.h"         "Stream/MemoryStreamBuffer.cpp"
    "Stream/MemoryStreamBufferPool.h"         "Stream/MemoryStreamBufferPool.cpp"

    # Containers
    "Containers/IContainer.h"       "Containers/IContainer.cpp"
    "Containers/TMap.h"             "Containers/TMap.cpp"   "Containers/TMap.inl"
    "Containers/TSet.h"             "Containers/TSet.cpp"   "Containers/TSet.inl"
    "Containers/TVector.h"          "Containers/TVector.cpp"   "Containers/TVector.inl"
    "Containers/TString.h"          "Containers/TString.cpp"   "Containers/TString.inl"
    "Containers/TDeque.h"           "Containers/TDeque.cpp"   "Containers/TDeque.inl"
    "Containers/TBitset.h"          "Containers/TBitset.cpp"    "Containers/TBitset.inl"
    "Containers/TStaticLinkedList.h""Containers/TStaticLinkedList.cpp"    "Containers/TStaticLinkedList.inl"

    "Containers/Slab/SVector.h"     "Containers/Slab/SVector.cpp"
    "Containers/Slab/SSet.h"        "Containers/Slab/SSet.cpp"
    "Containers/Slab/SMap.h"        "Containers/Slab/SMap.cpp"
    "Containers/Slab/SString.h"     "Containers/Slab/SString.cpp"
    "Containers/Slab/SDeque.h"     "Containers/Slab/SDeque.cpp"
    "Containers/Slab/SBitset.h"     "Containers/Slab/SBitset.cpp"
    "Containers/Slab/SStaticLinkedList.h"     "Containers/Slab/SStaticLinkedList.cpp"

    "Containers/Linear/LVector.h"   "Containers/Linear/LVector.cpp"
    "Containers/Linear/LSet.h"      "Containers/Linear/LSet.cpp"
    "Containers/Linear/LMap.h"      "Containers/Linear/LMap.cpp"
    "Containers/Linear/LString.h"   "Containers/Linear/LString.cpp"
    "Containers/Linear/LDeque.h"     "Containers/Linear/LDeque.cpp"
    "Containers/Linear/LBitset.h"     "Containers/Linear/LBitset.cpp"
    "Containers/Linear/LStaticLinkedList.h"     "Containers/Linear/LStaticLinkedList.cpp"

    # Hasing
    "Hashing/IHashFunctor.h"        "Hashing/IHashFunctor.cpp"
    "Hashing/StringHash.h"          "Hashing/StringHash.cpp"

    # Task
    "Task/MGFAsyncResult.h"         "Task/MGFAsyncResult.cpp"          "Task/MGFAsyncResult.inl"
    "Task/TaskDeleter.h"            "Task/TaskDeleter.cpp"
    "Task/Task.h"                   "Task/Task.cpp"
    "Task/TaskPool.h"               "Task/TaskPool.cpp"

    # Thread
    "Thread/MGFThread.h"            "Thread/MGFThread.cpp"
    "Thread/MGFSignal.h"            "Thread/MGFSignal.cpp"  
    "Thread/MGFJob.h"               "Thread/MGFJob.cpp"  

    # Utils
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
        nlohmann_json::nlohmann_json
        ryml::ryml
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

    # 2. Assets (Engine)
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/Assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/BuiltIn"
            "${OUTPUT_DIR}/Assets/BuiltIn"
)