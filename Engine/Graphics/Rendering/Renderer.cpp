#include "EnginePch.h"
#include "Renderer.h"
#include "Scene/Scene.h"
#include "Graphics/Rendering/RenderPipeline.h"

DECLARE_DEFAULTS_IMPL(Renderer)

RendererUPtr Renderer::Create()
{
	auto renderer = RendererUPtr(new Renderer());
	if (!renderer->Init()) return nullptr;
	return std::move(renderer);
}

bool Renderer::Init()
{
	// GLFW에서 모니터 정보 추출 및 주사율 주입
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	if (!mode) return false;
	int32 hz = mode->refreshRate;
	SetRefreshRate(hz);

	// 기본 OpenGL 상태 설정 (초기화)
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	return true;
}

void Renderer::Render(Scene* scene)
{
	// 파이프라인이 아직 세팅되지 않았으면 무시 (안전장치)
	if (m_pipeline)
	{
		m_pipeline->Render(scene);
	}
	else
	{
		// 파이프라인이 없을 때 화면 클리어
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void Renderer::OnResize(int32 width, int32 height)
{
	m_pipeline->OnResize(width, height);
}

void Renderer::SetPipeline(RenderPipelineUPtr pipeline)
{
	m_pipeline = std::move(pipeline);
}

void Renderer::SetRefreshRate(int32 refreshRate)
{
	if (refreshRate <= 0) refreshRate = 60;
	m_refreshRate = refreshRate;
	m_targetShutterSpeed = 1.0f / static_cast<float>(m_refreshRate);
}

void Renderer::SetTargetShutterSpeed(float shutterSpeed)
{
	// 0이나 음수가 들어오면 곤란하니 방어 코드
	if (shutterSpeed <= 0.0001f)
	{
		m_targetShutterSpeed = 1.0f / 60.0f; // 기본값 복구
		return;
	}
	m_targetShutterSpeed = shutterSpeed;
}
