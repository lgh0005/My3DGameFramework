#include "EnginePch.h"
#include "Game.h"

Game::~Game() = default;

GameUPtr Game::Create()
{
    auto game = GameUPtr(new Game());
    if (!game->Init()) return nullptr;
    return game;
}

bool Game::Init()
{
    // 윈도우 생성과 윈도우 매니저 초기화
    SPDLOG_INFO("Start Program!");
    if (!WINDOW.Init()) return false;
    auto handle = WINDOW.GetWindow();

    // 초기화가 필요한 다른 매니저들 초기화
    RENDER.Init();

    INPUT.Init();
    TIME.Init();
    IMGUI.Init(true);

    // 첫 씬 로드
    SCENE.LoadScene("DevScene", "Standard");

    return true;
}

void Game::Update()
{
    auto handle = WINDOW.GetWindow();
    while (!glfwWindowShouldClose(handle))
    {
        glfwPollEvents();

        // 윈도우가 최소화된 상태라면 건너뛰기
        if (WINDOW.IsInconified()) continue;

        // 메니저 업데이트
        TIME.Update();

        // 컨텍스트 업데이트
        // 2. 씬 로직 및 렌더링 (Context::Update/Render 통합)
        if (auto scene = SCENE.GetActiveScene())
        {
            // 게임 로직 업데이트
            scene->Update();

            // 렌더링 수행 (RenderManager에게 위임)
            RENDER.Render(scene);
        }
        else
        {
            // 씬이 없거나 로딩 중일 때 기본 화면 클리어
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        // 인풋 업데이트
        INPUT.Update();

        // 버퍼 스왑
        glfwSwapBuffers(handle);
    }
}

void Game::Shutdown()
{
    // 1. 렌더러 정리 (가장 중요! OpenGL 컨텍스트 살아있을 때)
    RENDER.Clear();

    // 매니저 정리
    IMGUI.ShutDown();
    RESOURCE.Clear();
    
    // glfw 정리
    WINDOW.DestroyWindow();

    // spdlog 정리
    SPDLOG_INFO("Program terminated successfully.");
    spdlog::shutdown();
}

