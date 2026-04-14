#pragma once
#include "Registries/IRegistry.h"

namespace MGF3D
{
	MGF_CLASS_PTR(IComponentRegistry)
	class IComponentRegistry : public IRegistry
	{
	public:
		virtual ~IComponentRegistry() override;
	};
}