#include "EnginePch.h"
#include "Application.h"
#include "Scene/Scene.h"
#include "Graphics/RenderPipeline.h"

Application::Application() = default;
Application::~Application() { Shutdown(); }

bool Application::Init(int32 width, int32 height, const std::string& title)
{
	// 0. Logger 초기화
	LOGGER.Init();
	LOG_INFO("Application Started [Start]");
	LOG_INFO("Engine Initialization Started...");

	// 1. 윈도우 생성 (가장 먼저 되어야 함)
	if (!WINDOW.Init(width, height, title))
	{
		LOG_ERROR("Window Init Failed!");
		return false;
	}

	// 2. 핵심 코어 시스템 초기화
	if (!RENDER.Init()) return false;    // OpenGL 컨텍스트 등 로드
	INPUT_MGR.Init();					 // 입력 시스템
	TIME.Init();						 // 타이머
	PHYSICS.Init();						 // 물리
	AUDIO.Init();						 // 오디오 시스템
	IMGUI.Init(true);					 // 디버그 UI (Window가 있어야 가능)

	// 3. 레벨들을 등록
	OnInit();

	return true;
}

void Application::Run(const std::string& startLevelName)
{
	// [Start] 프로그램 시작 확인 문구
	LOG_INFO("Start Program!");

	// 0. 첫 시작할 레벨이 등록되었는 지 확인
	if (m_levelMap.find(startLevelName) == m_levelMap.end())
	{
		LOG_ERROR("Failed to find level: {}", startLevelName);
		return;
	}

	LevelInfo& info = m_levelMap[startLevelName];
	SCENE.LoadScene(info.sceneKey, info.pipelineKey);

	// 2. 메인 루프
	auto windowHandle = WINDOW.GetWindow();
	while (!glfwWindowShouldClose(windowHandle))
	{
		// 이벤트 폴링
		glfwPollEvents();

		// 윈도우가 최소화된 상태라면 건너뛰기
		if (WINDOW.IsInconified()) continue;

		// 타이머 업데이트
		TIME.Update();

		// 물리 시뮬레이션 (고정 시간 업데이트)
		while (TIME.CheckFixedUpdate()) PHYSICS.Update();

		// 컨텍스트 업데이트
		if (auto scene = SCENE.GetActiveScene())
		{
			// 게임 로직 업데이트
			scene->Update();
			scene->LateUpdate();

			// 렌더링 수행 (RenderManager에게 위임)
			RENDER.Render(scene);

			// 파괴 예약 오브젝트를 파괴
			scene->ProcessPendingKills();
		}
		else
		{
			// 씬이 없거나 로딩 중일 때 기본 화면 클리어
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		// 인풋 업데이트
		INPUT_MGR.Update();

		// 버퍼 스왑
		glfwSwapBuffers(windowHandle);
	}
}

void Application::Update()
{
	// 현재 활성화된 씬이 있다면 업데이트
	if (auto scene = SCENE.GetActiveScene())
	{
		scene->Update();
	}
}

void Application::Render()
{
	// 씬이 존재하면 렌더 매니저에게 위임
	if (auto scene = SCENE.GetActiveScene())
	{
		RENDER.Render(scene);
	}
	else
	{
		// 씬 로딩 중이거나 없을 때 기본 화면 (검은색/회색 등)
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void Application::Shutdown()
{
	// 중복 종료 방지
	static bool isShutdown = false;
	if (isShutdown) return;
	isShutdown = true;

	LOG_INFO("Engine Shutdown Started...");

	SCENE.Clear();      // 씬/게임 오브젝트 정리
	PHYSICS.Clear();    // 물리 엔진 정리
	RENDER.Clear();     // 렌더 리소스/셰이더 정리

	IMGUI.Clear();
	AUDIO.Clear();
	RESOURCE.Clear();   // 로드된 텍스처/모델 등 정리

	WINDOW.DestroyWindow(); // 윈도우 파괴

	LOG_INFO("Program terminated successfully.");
	LOGGER.Clear();
}