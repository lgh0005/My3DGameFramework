#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(GUIWindow)
	class GUIWindow
	{
	public:
		virtual ~GUIWindow();
		virtual void OnDraw() = 0;

	public:
		void SetOpen(bool isOpen);
		bool IsOpen() const;

	protected:
		GUIWindow();
		String m_name { "Unnamed Window" };
		bool m_isOpen { true };
	};
}