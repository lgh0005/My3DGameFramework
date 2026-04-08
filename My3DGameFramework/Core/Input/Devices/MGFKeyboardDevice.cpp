#include "CorePch.h"
#include "MGFKeyboardDevice.h"
#include "Input/Actions/MGFKeyAction.h"
#include "Managers/TypeManager.h"
#include "Identities/MGFTypeTree.h"

namespace MGF3D
{
	MGFKeyboardDevice::MGFKeyboardDevice() = default;
	MGFKeyboardDevice::~MGFKeyboardDevice()
	{
		Shutdown();
	}

	bool MGFKeyboardDevice::Init()
	{
		// 초기화 시 특별히 할당이 필요한 자원이 있다면 여기서 처리
		return true;
	}

	/*====================================//
	//    MGFKeyboardDevice Custom Type   //
	//====================================*/
	int16 MGFKeyboardDevice::s_typeIndex = -1;
	const MGFType* MGFKeyboardDevice::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Device");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void MGFKeyboardDevice::Update(GLFWwindow* window)
	{
		if (window == nullptr) return;

		// 1. 이전 프레임 상태 백업
		m_prevStates = m_currentStates;

		// 2. 등록된 모든 액션의 상태를 폴링(Polling)하여 업데이트
		for (auto& [hash, action] : m_actions)
			m_currentStates[hash] = action->CheckRawState(window);
	}

	void MGFKeyboardDevice::Shutdown()
	{
		m_actions.clear();
		m_currentStates.clear();
		m_prevStates.clear();

		for (auto& bindings : m_keyBindings)
			bindings.clear();
	}

	bool MGFKeyboardDevice::GetButton(StringView actionName) const
	{
		StringHash hash{ actionName };
		auto it { m_currentStates.find(hash) };
		return CommonUtils::Select(it != m_currentStates.end(), it->second, false);
	}

	bool MGFKeyboardDevice::GetButtonDown(StringView actionName) const
	{
		StringHash hash { actionName };
		auto currIt { m_currentStates.find(hash) };
		auto prevIt { m_prevStates.find(hash) };

		if (currIt == m_currentStates.end() || prevIt == m_prevStates.end())
			return false;

		// 현재는 눌려있고, 이전에는 떼져 있어야 함
		return currIt->second && !prevIt->second;
	}

	bool MGFKeyboardDevice::GetButtonUp(StringView actionName) const
	{
		StringHash hash { actionName };
		auto currIt { m_currentStates.find(hash) };
		auto prevIt { m_prevStates.find(hash) };

		if (currIt == m_currentStates.end() || prevIt == m_prevStates.end())
			return false;

		// 현재는 떼져 있고, 이전에는 눌려 있어야 함
		return !currIt->second && prevIt->second;
	}

	void MGFKeyboardDevice::MapKey(StringView actionName, int32 keyCode)
	{
		if (!CommonUtils::IsBetween(keyCode, 0, GLFW_KEY_LAST))
		{
			MGF_LOG_ERROR("MGFKeyboardDevice: Invalid key code {0}", keyCode);
			return;
		}

		StringHash hash{ actionName };

		// 1. 해당 이름의 액션이 없다면 새로 생성
		if (m_actions.find(hash) == m_actions.end())
		{
			m_actions[hash] = MakeUnique<MGFKeyAction>(String(actionName));
			m_currentStates[hash] = false;
			m_prevStates[hash] = false;
		}

		// 2. 액션에 키 바인딩 추가
		m_actions[hash]->AddKeyBinding(keyCode);

		// 3. 빠른 이벤트 전파를 위해 키 바인딩 배열에 액션 포인터 등록
		auto& bindings{ m_keyBindings[keyCode] };
		bool alreadyExists{ false };
		for (auto* actionPtr : bindings) // find로 바꿀 수 없음?
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

	void MGFKeyboardDevice::OnKey(int32 key, int32 action)
	{
		if (!CommonUtils::IsBetween(key, 0, GLFW_KEY_LAST))
		{
			MGF_LOG_ERROR("MGFKeyboardDevice: Invalid key code {0}", key);
			return;
		}

		// GLFW 액션을 엔진의 InputEvent로 변환
		InputEvent evt;
		switch (action)
		{
		case GLFW_PRESS: evt = InputEvent::Pressed; break;
		case GLFW_RELEASE: evt = InputEvent::Released; break;
		default: return;
		}

		// 해당 키에 묶인 모든 액션의 콜백 실행
		const auto& actions{ m_keyBindings[key] };
		for (auto* keyAction : actions)
		{
			if (keyAction != nullptr)
				keyAction->Invoke(evt);
		}
	}
}