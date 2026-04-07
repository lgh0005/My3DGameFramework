#pragma once

namespace MGF3D
{
	template <typename T>
	class IModuleInterface
	{
		MGF_DECLARE_UTILITIES(IModuleInterface)

	public:
		static void RegisterTypes() { T::OnRegisterTypes(); }
		static bool Init() { return T::OnInit(); }
		static bool Shutdown() { return T::OnShutdown(); }
	};
}