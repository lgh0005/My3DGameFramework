# 하위 모듈 추가

# Level 1: 가장 기초
add_subdirectory(Core)
add_subdirectory(Serializers)
add_subdirectory(Scripting)
add_subdirectory(Audio)

# Leve 2: Core 매니저 초기화 및 glfw 컨텍스트 생성
add_subdirectory(Bootstrap)

# Level 3: Core와 glfw에 의존하는 기초 시스템들
#add_subdirectory(Window)
#add_subdirectory(InputSystem)
#add_subdirectory(Time)

# Level 4: 윈도우/기초에 의존하는 그래픽 및 AI
#add_subdirectory(Physics)
#add_subdirectory(Graphics)
#add_subdirectory(AI)
#add_subdirectory(Editor)
#add_subdirectory(UI)

# Level 5: 모든 시스템을 조립하는 게임 로직 레이어
#add_subdirectory(Scene)

# Level 6: 에셋 변환
#add_subdirectory(Importers)

# Level 7: 엔진의 최종 통합 패키지
#add_subdirectory(Runtime)
