# 1. 하위 개별 모듈들 추가
# add_subdirectory(Mechanics)
add_subdirectory(Resources)
add_subdirectory(Assets)
add_subdirectory(Components)

# 2. 통합 INTERFACE 생성
add_library(FrameworkLib INTERFACE)
add_library(Engine::Framework ALIAS FrameworkLib)

# 3. 공통 인클루드 경로 제공
target_include_directories(FrameworkLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 4. 하위 시스템 링크
target_link_libraries(FrameworkLib INTERFACE
	Engine::Assets
	Engine::Resources
	#Engine::Components
	#Engine::Mechanics
)