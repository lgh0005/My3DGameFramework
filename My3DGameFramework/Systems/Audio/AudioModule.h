#pragma once
#include "Module/IModuleInterface.h"

namespace MGF3D
{
	class AudioModule : public IModuleInterface<AudioModule>
	{
		MGF_DECLARE_UTILITIES(AudioModule)

	public:
		static void OnRegisterTypes();
		static bool OnInit();
		static bool OnShutdown();
	};
}