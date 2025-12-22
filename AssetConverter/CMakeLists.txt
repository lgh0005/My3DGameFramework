find_package(assimp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# 1. "AssetConverter"이라는 이름의 실행 파일(.exe)을 만듭니다.
add_executable(AssetConverter
    
    # Core
    "main.cpp"
    "pch.h"                 "pch.cpp" 

    # Converters
    "Converters/ModelConverter.h"   "Converters/ModelConverter.cpp"
    "Converters/AnimationConverter.h" "Converters/AnimationConverter.cpp"
    "Converters/ORMTexturePacker.h"   "Converters/ORMTexturePacker.cpp"

    # AssetTypes
    "AssetTypes/AssetUtils.h"       "AssetTypes/AssetUtils.cpp"
    "AssetTypes/AssetFormat.h"      "AssetTypes/AssetFormat.cpp"

    # Utils
    "Utils/Utils.h"                 "Utils/Utils.cpp"
    "Utils/Logger.h"                "Utils/Logger.cpp"
    "Utils/ArgumentParser.h"        "Utils/ArgumentParser.cpp"
    "Utils/Types.h"
    "Utils/Defines.h"

 )

# 2. [PCH 설정] AssetConverter/pch.h를 AsssetConverter 타겟 전용(PRIVATE) PCH로 사용합니다.
target_precompile_headers(AssetConverter PRIVATE pch.h)

# 3. [경로 설정] pch의 위치를 알리기 위해 필요합니다.
target_include_directories(AssetConverter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 4. 외부 라이브러리를 여기에 링크
target_link_libraries(AssetConverter PUBLIC

    spdlog::spdlog
    assimp::assimp
    glm::glm
)