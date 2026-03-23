#pragma once
#include "Module/IModuleUnit.h"

namespace MGF3D
{
	class GraphicsModule : public IModuleUnit<GraphicsModule>
	{
	protected:
		GraphicsModule();
		virtual ~GraphicsModule();

	public:
		static bool OnModuleInit();
		static void OnShutDown();

	private:
		static bool VerifyOpenGLContext();
	};
}