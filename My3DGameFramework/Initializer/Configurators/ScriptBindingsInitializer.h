#pragma once

namespace MGF3D
{
	class ScriptBindingsInitializer
	{
		MGF_DECLARE_UTILITIES(ScriptBindingsInitializer)

	public:
		static bool Init();
		static void Shutdown();
	};
}