#include "GraphicsPch.h"
#include "FramebufferManager.h"

namespace MGF3D
{
	FramebufferManager::FramebufferManager() = default;
	FramebufferManager::~FramebufferManager() = default;

	bool FramebufferManager::Init()
	{
		glEnable(GL_FRAMEBUFFER_SRGB);
		
		return true;
	}

	void FramebufferManager::Shutdown()
	{

	}
}
