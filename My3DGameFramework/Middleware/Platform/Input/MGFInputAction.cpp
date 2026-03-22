#include "MiddlewarePch.h"
#include "MGFInputAction.h"

namespace MGF3D
{
	MGFInputAction::MGFInputAction(const SString& name) : m_name(name) { }
	MGFInputAction::~MGFInputAction()
	{
		Release();
	}

	void MGFInputAction::AddKeyBinding(int32 keyCode)
	{
		if (!m_mappedKeys.Contains(keyCode))
			m_mappedKeys.PushBack(keyCode);
	}

	void MGFInputAction::AddMouseButtonBinding(int32 button)
	{
		if (!m_mappedMouseButtons.Contains(button))
			m_mappedMouseButtons.PushBack(button);
	}

	void MGFInputAction::BindCallback(InputEvent event, InputCallback callback)
	{
		if (event >= InputEvent::MAX) return;
		m_callbacks[(int32)event].PushBack(std::move(callback));
	}

	void MGFInputAction::Invoke(InputEvent event)
	{
		if (event >= InputEvent::MAX) return;
		for (const auto& callback : m_callbacks[(int32)event])
		{
			if (callback) 
				callback();
		}
	}

	void MGFInputAction::Release()
	{
		m_name.Release();
		m_mappedKeys.Release();
		m_mappedMouseButtons.Release();

		for (int32 i = 0; i < (int32)InputEvent::MAX; ++i)
			m_callbacks[i].Release();
	}
}