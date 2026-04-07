#include "CorePch.h"
#include "GLFWManager.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace MGF3D
{
	GLFWManager::GLFWManager() = default;
	GLFWManager::~GLFWManager() = default;

	bool GLFWManager::Init()
	{
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

	void GLFWManager::ErrorCallback(int32 err, cstr desc)
	{
		MGF_LOG_ERROR
		(
			"glfw Error [Code: {0}]: {1}", 
			err, 
			desc
		);
	}
}