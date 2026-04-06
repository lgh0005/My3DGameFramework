# 0. 라이브러리 이름
set(MODULE_NAME Core) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES
    ## TODO
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
## TODO : 얘네들은 차라리 빌드가 되기 전에 수행해야 하는게 아닐까?
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