# 소스 파일 목록
set(ASSET_CONVERTER_SOURCES

    "main.cpp"
    "ConverterPch.h"                    "ConverterPch.cpp"

    # Assets
    "Assets/CubeFaceElement.h"
    "Assets/RawAnimation.h"
    "Assets/RawAnimChannel.h"
    "Assets/RawBone.h"
    "Assets/RawBoneInfo.h"
    "Assets/RawKeyPosition.h"      "Assets/RawKeyRotation.h"       "Assets/RawKeyScale.h"
    "Assets/RawMaterial.h"
    "Assets/RawMesh.h"
    "Assets/RawModel.h"
    "Assets/RawNode.h"
    "Assets/RawStaticVertex.h"
    "Assets/RawTexture.h"
    "Assets/RawSkinnedVertex.h"    "Assets/RawSkinnedVertex.h"
    "Assets/RawImage.h"            "Assets/RawImage.cpp"

    # Core
    "Core/AssetUtils.h"                "Core/AssetUtils.cpp"
    "Core/MathUtils.h"                "Core/MathUtils.cpp"
    "Core/ArgumentParser.h"            "Core/ArgumentParser.cpp"
    "Core/ParseResult.h"            "Core/ParseResult.cpp"

    # Animation
    "Animation/Pose.h"                 "Animation/Pose.cpp"
    "Animation/AnimChannel.h"          "Animation/AnimChannel.cpp"

    # Debug
    "Debug/Logger.h"                    "Debug/Logger.cpp"          "Debug/Logger.inl"

    # Converters
    "Converters/ModelConverter.h"       "Converters/ModelConverter.cpp"
    "Converters/AnimationConverter.h"   "Converters/AnimationConverter.cpp"
    "Converters/ORMTexturePacker.h"     "Converters/ORMTexturePacker.cpp"
    "Converters/KTXTextureConverter.h"  "Converters/KTXTextureConverter.cpp"
    "Converters/KTXCubeMapConverter.h"  "Converters/KTXCubeMapConverter.cpp"

    # Defines
    "Defines/DefineConverters.h"        "Defines/DefineManagers.h"
    "Defines/DefineDisables.h"        "Defines/DefineClassPointers.h"
    "Defines/DefineSingleton.h"         "Defines/DefineUtilities.h"
    "Defines/DefineLoggings.h"

    # Managers
    "Managers/ThreadManager.h"          "Managers/ThreadManager.cpp"

    # Types
    "Types/Containers.h"
    "Types/Strings.h"
    "Types/Types.h"
    "Types/Delegates.h"
    "Types/FileStream.h"
    "Types/Threading.h"
    "Types/Math.h"
    "Types/SmartPtrs.h"

    # Thread
    "Thread/Worker.h"                   "Thread/Worker.cpp"
    "Thread/TaskQueue.h"                "Thread/TaskQueue.cpp"

    # Utils
    "Types/Types.h"
    "Types/Containers.h"
    "Types/Strings.h"
)

# Visual Studio 필터 자동 생성
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ASSET_CONVERTER_SOURCES})

# 실행 파일 생성
add_executable(AssetConverter ${ASSET_CONVERTER_SOURCES})

# Include 및 PCH 설정
target_precompile_headers(AssetConverter PRIVATE ConverterPch.h)
target_include_directories(AssetConverter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 라이브러리 링크
target_link_libraries(AssetConverter PRIVATE
    spdlog::spdlog
    assimp::assimp
    glm::glm
    KTX::ktx
)

# 출력 경로 개별 설정
set(CONVERTER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/Bin/AssetConverter")
set_target_properties(AssetConverter PROPERTIES

    # 실행 파일(.exe)이 생성될 위치 지정
    RUNTIME_OUTPUT_DIRECTORY "${CONVERTER_OUTPUT_DIR}"
    
    # VS 설정(Debug/Release)에 따른 경로 강제 통일
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CONVERTER_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CONVERTER_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CONVERTER_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CONVERTER_OUTPUT_DIR}"
    
    # VS 시작 프로젝트 설정
    VS_STARTUP_PROJECT AssetConverter
)

# 프론트엔드(Python) 배포
add_custom_target(DeployAssetConverterTools
    COMMENT "Deploying AssetConverter Tools..."

    COMMAND ${CMAKE_COMMAND} -E make_directory "${CONVERTER_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${CONVERTER_OUTPUT_DIR}/Tools"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Tools"
            "${CONVERTER_OUTPUT_DIR}/Tools"
)

add_dependencies(DeployAssetConverterTools AssetConverter)

# PyInstaller 자동 빌드
find_program(PYINSTALLER_PATH "pyinstaller")
if (PYINSTALLER_PATH)
    add_custom_target(BuildAssetTool
        COMMENT "Building Python Frontend with PyInstaller..."
        
        # main.py를 AssetTool.exe로 빌드
        COMMAND ${PYINSTALLER_PATH}
            --noconfirm --clean --onefile --windowed
            --distpath "${CONVERTER_OUTPUT_DIR}"
            --workpath "${CMAKE_CURRENT_BINARY_DIR}/PyInstallerTemp/build"
            --specpath "${CMAKE_CURRENT_BINARY_DIR}/PyInstallerTemp"
            --name "AssetTool"
            "${CMAKE_CURRENT_SOURCE_DIR}/Tools/main.py"

        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    # 빌드 성공 메시지 (CMake 출력창에서 확인 가능)
    add_dependencies(BuildAssetTool DeployAssetConverterTools)
    message(STATUS "PyInstaller detected. 'AssetTool.exe' will be built automatically.")
else()
    message(STATUS "PyInstaller NOT found. Skipping Python build. (Only source files copied)")
endif()