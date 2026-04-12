#pragma once

namespace MGF3D
{
	class IReistry
	{
	public:
		virtual ~IRegistry();

	public:
		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;
		virtual void Clear() = 0;
	};
}