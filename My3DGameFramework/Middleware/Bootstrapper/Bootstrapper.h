#pragma once

namespace MGF3D
{
	class Bootstrapper
	{
		MGF_DECLARE_UTILITIES(Bootstrapper)

	public:
		static bool Init();
		static void Shutdown();
	};
}
