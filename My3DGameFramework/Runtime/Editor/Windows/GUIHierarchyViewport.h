#pragma once
#include "Editor/GUIWindow.h"

namespace MGF3D
{
	MGF_CLASS_PTR(HierarchyViewport)
		class HierarchyViewport : public GUIWindow
	{
		using Super = GUIWindow;

	public:
		virtual ~HierarchyViewport() override;
		static HierarchyViewportUPtr Create(const String& name);

	public:
		virtual void OnDraw() override;

	private:
		HierarchyViewport();
		bool Init(const String& name);
	};
}