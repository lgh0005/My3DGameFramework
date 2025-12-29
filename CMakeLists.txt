cmake_minimum_required(VERSION 3.20)
project(My3DGameFramework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/utf-8)
else()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

add_subdirectory(AssetConverter)
add_subdirectory(Engine)
add_subdirectory(Game)