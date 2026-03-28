# 0. 라이브러리 이름
set(MODULE_NAME Assets) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # 0. Module
    "AssetModule.h"          "AssetModule.cpp"  

    # 1. Descriptors

    # 2. Loaders

    # 3. Scene

    # 4. Model
    # "Model/Model.h"                 "Model/Model.cpp"

    # 5. Animation
    # "Animation/Animation.h"         "Animation/Animation.cpp"

    # 6. Audios

    # 7. Material
    # "Material/Material.h"         "Material/Material.cpp"

    # 8. Programs

    # 9. Textures

    # 10. Script
    # "Script/LuaScript.h"         "Script/LuaScript.cpp"

    # 11. Font
    # "Fonts/Font.h"              "Fonts/Font.cpp"

)

# 2. 빌드 타겟 생성 및 별칭 등록
add_library(${LIB_NAME} STATIC ${SOURCES})
add_library(${TARGET_NAME} ALIAS ${LIB_NAME})

# 3. Visual Studio 필터 설정
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# 4. Include 경로 설정
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 5. 의존성 링크 : miniaudio.h
target_link_libraries(${LIB_NAME} 

PUBLIC
    Engine::Core
    Engine::Graphics
)

# 6. PCH (Precompiled Header) 설정
# 옵션 A: 이 라이브러리만의 개별 PCH를 사용할 경우
target_precompile_headers(${LIB_NAME} PRIVATE "AssetPch.h")

# 옵션 B: CoreLib에 정의된 PCH를 재사용할 경우 (빌드 속도 최적화)
# target_precompile_headers(${LIB_NAME} REUSE_FROM CoreLib)