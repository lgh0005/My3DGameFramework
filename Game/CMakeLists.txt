# 1. "Game"이라는 이름의 실행 파일(.exe)을 만듭니다.
add_executable(Game

    pch.h pch.cpp 
    main.cpp
)

# 2. [PCH 설정] GamePCH.h를 Game 타겟 전용(PRIVATE) PCH로 사용합니다.
target_precompile_headers(Game PRIVATE pch.h)

# 3. [링크 설정] Game 타겟은 Engine 라이브러리가 필요합니다.
target_link_libraries(Game PRIVATE Engine)

# 4. Shaders 폴더를 빌드 폴더로 복사 (이미 복사된 Shaders 폴더를 지우고 다시 복사)
file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/Resources)
file(COPY ${CMAKE_SOURCE_DIR}/Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})