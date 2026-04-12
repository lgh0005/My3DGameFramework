#pragma once

namespace MGF3D
{
	class GUIManager
	{
		MGF_DECLARE_SINGLE(GUIManager)

	private:
		GUIManager();
		~GUIManager();

	public:
		void Init(bool enable);
		void Clear();
		void BeginFrame();
		void EndFrame();
		bool Begin
		(
			const char* name,
			bool* p_open = nullptr,
			ImGuiWindowFlags flags = 0
		);
		void End();

	private:
		bool m_isEnable{ true };
	};
}