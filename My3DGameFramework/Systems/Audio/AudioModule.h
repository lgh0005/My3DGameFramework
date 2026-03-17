#pragma once
#include "Module/IModuleUnit.h"

namespace MGF3D
{
	class AudioModule : public IModuleUnit<AudioModule>
	{
	protected:
		AudioModule();
		virtual ~AudioModule();

	public:
		static bool OnModuleInit();
		static void OnShutDown();
	};
}