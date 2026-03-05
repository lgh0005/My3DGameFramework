#pragma once

namespace MGF3D
{
	class IModuleUnit
	{
	public:
		virtual ~IModuleUnit();
		virtual void OnModuleInit() = 0;
		virtual void OnShutDown() = 0;
	};
}