#pragma once
#include "Editor/GUIWindow.h"

namespace MGF3D
{
    MGF_CLASS_PTR(SceneViewport)
    class SceneViewport : public GUIWindow
    {
        using Super = GUIWindow;

    public:
        virtual ~SceneViewport() override;
        static SceneViewportUPtr Create(const String& name);

    public:
        virtual void OnDraw() override;

    private:
        SceneViewport();
        bool Init(const String& name);
    };
}