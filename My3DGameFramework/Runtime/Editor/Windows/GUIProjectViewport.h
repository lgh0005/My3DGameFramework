#pragma once
#include "Editor/GUIWindow.h"

namespace MGF3D
{
	MGF_CLASS_PTR(ProjectViewport)
		class ProjectViewport : public GUIWindow
	{
		using Super = GUIWindow;

	public:
		virtual ~ProjectViewport() override;
		static ProjectViewportUPtr Create(const String& name);

	public:
		virtual void OnDraw() override;

	private:
		ProjectViewport();
		bool Init(const String& name);
		void ScanDirectory(const FileSystem::path& path);

	private:
		FileSystem::path m_rootPath;
		FileSystem::path m_currentPath;
	};
}