find_package(assimp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# 1. "AssetConverter"이라는 이름의 실행 파일(.exe)을 만듭니다.
add_executable(AssetConverter
    
    # Core
    "main.cpp"
    "pch.h"                 "pch.cpp" 

    # 컨버터 클래스들
    "Converters/ModelConverter.h"   "Converters/ModelConverter.cpp"
    "Converters/AnimConverter.h"    "Converters/AnimConverter.cpp"
)

# 2. [PCH 설정] AssetConverter/pch.h를 AsssetConverter 타겟 전용(PRIVATE) PCH로 사용합니다.
target_precompile_headers(AssetConverter PRIVATE pch.h)

# 3. [헤더 경로] 현재 폴더(CMAKE_CURRENT_SOURCE_DIR)를 include 검색 경로에 추가
target_include_directories(AssetConverter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 4. 외부 라이브러리를 여기에 링크
target_link_libraries(AssetConverter PUBLIC

    spdlog::spdlog
    assimp::assimp
)