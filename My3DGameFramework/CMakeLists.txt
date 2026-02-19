# 하위 모듈 추가

# Level 1: 가장 기초
add_subdirectory(Core)
add_subdirectory(Debug)
add_subdirectory(Serializers)
add_subdirectory(Scripting)
add_subdirectory(Bootstrap)

# Level 2: Core에 의존하는 기초 시스템들
add_subdirectory(Window)
add_subdirectory(Object)
add_subdirectory(InputSystem)
add_subdirectory(FileSystem)
add_subdirectory(ResourceSystem)

# Level 3: 윈도우/기초에 의존하는 그래픽 및 AI
add_subdirectory(Physics)
add_subdirectory(Graphics)
add_subdirectory(AI)
add_subdirectory(Audio)
add_subdirectory(Editor)
add_subdirectory(UI)

# Level 4: 모든 시스템을 조립하는 게임 로직 레이어
add_subdirectory(Scene)

# Level 5: 에셋 변환
add_subdirectory(Importers)

# Level 6: 엔진의 최종 통합 패키지
add_subdirectory(AppFramework)
