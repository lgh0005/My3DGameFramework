#pragma once

namespace MGF3D
{
	template <typename Module>
	class IModuleUnit
	{
	protected:
		IModuleUnit();
		virtual ~IModuleUnit();

	public:
		static bool Init();
		static void Shutdown();
	};
}

#include "Module/IModuleUnit.inl"