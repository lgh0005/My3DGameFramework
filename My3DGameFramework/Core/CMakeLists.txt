# 0. 라이브러리 이름
set(MODULE_NAME Core) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # pch
    "CorePch.cpp"

    # Types
    "Types/Containers.h" "Types/Delegates.h"
    "Types/FileStream.h" "Types/Math.h"
    "Types/SmartPtrs.h" "Types/Strings.h"
    "Types/Types.h"

    # Constants
    "Constants/ConstantMath.h"        "Constants/ConstantAsset.h" 
    "Constants/ConstantStringHash.h"  "Constants/ConstantType.h" 
    "Constants/ConstantString.h"      "Constants/ConstantChunk.h"

    # ECS
    "ECS/MGFChunk.h"    "ECS/MGFChunk.cpp"    "ECS/MGFChunk.inl"
    "ECS/MGFPackedArray.h"    "ECS/MGFPackedArray.cpp"    "ECS/MGFPackedArray.inl"

    # Defines
    "Defines/DefineAsserter.h"          "Defines/DefineBreakpoint.h"
    "Defines/DefineClassPointers.h"     "Defines/DefineDisables.h"
    "Defines/DefineHash.h"              
    "Defines/DefineLoggings.h"          "Defines/DefineManagers.h"
    "Defines/DefineSingleton.h"         "Defines/DefineStructPointers.h"
    "Defines/DefineTimeStamp.h"         "Defines/DefineUtilities.h"

    # Entities
    "Entities/Component.h"       "Entities/Component.cpp"
    "Entities/GameObject.h"      "Entities/GameObject.cpp"   "Entities/GameObject.inl"
    "Entities/Mechanic.h"       "Entities/Mechanic.cpp"
    "Entities/Object.h"       "Entities/Object.cpp"

    # Components
    "Components/Transform.h"        "Components/Transform.cpp"
    "Components/Camera.h"           "Components/Camera.cpp"

    # Mechanics
    "Mechanics/TransformHierarchy.h"       "Mechanics/TransformHierarchy.cpp"
    "Mechanics/TransformCoordinate.h"      "Mechanics/TransformCoordinate.cpp"
    "Mechanics/CameraShutter.h"            "Mechanics/CameraShutter.cpp"

    # States
    "States/MGFStateMachine.h"        "States/MGFStateMachine.cpp"      "States/MGFStateMachine.inl"

    # Registries
    "Registries/IRegistry.h"        "Registries/IRegistry.cpp"
    "Registries/IComponentRegistry.h"        "Registries/IComponentRegistry.cpp"
    "Registries/ComponentRegistry.h"     "Registries/ComponentRegistry.cpp"           "Registries/ComponentRegistry.inl"
    "Registries/GameObjectRegistry.h"     "Registries/GameObjectRegistry.cpp"

    # Debug
    "Debug/Asserter.h"      "Debug/Asserter.cpp"
    "Debug/Logger.h"        "Debug/Logger.cpp"    "Debug/Logger.inl"
    "Debug/Stopwatch.h"     "Debug/Stopwatch.cpp"

    # Parsers
    "Parsers/JsonParser.h"      "Parsers/JsonParser.cpp"           "Parsers/JsonParser.inl"

    # Input
    "Input/MGFInputInterface.h"	  "Input/MGFInputInterface.cpp"

    "Input/MGFInputDevice.h"	  "Input/MGFInputDevice.cpp"
    "Input/Devices/MGFKeyboardDevice.h"	  "Input/Devices/MGFKeyboardDevice.cpp"
    "Input/Devices/MGFMouseDevice.h"	  "Input/Devices/MGFMouseDevice.cpp"

    "Input/MGFInputAction.h"              "Input/MGFInputAction.cpp"
    "Input/Actions/MGFKeyAction.h"        "Input/Actions/MGFKeyAction.cpp"
    "Input/Actions/MGFMouseAction.h"      "Input/Actions/MGFMouseAction.cpp"

    # Window
    "Window/MGFWindowInterface.h"  "Window/MGFWindowInterface.cpp"
    "Window/MGFWindow.h"           "Window/MGFWindow.cpp"
    "Window/MGFScreen.h"           "Window/MGFScreen.cpp"

    # Managers
    "Managers/InputManager.h"      "Managers/InputManager.cpp"        "Managers/InputManager.inl"
    "Managers/EntityManager.h"      "Managers/EntityManager.cpp"       "Managers/EntityManager.inl"
    "Managers/PathManager.h"      "Managers/PathManager.cpp"        "Managers/PathManager.inl"
    "Managers/ThreadManager.h"      "Managers/ThreadManager.cpp"
    "Managers/NameManager.h"      "Managers/NameManager.cpp"
    "Managers/TypeManager.h"      "Managers/TypeManager.cpp"
    "Managers/TimeManager.h"      "Managers/TimeManager.cpp"
    "Managers/WindowManager.h"      "Managers/WindowManager.cpp"

    # Identities
    "Identities/MGFObjectInfo.h"      
    "Identities/MGFType.h"      "Identities/MGFType.cpp"
    "Identities/MGFTypeTree.h"      "Identities/MGFTypeTree.cpp"
    "Identities/MGFTypeCaster.h"   "Identities/MGFTypeCaster.cpp"      "Identities/MGFTypeCaster.inl"

    # Hashes
    "Hashes/IHashFunctor.h"      "Hashes/IHashFunctor.cpp"
    "Hashes/PathHash.h"        "Hashes/PathHash.cpp"            "Hashes/PathHash.inl"
    "Hashes/StringHash.h"        "Hashes/StringHash.cpp"            "Hashes/StringHash.inl"
    "Hashes/TypeHash.h"          "Hashes/TypeHash.cpp"              "Hashes/TypeHash.inl"
    "Hashes/ObjectIDHash.h"      "Hashes/ObjectIDHash.cpp"          "Hashes/ObjectIDHash.inl"

    # Sources
    "Sources/Asset.h"         "Sources/Asset.cpp"
    "Sources/Resource.h"      "Sources/Resource.cpp"
    "Sources/NamedResource.h" "Sources/NamedResource.cpp"

    # Thread
    "Thread/TaskQueue.h"           "Thread/TaskQueue.cpp"
    "Thread/LockFreeQueue.h"      "Thread/LockFreeQueue.cpp" "Thread/LockFreeQueue.inl"
    "Thread/Worker.h"           "Thread/Worker.cpp"
    "Thread/CPUWorker.h"        "Thread/CPUWorker.cpp"
    "Thread/GPUWorker.h"        "Thread/GPUWorker.cpp"

    # Utils
    "CoreUtils/AssetUtils.h"       "CoreUtils/AssetUtils.cpp"           "CoreUtils/AssetUtils.inl"
    "CoreUtils/CommonUtils.h"      "CoreUtils/CommonUtils.cpp"           "CoreUtils/CommonUtils.inl"
    "CoreUtils/MathUtils.h"        "CoreUtils/MathUtils.cpp"            "CoreUtils/MathUtils.inl"

    # Module
    "Module/IModuleInterface.h"     "Module/IModuleInterface.cpp"
    "CoreModule.h"       "CoreModule.cpp"
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
        glad::glad
        glfw
        glm::glm
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

# 6. PCH (Precompiled Header) 설정
target_precompile_headers(${LIB_NAME} PRIVATE "CorePch.h")

# 7. 컴파일 설정
target_compile_definitions(${LIB_NAME} 

PUBLIC
    WINDOW_WIDTH=1200
    WINDOW_HEIGHT=800
    WINDOW_NAME="My3DGameFramework"
)

# 7. 리소스 배포 (Resource Deployment)
add_custom_command(TARGET CoreLib POST_BUILD
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