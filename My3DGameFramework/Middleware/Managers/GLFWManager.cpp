#include "MiddlewarePch.h"
#include "GLFWManager.h"
#include "Utils/GLFWAllocateHelper.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace MGF3D
{
	GLFWManager::GLFWManager() = default;
	GLFWManager::~GLFWManager() = default;

	bool GLFWManager::Init()
	{
		// 0. GLFW 커스텀 할당자 등록
		GLFWallocator allocator;
		allocator.allocate = GLFWAllocateHelper::GLFW_Allocate;
		allocator.reallocate = GLFWAllocateHelper::GLFW_Reallocate;
		allocator.deallocate = GLFWAllocateHelper::GLFW_Deallocate;
		allocator.user = nullptr;
		glfwInitAllocator(&allocator);

		// 1. 에러 콜백함수 등록
		glfwSetErrorCallback(ErrorCallback);

		// 2. glfw 초기화
		if (!glfwInit())
		{
			MGF_LOG_FATAL("glfw: Failed to initialize GLFW!");
			return false;
		}

		MGF_LOG_INFO("glfw: GLFW Initialized Successfully.");
		return true;
	}

	void GLFWManager::Shutdown()
	{
		glfwTerminate();
		MGF_LOG_INFO("glfw: GLFW Terminated.");
	}

	void GLFWManager::Update()
	{
		glfwPollEvents();
	}

	double GLFWManager::GetTime()
	{
		return glfwGetTime();
	}

	void GLFWManager::ErrorCallback(int32 err, cstr desc)
	{
		MGF_LOG_ERROR("glfw Error [Code: {0}]: {1}", err, desc);
	}
}