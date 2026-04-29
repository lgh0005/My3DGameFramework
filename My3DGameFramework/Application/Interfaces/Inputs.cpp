#include "ApplicationPch.h"
#include "Inputs.h"
#include "Input/Devices/MGFKeyboardDevice.h"
#include "Input/Devices/MGFMouseDevice.h"

namespace MGF3D
{
	void Inputs::MapKeyboard(const String& action, int32 key)
	{
		auto* keyboard = MGF_INPUT.GetDevice<MGFKeyboardDevice>();
		keyboard->MapKey(action, key);
	}

	void Inputs::MapMouse(const String& action, int32 button)
	{
		auto* mouse = MGF_INPUT.GetDevice<MGFMouseDevice>();
		mouse->MapButton(action, button);
	}

	bool Inputs::GetButton(const String& action)
	{
		return MGF_INPUT.GetDevice<MGFKeyboardDevice>()->GetButton(action) ||
			MGF_INPUT.GetDevice<MGFMouseDevice>()->GetButton(action);
	}

	bool Inputs::GetButtonDown(const String& action)
	{
		return MGF_INPUT.GetDevice<MGFKeyboardDevice>()->GetButtonDown(action) ||
			MGF_INPUT.GetDevice<MGFMouseDevice>()->GetButtonDown(action);
	}

	bool Inputs::GetButtonUp(const String& action)
	{
		return MGF_INPUT.GetDevice<MGFKeyboardDevice>()->GetButtonUp(action) ||
			MGF_INPUT.GetDevice<MGFMouseDevice>()->GetButtonUp(action);
	}

	vec2 Inputs::GetMousePosition()
	{
		return MGF_INPUT.GetDevice<MGFMouseDevice>()->GetMousePos();
	}

	vec2 Inputs::GetMouseDelta()
	{
		return MGF_INPUT.GetDevice<MGFMouseDevice>()->GetDelta();
	}
}