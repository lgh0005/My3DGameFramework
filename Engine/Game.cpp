#include "EnginePch.h"
#include "Game.h"
#include "Graphics/Context.h"
#include "Components/Camera.h"

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
	SPDLOG_INFO("Start Program!");

	// glfw 라이브러리 초기화, 실패하면 에러 출력후 종료
    if (!glfwInit())
    {
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);
        return false;
    }

    // OpenGL 버전 설정
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw 윈도우 생성
    SPDLOG_INFO("Create glfw window");
    m_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!m_window)
    {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this); // Game 클래스에 glfw 윈도우 핸들 관리 책임 위임
    glfwSwapInterval(0); // v-sync 잠시 끄기

    // glad를 활용한 OpenGL 함수 로딩
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return false;
    }
    auto glVersion = glGetString(GL_VERSION);
    SPDLOG_INFO("OpenGL context version: {}", reinterpret_cast<const char*>(glVersion));

    // Context 생성
    m_context = Context::Create();
    if (!m_context)
    {
        SPDLOG_ERROR("failed to create context");
        glfwTerminate();
        return false;
    }

    // TODO : 이후에 Window 매니저 클래스가 등록해야 한다.
    // glfw 콜백 함수 등록
    OnFramebufferSizeChange(m_window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(m_window, OnFramebufferSizeChange);
    glfwSetWindowIconifyCallback(m_window, OnWindowIconified);
    glfwSetKeyCallback(m_window, OnKeyEvent);
    glfwSetCursorPosCallback(m_window, OnCursorPos);
    glfwSetMouseButtonCallback(m_window, OnMouseButton);

    // SPIR-V 확장 지원 체크
    if (glfwExtensionSupported("GL_ARB_gl_spirv")) SPDLOG_INFO("SPIR-V supported!");
    else SPDLOG_WARN("SPIR-V not supported on this system!");

    // 블렌딩 활성화
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 매니저들 초기화
    WINDOW.Init(m_window);
    TIME.Init();
    IMGUI.Init(m_window, true);

    return true;
}

void Game::Update()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();

        // 윈도우가 최소화된 상태라면 건너뛰기
        if (WINDOW.IsInconified()) continue;

        // 메니저 업데이트
        TIME.Update();

        // 컨텍스트 업데이트
        m_context->ProcessInput(m_window);
        m_context->Update();
        m_context->Render();

        glfwSwapBuffers(m_window);
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
    glfwTerminate();
    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
}

// TEMP : 임시적으로 Game 클래스가 직접 콜백 함수를 등록하도록 구현
/*======================//
//   default callbacks  //
//======================*/
#pragma region DEFAULT_CALLBACKS
void Game::HandleFramebufferSizeChange(int32 width, int32 height)
{
    WINDOW.HandleFramebufferSizeChange(*m_context, width, height);
}

void Game::HandleWindowIconified(int32 iconified)
{
    WINDOW.HandleWindowIconified(iconified);
}

void Game::HandleKeyEvent(int32 key, int32 scancode, int32 action, int32 mods)
{
    INPUT.HandleKeyEvent(key, scancode, action, mods);
}

void Game::HandleCursorMove(double x, double y)
{
    INPUT.HandleCursorMove(*m_context, x, y);
}

void Game::HandleMouseButton(int32 button, int32 action, int32 mod)
{
    INPUT.HandleMouseButton(*m_context, button, action, mod);
}

#pragma endregion

// TEMP : 이후에 콜백을 넘기는 방식을 좀 더 고려 필요
/*====================//
//   static wrappers  //
//====================*/
#pragma region STATIC_WRAPPERS
void Game::OnFramebufferSizeChange(GLFWwindow* window, int32 width, int32 height)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (game) game->HandleFramebufferSizeChange(width, height);
}

void Game::OnWindowIconified(GLFWwindow* window, int32 iconified)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (game) game->HandleWindowIconified(iconified);
}

void Game::OnKeyEvent(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (game) game->HandleKeyEvent(key, scancode, action, mods);
}

void Game::OnCursorPos(GLFWwindow* window, double x, double y)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (game) game->HandleCursorMove(x, y);
}

void Game::OnMouseButton(GLFWwindow* window, int32 button, int32 action, int32 mod)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (game) game->HandleMouseButton(button, action, mod);
}

#pragma endregion