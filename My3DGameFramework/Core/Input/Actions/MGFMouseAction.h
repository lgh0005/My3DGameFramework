#pragma once
#include "Input/MGFInputAction.h"

namespace MGF3D
{
	MGF_CLASS_PTR(MGFMouseAction)
	class MGFMouseAction : public MGFInputAction
	{
		using Super = MGFInputAction;

	public:
		MGFMouseAction(const String& name);
		virtual ~MGFMouseAction() override;

	public:
		void AddMouseButtonBinding(int32 button);
		virtual bool IsMapped(int32 inputCode) const override;
		virtual bool CheckRawState(GLFWwindow* window) const override;

	public:
		const Vector<int32>& GetMappedMouseButtons() const { return m_mappedMouseButtons; }

	private:
		Vector<int32> m_mappedMouseButtons;
	};
}