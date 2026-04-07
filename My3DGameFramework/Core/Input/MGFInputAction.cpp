#include "CorePch.h"
#include "MGFInputAction.h"
#include <GLFW/glfw3.h>

namespace MGF3D
{
	MGFInputAction::MGFInputAction(const String& name) : m_name(name) {}
	MGFInputAction::~MGFInputAction()
	{
		Release();
	}

    void MGFInputAction::BindCallback(InputEvent event, const Action<>& callback)
    {
        if (event >= InputEvent::MAX) return;
        m_callbacks[static_cast<int32>(event)].push_back(callback);
    }

    void MGFInputAction::Invoke(InputEvent event)
    {
        if (event >= InputEvent::MAX) return;

        const auto& eventCallbacks = m_callbacks[static_cast<int32>(event)];
        for (const auto& callback : eventCallbacks)
        {
            if (callback)
                callback();
        }
    }

    void MGFInputAction::Release()
    {
        for (int32 i = 0; i < static_cast<int32>(InputEvent::MAX); ++i)
            m_callbacks[i].clear();
    }
}