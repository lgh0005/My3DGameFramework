#include "GraphicsPch.h"
#include "GraphicsModule.h"
#include "Managers/VRAMManager.h"

namespace MGF3D
{
	GraphicsModule::GraphicsModule() = default;
	GraphicsModule::~GraphicsModule() = default;

	bool GraphicsModule::OnModuleInit()
	{
		// 0. 함수 포인터 자체가 유효한지 먼저 체크
		MGF_ASSERT
		(
			glGetString != nullptr, 
			"Graphics: GLAD function pointers are null! Check MGFWindow initialization."
		);

		// 1. 드라이버로부터 실제 정보를 획득
		cstr glVersion = (cstr)glGetString(GL_VERSION);
		cstr glRenderer = (cstr)glGetString(GL_RENDERER);
		if (glVersion && glRenderer)
		{
			MGF_LOG_INFO("GraphicsModule: OpenGL Context Verified.");
			MGF_LOG_INFO(" - Version : {0}", glVersion);
			MGF_LOG_INFO(" - Renderer: {0}", glRenderer);
		}
		else
		{
			MGF_LOG_ERROR("Graphics: Failed to retrieve OpenGL strings. Context might not be current.");
			return false;
		}

		// 2. VRAMManager 초기화
		MGF_INIT_SYS(MGF_VRAM.Init(StaticPoolSize, DynamicPoolSize), "VRAMManager");

		return true;
	}

	void GraphicsModule::OnShutDown()
	{
		MGF_VRAM.Shutdown();
	}
}