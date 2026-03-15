#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(IRaycastTarget)
	class IRaycastTarget
	{
	public:
		virtual ~IRaycastTarget();
		virtual void* GetTargetData() const = 0;
	};
}