#include "InitializerPch.h"
#include "InitializerModule.h"

// Configurators
#include "Configurators/CollisionPhaseConfigurator.h"
#include "Configurators/ScriptBindingsInitializer.h"

namespace MGF3D
{
	InitializerModule::InitializerModule() = default;
	InitializerModule::~InitializerModule() = default;

	bool InitializerModule::OnModuleInit()
	{
		// 1. 엔진 게임 어휘 모듈 초기화
		MGF_MODULE_INIT(AssetModule::Init(), "ResourceModule");
		MGF_MODULE_INIT(ResourceModule::Init(), "ResourceModule");
		// MGF_MODULE_INIT(MechanicModule::Init(), "ResourceModule");
		// MGF_MODULE_INIT(ComponentModule::Init(), "ResourceModule");

		// 2. 엔진 기본 설정 초기화
		// MGF_INIT_SYS(CollisionPhaseConfigurator::Init(), "CollisionPhaseConfigurator");
		// MGF_INIT_SYS(ScriptBindingsInitializer::Init(), "ScriptBindingsInitializer");

		return true;
	}

	void InitializerModule::OnShutDown()
	{
		// MGF_SHUTDOWN_SYS(ScriptBindingsInitializer::Shutdown(), "ScriptBindingsInitializer");
		// MGF_SHUTDOWN_SYS(CollisionPhaseConfigurator::Shutdown(), "CollisionPhaseConfigurator");

		// MGF_MODULE_SHUTDOWN(ComponentModule::Shutdown(), "ComponentModule");
		// MGF_MODULE_SHUTDOWN(MechanicModule::Shutdown(), "MechanicModule");
		MGF_MODULE_SHUTDOWN(ResourceModule::Shutdown(), "ResourceModule");
		MGF_MODULE_SHUTDOWN(AssetModule::Shutdown(), "AssetModule");
	}
}