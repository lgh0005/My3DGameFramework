# 소스 파일 목록
set(ASSET_IMPORTER_SOURCES
    # Core
    "main.cpp"
    "pch.h"                 "pch.cpp"

    # Importers
    "Importers/ModelImporter.h"     "Importers/ModelImporter.cpp"
    "Importers/ShaderPacker.h"      "Importers/ShaderPacker.cpp"
    "Importers/TextureBaker.h"      "Importers/TextureBaker.cpp"

    # Utils
    "Utils/FileUtils.h"             "Utils/FileUtils.cpp"
    "Utils/UUIDGenerator.h"         "Utils/UUIDGenerator.cpp"
)

# Visual Studio 필터 자동 생성
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ASSET_IMPORTER_SOURCES})

# 실행 파일 생성
add_executable(AssetImporter ${ASSET_IMPORTER_SOURCES})

# Include 및 PCH 설정
target_precompile_headers(AssetImporter PRIVATE pch.h)
target_include_directories(AssetImporter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 라이브러리 링크
target_link_libraries(AssetConverter PRIVATE
    spdlog::spdlog
    assimp::assimp
    nlohmann_json::nlohmann_json
)
