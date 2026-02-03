# 소스 파일 목록
set(ASSET_CONVERTER_SOURCES
    # Core
    "main.cpp"
    "pch.h"                 "pch.cpp" 

    # Converters
    "Converters/ModelConverter.h"       "Converters/ModelConverter.cpp"
    "Converters/AnimationConverter.h"   "Converters/AnimationConverter.cpp"
    "Converters/ORMTexturePacker.h"     "Converters/ORMTexturePacker.cpp"

    # AssetTypes
    "AssetTypes/AssetUtils.h"           "AssetTypes/AssetUtils.cpp"
    "AssetTypes/AssetFormat.h"          "AssetTypes/AssetFormat.cpp"

    # Utils
    "Utils/Utils.h"                     "Utils/Utils.cpp"
    "Utils/Logger.h"                    "Utils/Logger.cpp"
    "Utils/ArgumentParser.h"            "Utils/ArgumentParser.cpp"
    "Utils/Types.h"
    "Utils/Defines.h"
)

# Visual Studio 필터 자동 생성
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ASSET_CONVERTER_SOURCES})

# 실행 파일 생성
add_executable(AssetConverter ${ASSET_CONVERTER_SOURCES})

# Include 및 PCH 설정
target_precompile_headers(AssetConverter PRIVATE pch.h)
target_include_directories(AssetConverter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 라이브러리 링크
target_link_libraries(AssetConverter PRIVATE
    spdlog::spdlog
    assimp::assimp
    glm::glm
)
