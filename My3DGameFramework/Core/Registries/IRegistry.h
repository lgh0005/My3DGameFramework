#pragma once

namespace MGF3D
{
	class IRegistry
	{
	public:
		IRegistry();
		virtual ~IRegistry();

	public:
		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;
		virtual void Clear() = 0;
	};
}