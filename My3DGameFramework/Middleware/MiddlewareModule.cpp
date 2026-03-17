#include "MiddlewarePch.h"
#include "MiddlewareModule.h"
#include "Managers/GLFWManager.h"
#include "Managers/TimeManager.h"
#include "Managers/WindowManager.h"
#include "Managers/InputManager.h"

namespace MGF3D
{
	MiddlewareModule::MiddlewareModule() = default;
	MiddlewareModule::~MiddlewareModule() = default;

	bool MiddlewareModule::OnModuleInit()
	{
		MGF_LOG_INFO("[Middleware] Starting Middleware Module (Platform Systems)...");
		
		MGF_INIT_SYS(MGF_GLFW.Init(), "GLFWManager");
		MGF_INIT_SYS(MGF_TIME.Init(), "TimeManager");
		MGF_INIT_SYS(MGF_WINDOW.Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME), "WindowManager");
		MGF_INIT_SYS(MGF_INPUT.Init(MGF_WINDOW.GetNativeWindow()), "InputManager");

		MGF_LOG_INFO("[Middleware] Middleware Module booted successfully.");
		return true;
	}

	void MiddlewareModule::OnShutDown()
	{
		MGF_LOG_INFO("[Middleware] Shutting down platform systems.");

		MGF_SHUTDOWN_SYS(MGF_INPUT.Shutdown(), "InputManager");
		MGF_SHUTDOWN_SYS(MGF_WINDOW.Shutdown(), "WindowManager");
		MGF_SHUTDOWN_SYS(MGF_TIME.Shutdown(), "TimeManager");
		MGF_SHUTDOWN_SYS(MGF_GLFW.Shutdown(), "GLFWManager");

		MGF_LOG_INFO("[Middleware] Middleware Module shutdown completed.");
	}
}