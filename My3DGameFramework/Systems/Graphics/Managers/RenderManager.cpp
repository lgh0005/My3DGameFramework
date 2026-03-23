#include "GraphicsPch.h"
#include "RenderManager.h"

namespace MGF3D
{
	RenderManager::RenderManager() = default;
	RenderManager::~RenderManager() = default;

	bool RenderManager::Init()
	{
		// 1. 전역 그래픽 설정
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_STENCIL_TEST);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return true;
	}

	void RenderManager::Shutdown()
	{

	}
}