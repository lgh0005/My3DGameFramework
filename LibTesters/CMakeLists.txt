# LibTesters/CMakeLists.txt

# 1. 소스 파일 목록 정의
set(LIB_TEST_SOURCE
    "main.cpp"
    "pch.h"
)

# 2. Visual Studio 필터 자동 생성
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIB_TEST_SOURCE})

# 3. 실행 파일 생성 (타겟 이름을 LibTester로 변경)
# 이렇게 하면 기본적으로 LibTester.exe가 생성됩니다.
add_executable(LibTester ${LIB_TEST_SOURCE})

# 4. 콘솔 앱 설정 (std::cout 출력 확인용)
set_target_properties(LibTester PROPERTIES WIN32_EXECUTABLE FALSE)

# 5. Include 및 PCH 설정
target_precompile_headers(LibTester PRIVATE pch.h)
target_include_directories(LibTester PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 6. 라이브러리 링크
# Engine::Core의 기능을 사용하기 위해 링크합니다.
target_link_libraries(LibTester 
    PRIVATE 
    Engine::Core
)