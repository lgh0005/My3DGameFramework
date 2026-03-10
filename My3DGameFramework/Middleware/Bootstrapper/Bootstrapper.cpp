#include "MiddlewarePch.h"
#include "Bootstrapper.h"

// Core module managers
#include "Managers/MemoryManager.h"
#include "Managers/NameManager.h"
#include "Managers/JobManager.h"
#include "Managers/PathManager.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/TaskManager.h"
#include "Managers/TypeManager.h"

// Systems module managers
//#include "Managers/AIManager.h"
//#include "Managers/AudioManager.h"
//#include "Managers/InstanceManager.h"
//#include "Managers/RenderManager.h"
//#include "Managers/PhysicsManager.h"
//#include "Managers/CoroutineManager.h"
//#include "Managers/LuaManager.h"
//#include "Managers/UIManager.h"

// glfw depended managers
#include "Managers/GLFWManager.h"
#include "Managers/WindowManager.h"
#include "Managers/InputManager.h"
#include "Managers/TimeManager.h"

namespace MGF3D
{
	bool Bootstrapper::Init()
	{
		// 0. 디버거 초기화
		MGF_LOG_INIT();
		MGF_TIMER_INIT();
		MGF_LOG_INFO("Bootstrapper : Engine started.");

		// 1. Core 단 매니저 초기화
		MGF_LOG_INFO("Bootstrapper : Initializing Core Systems.");
		MGF_INIT_SYS(MGF_MEMORY.Init(), "MemoryManager");
		MGF_INIT_SYS(MGF_JOB.Init(), "JobManager");
		MGF_INIT_SYS(MGF_PATH.Init(), "PathManager");

		// [TODO: 레거시 청산 후 주석 해제]
		// MGF_INIT_SYS(MGF_RESOURCE, "ResourceManager");
		// MGF_INIT_SYS(MGF_ENTITY,   "EntityManager");

		// 2. System 단 매니저 초기화
		// [TODO: 레거시 청산 후 주석 해제]
		// MGF_INIT_SYS(MGF_AI, "AIManager");
		// MGF_INIT_SYS(MGF_AUDIO, "AudioManager");
		// MGF_INIT_SYS(MGF_INSTANCE, "InstanceManager");
		// MGF_INIT_SYS(MGF_RENDER, "RenderManager");
		// MGF_INIT_SYS(MGF_PHYSICS, "PhysicsManager");
		// MGF_INIT_SYS(MGF_COROUTINE, "CoroutineManager");
		// MGF_INIT_SYS(MGF_LUA, "LuaManager");
		// MGF_INIT_SYS(MGF_UI, "UIManager");

		// 3. glfw 초기화
		MGF_LOG_INFO("Bootstrapper : Initializing glfw.");
		MGF_INIT_SYS(MGF_GLFW.Init(), "GLFWManager");

		// 4. Platform 단 초기화
		MGF_LOG_INFO("Bootstrapper : Initializing platform systems.");

		// [TODO: 레거시 청산 후 주석 해제]
		// MGF_INIT_SYS(MGF_TIME.Init(), "TimeManager");
		MGF_INIT_SYS(MGF_WINDOW.Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME), "WindowManager");
		// MGF_INIT_SYS(MGF_INPUT.Init(), "InputManager");

		return true;
	}

	void Bootstrapper::Shutdown()
	{
		// 1. System 단 종료
		MGF_LOG_INFO("Bootstrapper : Shutting down Systems.");

		// [TODO: 레거시 청산 후 주석 해제]
		//MGF_UI.Shutdown();
		//MGF_LUA.Shutdown();
		//MGF_COROUTINE.Shutdown();
		//MGF_PHYSICS.Shutdown();
		//MGF_RENDER.Shutdown();
		//MGF_INSTANCE.Shutdown();
		//MGF_AUDIO.Shutdown();
		//MGF_AI.Shutdown();

		// 2. Platform 단 종료
		MGF_LOG_INFO("Bootstrapper : Shutting down platform systems.");

		// [TODO: 레거시 청산 후 주석 해제]
		// MGF_INPUT.Shutdown();
		MGF_WINDOW.Shutdown();
		// MGF_TIME.Shutdown();

		// 3. glfw 종료
		MGF_LOG_INFO("Bootstrapper : Shutting down glfw.");
		MGF_GLFW.Shutdown();

		// 4. Core 단 종료
		MGF_LOG_INFO("Bootstrapper : Shutting down Core Systems.");

		// [TODO: 코어 모듈 레거시 청산 후 주석 해제]
		MGF_TASK.Shutdown();
		// MGF_ENTITY.Shutdown();
		// MGF_RESOURCE.Shutdown();
		// MGF_TYPE.Shutdown();
		MGF_NAME.Shutdown();

		MGF_PATH.Shutdown();
		MGF_JOB.Shutdown();
		MGF_MEMORY.Shutdown();

		// 5. 디버거 종료
		MGF_LOG_INFO("Bootstrapper : Engine Ended.");
		MGF_LOG_FLUSH();
		MGF_LOG_CLEAR();
	}
}