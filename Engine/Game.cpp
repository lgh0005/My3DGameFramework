#include "EnginePch.h"
#include "Game.h"
#include "Graphics/Context.h"

Game::~Game() = default;

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

    // glfw 콜백 함수 등록
    OnFramebufferSizeChange(m_window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(m_window, OnFramebufferSizeChange);
    glfwSetKeyCallback(m_window, OnKeyEvent);

    // SPIR-V 확장 지원 체크
    if (glfwExtensionSupported("GL_ARB_gl_spirv")) SPDLOG_INFO("SPIR-V supported!");
    else SPDLOG_WARN("SPIR-V not supported on this system!");

    // 블렌딩 활성화
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void Game::Update()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();

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

    // glfw 정리
    glfwTerminate();
    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
}