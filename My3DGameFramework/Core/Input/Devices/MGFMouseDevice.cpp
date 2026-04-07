#include "CorePch.h"
#include "MGFMouseDevice.h"
#include "Input/Actions/MGFMouseAction.h"
#include "Managers/TypeManager.h"
#include "Identities/MGFTypeTree.h"
#include <GLFW/glfw3.h>

namespace MGF3D
{
	MGFMouseDevice::MGFMouseDevice(const String& name) : Super{ name } { }
	MGFMouseDevice::~MGFMouseDevice()
	{
		Shutdown();
	}

	/*================================//
	//    MGFMouseDevice Custom Type  //
	//================================*/
	int16 MGFMouseDevice::s_typeIndex = -1;
	const MGFType* MGFMouseDevice::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Device");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	bool MGFMouseDevice::Init()
	{
		// 초기화 시 특별히 할당이 필요한 자원이 있다면 여기서 처리
		return true;
	}

	void MGFMouseDevice::Update(GLFWwindow* window)
	{
		if (window == nullptr) return;

		// 1. 이전 프레임 상태 백업
		m_prevStates = m_currentStates;

		// 2. 등록된 모든 액션의 상태를 폴링(Polling)하여 업데이트
		for (auto& [hash, action] : m_actions)
			m_currentStates[hash] = action->CheckRawState(window);
	}

	void MGFMouseDevice::Shutdown()
	{
		m_actions.clear();
		m_currentStates.clear();
		m_prevStates.clear();

		for (auto& bindings : m_buttonBindings)
			bindings.clear();
	}

	bool MGFMouseDevice::GetButton(StringView actionName) const
	{
		StringHash hash{ actionName };
		auto it{ m_currentStates.find(hash) };
		return CommonUtils::Select(it != m_currentStates.end(), it->second, false);
	}

	bool MGFMouseDevice::GetButtonDown(StringView actionName) const
	{
		StringHash hash{ actionName };
		auto currIt{ m_currentStates.find(hash) };
		auto prevIt{ m_prevStates.find(hash) };

		if (currIt == m_currentStates.end() || prevIt == m_prevStates.end())
			return false;

		return currIt->second && !prevIt->second;
	}

	bool MGFMouseDevice::GetButtonUp(StringView actionName) const
	{
		StringHash hash{ actionName };
		auto currIt{ m_currentStates.find(hash) };
		auto prevIt{ m_prevStates.find(hash) };

		if (currIt == m_currentStates.end() || prevIt == m_prevStates.end())
			return false;

		return !currIt->second && prevIt->second;
	}

	void MGFMouseDevice::MapButton(StringView actionName, int32 button)
	{
		if (!CommonUtils::IsBetween(button, 0, GLFW_MOUSE_BUTTON_LAST)) return;

		StringHash hash{ actionName };

		if (m_actions.find(hash) == m_actions.end())
		{
			m_actions[hash] = std::make_unique<MGFMouseAction>(String(actionName));
			m_currentStates[hash] = false;
			m_prevStates[hash] = false;
		}

		m_actions[hash]->AddMouseButtonBinding(button);

		auto& bindings{ m_buttonBindings[button] };
		bool alreadyExists{ false };
		for (auto* actionPtr : bindings)  // find로 바꿀 수 없음?
		{
			if (actionPtr == m_actions[hash].get())
			{
				alreadyExists = true;
				break;
			}
		}

		if (alreadyExists == false)
			bindings.push_back(m_actions[hash].get());
	}

	void MGFMouseDevice::OnMouseButton(int32 button, int32 action)
	{
		if (!CommonUtils::IsBetween(button, 0, GLFW_MOUSE_BUTTON_LAST)) return;

		// GLFW 액션을 엔진의 InputEvent로 변환
		InputEvent evt;
		switch (action)
		{
		case GLFW_PRESS: evt = InputEvent::Pressed; break;
		case GLFW_RELEASE: evt = InputEvent::Released; break;
		default: return;
		}

		// 해당 키에 묶인 모든 액션의 콜백 실행
		const auto& actions{ m_buttonBindings[button] };
		for (auto* mouseAction : actions)
		{
			if (mouseAction != nullptr)
				mouseAction->Invoke(evt);
		}
	}

	void MGFMouseDevice::OnCursorPos(double x, double y)
	{
		m_mousePos.x = static_cast<float>(x);
		m_mousePos.y = static_cast<float>(y);
	}
}