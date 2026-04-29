#pragma once
#include "Input/MGFInputDevice.h"

namespace MGF3D
{
	MGF_CLASS_PTR(MGFMouseAction)

	MGF_CLASS_PTR(MGFMouseDevice)
	class MGFMouseDevice : public MGFInputDevice
	{
		using Super = MGFInputDevice;

	public:
		MGFMouseDevice();
		virtual ~MGFMouseDevice() override;

	/*================================//
	//    MGFMouseDevice Custom Type  //
	//================================*/
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
		void MapButton(StringView actionName, int32 button);
		void OnMouseButton(int32 button, int32 action);
		void OnCursorPos(double x, double y);
		const vec2& GetMousePos() const { return m_mousePos; }
		const vec2& GetDelta() const { return m_mouseDelta; }

	private:
		Array<Vector<MGFMouseAction*>, 16> m_buttonBindings;
		HashMap<StringHash, MGFMouseActionUPtr> m_actions;
		HashMap<StringHash, bool> m_currentStates;
		HashMap<StringHash, bool> m_prevStates;

		vec2 m_mousePos{ 0.0f, 0.0f };
		vec2 m_prevMousePos{ 0.0f, 0.0f };
		vec2 m_mouseDelta{ 0.0f, 0.0f }; 
	};
}
