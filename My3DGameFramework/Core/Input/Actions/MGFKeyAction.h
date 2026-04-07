#pragma once
#include "Input/MGFInputAction.h"

namespace MGF3D
{
	MGF_CLASS_PTR(MGFKeyAction)
	class MGFKeyAction : public MGFInputAction
	{
		using Super = MGFInputAction;

	public:
		MGFKeyAction(const String& name);
		virtual ~MGFKeyAction() override;

	public:
		void AddKeyBinding(int32 keyCode);
		virtual bool IsMapped(int32 inputCode) const override;
		virtual bool CheckRawState(GLFWwindow* window) const override;

	public:
		const Vector<int32>& GetMappedKeys() const { return m_mappedKeys; }

	private:
		Vector<int32> m_mappedKeys;
	};
}