#include "CorePch.h"
#include "CoreModule.h"
#include "Managers/MemoryManager.h"
#include "Managers/JobManager.h"
#include "Managers/PathManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Managers/TaskManager.h"
#include "Managers/StreamManager.h"
#include "Managers/NameManager.h"

namespace MGF3D
{
	CoreModule::CoreModule() = default;
	CoreModule::~CoreModule() = default;

	bool CoreModule::OnModuleInit()
	{
		// 0. 디버거 초기화
		MGF_LOG_INIT();
		MGF_TIMER_INIT();
		MGF_LOG_INFO("[CoreModule] Booting up Core Subsystems...");

		// 1. Core 단 매니저 초기화
		MGF_INIT_SYS(MGF_MEMORY.Init(), "MemoryManager");
		MGF_INIT_SYS(MGF_JOB.Init(), "JobManager");
		MGF_INIT_SYS(MGF_STREAM.Init(), "StreamManager");
		MGF_INIT_SYS(MGF_PATH.Init(), "PathManager");
		MGF_INIT_SYS(MGF_ASSET.Init(), "AssetManager");
		// [TODO: 레거시 청산 후 주석 해제]
		// MGF_INIT_SYS(MGF_ENTITY,   "EntityManager");

		MGF_LOG_INFO("[CoreModule] Core Subsystems initialized successfully.");
		return true;
	}

	void CoreModule::OnShutDown()
	{
		MGF_LOG_INFO("[CoreModule] Shutting down Core Subsystems...");

		// 1. Core 단 매니저 해제
		// [TODO: 코어 모듈 레거시 청산 후 주석 해제]
		MGF_SHUTDOWN_SYS(MGF_TASK.Shutdown(), "TaskManager");
		//MGF_SHUTDOWN_SYS(MGF_ASSET.Shutdown(), "AssetManager");
		//MGF_SHUTDOWN_SYS(MGF_RESOURCE.Shutdown(), "ResourceManager");
		MGF_SHUTDOWN_SYS(MGF_NAME.Shutdown(), "NameManager");
		// MGF_SHUTDOWN_SYS(MGF_ENTITY.Shutdown(), "EntityManager");
		// MGF_SHUTDOWN_SYS(MGF_TYPE.Shutdown(), "TypeManager");

		MGF_SHUTDOWN_SYS(MGF_PATH.Shutdown(), "PathManager");
		MGF_SHUTDOWN_SYS(MGF_STREAM.Shutdown(), "StreamManager");
		MGF_SHUTDOWN_SYS(MGF_JOB.Shutdown(), "JobManager");
		MGF_SHUTDOWN_SYS(MGF_MEMORY.Shutdown(), "MemoryManager");

		// 2. 디버거 종료
		MGF_LOG_INFO("[CoreModule] Engine Ended.");
		MGF_LOG_FLUSH();
		MGF_LOG_CLEAR();
	}
}