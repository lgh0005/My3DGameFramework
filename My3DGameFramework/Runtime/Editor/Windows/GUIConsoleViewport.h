#pragma once
#include "Editor/GUIWindow.h"

namespace MGF3D
{
	MGF_CLASS_PTR(ConsoleViewport)
		class ConsoleViewport : public GUIWindow
	{
		using Super = GUIWindow;

	public:
		virtual ~ConsoleViewport() override;
		static ConsoleViewportUPtr Create(const String& name);

	public:
		virtual void OnDraw() override;

	private:
		ConsoleViewport();
		bool Init(const String& name);
	};
}