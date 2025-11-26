#include "EnginePch.h"
#include "Game.h"
#include "Core/Context.h"

Game::~Game() = default;
Context& Game::GetContext() { return *m_context; }

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
    INPUT.Init();
    TIME.Init();
    IMGUI.Init(true);

    // Context 생성
    m_context = Context::Create();
    if (!m_context)
    {
        SPDLOG_ERROR("failed to create context");
        WINDOW.DestroyWindow();
        return false;
    }

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
        m_context->Update();
        m_context->Render();

        // 인풋 업데이트
        INPUT.Update();

        // 버퍼 스왑
        glfwSwapBuffers(handle);
    }
}

void Game::Shutdown()
{
    // Context 정리
    m_context.reset();
     
    // spdlog 정리
    SPDLOG_INFO("Program terminated successfully.");
    spdlog::shutdown();

    // 매니저 정리
    IMGUI.ShutDown();

    // glfw 정리
    WINDOW.DestroyWindow();
}

