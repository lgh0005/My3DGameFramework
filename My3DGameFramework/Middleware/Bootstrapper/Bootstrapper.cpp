#include "MiddlewarePch.h"
#include "Bootstrapper.h"

// 각 모듈의 구체적인 헤더들
#include "CoreModule.h"
#include "PhysicsModule.h"
#include "AudioModule.h"
#include "MiddlewareModule.h"

namespace MGF3D
{
	bool Bootstrapper::Init()
	{
		// 1. Core Module
		MGF_MODULE_INIT(CoreModule::Init(), "CoreModule");

		// 2. System Modules
		MGF_MODULE_INIT(PhysicsModule::Init(), "PhysicsModule");
		MGF_MODULE_INIT(AudioModule::Init(), "AudioModule");

		// 3. Middleware Module
		MGF_MODULE_INIT(MiddlewareModule::Init(), "MiddlewareModule");

		MGF_LOG_INFO("[BOOT] MGF3D Engine is now fully operational.");
		return true;
	}

	void Bootstrapper::Shutdown()
	{
		MGF_LOG_INFO("[EXIT] Initiating Engine Shutdown Sequence...");

		// 1. 플랫폼 레이어 정리 (윈도우 닫기 등)
		MGF_MODULE_SHUTDOWN(MiddlewareModule::Shutdown(), "MiddlewareModule");

		// 2. 시스템 레이어 정리
		MGF_MODULE_SHUTDOWN(AudioModule::Shutdown(), "AudioModule");
		MGF_MODULE_SHUTDOWN(PhysicsModule::Shutdown(), "PhysicsModule");

		// 3. 최하위 기초 레이어 정리 (메모리 해제 등)
		MGF_MODULE_SHUTDOWN(CoreModule::Shutdown(), "CoreModule");

		MGF_LOG_INFO("[EXIT] MGF3D Engine terminated safely.");
	}
}