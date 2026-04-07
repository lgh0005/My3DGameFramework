#pragma once
#include "Input/MGFInputDevice.h"

namespace MGF3D
{
	MGF_CLASS_PTR(MGFKeyAction)

	MGF_CLASS_PTR(MGFKeyboardDevice)
	class MGFKeyboardDevice : public MGFInputDevice
	{
		using Super = MGFInputDevice;

	public:
		MGFKeyboardDevice(const String& name);
		virtual ~MGFKeyboardDevice() override;

	/*====================================//
	//    MGFKeyboardDevice Custom Type   //
	//====================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		virtual bool Init() override;
		virtual void Update(GLFWwindow* window) override;
		virtual void Shutdown() override;

	public:
		virtual bool GetButton(StringView actionName) const override;
		virtual bool GetButtonDown(StringView actionName) const override;
		virtual bool GetButtonUp(StringView actionName) const override;

	public:
		void MapKey(StringView actionName, int32 keyCode);
		void OnKey(int32 key, int32 action);

	private:
		HashMap<StringHash, MGFKeyActionUPtr> m_actions;
		HashMap<StringHash, bool> m_currentStates;
		HashMap<StringHash, bool> m_prevStates;
		Array<Vector<MGFKeyAction*>, 512> m_keyBindings;
	};
}