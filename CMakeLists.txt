cmake_minimum_required(VERSION 3.25)

# 프로젝트 정보
project(My3DGameFramework VERSION 0.1.0 LANGUAGES CXX)

# C++17 표준 강제
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 폴더 정리 활성화
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 최종 빌드 경로 설정
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/Bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

# 컴파일러 옵션
if (MSVC)
    add_compile_definitions(NOMINMAX)
    add_compile_options(/bigobj) ### 이후에 비대해진 Engine.lib를 몇 개의 lib로 더 분산시킬 예정
    add_compile_options(/utf-8 /W4 /MP /permissive- /Zc:__cplusplus)
else()
    add_compile_options(-Wall -Wextra -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# 패키지 중앙 관리
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules")
message(STATUS "Finding Packages...")
find_package(spdlog CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Freetype CONFIG REQUIRED)
find_package(Jolt CONFIG REQUIRED)
find_package(recastnavigation CONFIG REQUIRED)
find_package(KTX CONFIG REQUIRED)
find_package(ryml CONFIG REQUIRED)
find_package(lua CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
message(STATUS "All Packages Found Successfully!")

# 하위 프로젝트 추가
add_subdirectory(AssetConverter)
add_subdirectory(My3DGameFramework)
#add_subdirectory(Game)
add_subdirectory(LibTesters)