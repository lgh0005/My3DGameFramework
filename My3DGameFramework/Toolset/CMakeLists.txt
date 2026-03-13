# 1. 하위 개별 모듈들 추가
add_subdirectory(Import)
add_subdirectory(Editor)

# 2. 통합 INTERFACE 생성
add_library(ToolSetLib INTERFACE)
add_library(Engine::Toolset ALIAS ToolSetLib)

# 3. 공통 인클루드 경로 제공
target_include_directories(ToolSetLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 4. 모든 하위 시스템 링크
target_link_libraries(ToolSetLib INTERFACE 
    Engine::Import
    Engine::Editor
)