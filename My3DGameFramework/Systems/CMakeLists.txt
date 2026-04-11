# 1. 하위 개별 모듈들 추가
#add_subdirectory(Audio)
add_subdirectory(Graphics)
#add_subdirectory(Physics)
#add_subdirectory(Scripting)

# 2. 통합 INTERFACE 생성
add_library(Systems INTERFACE)
add_library(Engine::Systems ALIAS Systems)

# 3. 공통 인클루드 경로 제공
target_include_directories(Systems INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 4. 모든 하위 시스템 링크
target_link_libraries(Systems INTERFACE 
    #Engine::Audio
    Engine::Graphics
    #Engine::Physics
    #Engine::Scripting
)
