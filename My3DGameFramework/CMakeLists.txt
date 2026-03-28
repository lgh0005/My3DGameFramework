# 하위 모듈 추가

# Level 1: 가장 기초
add_subdirectory(Core)

# Level 2: 미들웨어 시스템들
add_subdirectory(Systems)

# Level 3: 미들웨어 초기화 및 glfw 컨텍스트 생성
add_subdirectory(Middleware)

# Level 4: 엔진 내부의 기본 컴포넌트와 리소스, 머캐닉, 에셋 정의
add_subdirectory(Framework)

# Level 5: 엔진 내부의 기본 컴포넌트와 리소스, 머캐닉, 에셋 타입 빌드 및 기본 설정
add_subdirectory(Initializer)

# Level 6: 핵심 게임 구성 요소들
#add_subdirectory(Environment/Rendering)
#add_subdirectory(Environment/Scene)

# Level 7 : 에셋 임포트 및 에디터
#add_subdirectory(Toolset/Editor)
#add_subdirectory(Toolset/Importer)

# Level 8: 엔진의 최종 통합 패키지
#add_subdirectory(Runtime)
