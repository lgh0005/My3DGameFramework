#include "RuntimePch.h"
#include "GUIProjectViewport.h"
#include <imgui.h>
#include <system_error>

namespace MGF3D
{
	ProjectViewport::ProjectViewport() = default;
	ProjectViewport::~ProjectViewport() = default;

	ProjectViewportUPtr ProjectViewport::Create(const String& name)
	{
		auto window = ProjectViewportUPtr(new ProjectViewport());
		if (!window->Init(name)) return nullptr;
		return window;
	}

	bool ProjectViewport::Init(const String& name)
	{
		m_name = name;
		m_isOpen = true;

		m_rootPath = FileSystem::current_path();
		m_currentPath = m_rootPath;

		return true;
	}

	void ProjectViewport::OnDraw()
	{
		if (ImGui::Begin(m_name.c_str(), &m_isOpen))
		{
			if (m_currentPath != m_rootPath)
			{
				if (ImGui::Button(".. Up"))
				{
					m_currentPath = m_currentPath.parent_path();
				}
				ImGui::SameLine();
			}

			ImGui::Text("Path: %s", m_currentPath.string().c_str());
			ImGui::Separator();

			if (ImGui::BeginChild("ProjectFilesScroll"))
			{
				ScanDirectory(m_currentPath);
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}

	void ProjectViewport::ScanDirectory(const std::filesystem::path& path)
	{
		std::error_code ec;
		auto it = std::filesystem::directory_iterator(path, ec);
		if (ec)
		{
			return;
		}

		for (const auto& entry : it)
		{
			const auto& entryPath = entry.path();
			String filename = entryPath.filename().string();

			std::error_code isDirEc;
			bool isDirectory = entry.is_directory(isDirEc);
			if (isDirEc)
			{
				continue;
			}

			if (isDirectory)
			{
				if (ImGui::Selectable(("[Dir] " + filename).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
				{
					if (ImGui::IsMouseDoubleClicked(0))
					{
						m_currentPath /= entryPath.filename();
						break;
					}
				}
			}
			else
			{
				String ext = entryPath.extension().string();
				String displayPrefix = "[File] ";

				if (ext == ".h" || ext == ".cpp") displayPrefix = "[Code] ";
				else if (ext == ".mymodel")       displayPrefix = "[Model] ";
				else if (ext == ".myanim")        displayPrefix = "[Anim] ";
				else if (ext == ".ktx")           displayPrefix = "[Tex] ";
				else if (ext == ".wav" || ext == ".mp3" || ext == ".ogg") displayPrefix = "[Audio] ";

				ImGui::Selectable((displayPrefix + filename).c_str(), false);

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					std::error_code relEc;
					auto relativePath = std::filesystem::relative(entryPath, m_rootPath, relEc);
					if (!relEc)
					{
						String relPathStr = relativePath.string();
						ImGui::SetDragDropPayload("CONTENT_ASSET_PATH", relPathStr.c_str(), relPathStr.size() + 1);
					}

					ImGui::Text("%s", filename.c_str());
					ImGui::EndDragDropSource();
				}
			}
		}
	}
}