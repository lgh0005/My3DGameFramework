#pragma once
#include "Editor/GUIWindow.h"

namespace MGF3D
{
	MGF_CLASS_PTR(InspectorViewport)
	class InspectorViewport : public GUIWindow
	{
		using Super = GUIWindow;

	public:
		virtual ~InspectorViewport() override;
		static InspectorViewportUPtr Create(const String& name);

	public:
		virtual void OnDraw() override;

	private:
		InspectorViewport();
		bool Init(const String& name);
	};
}