#pragma once

class FileManager
{
	DECLARE_SINGLE(FileManager)
	DECLARE_NONINSTANTIABLE(FileManager)

public:
	bool Init();
	std::string Resolve(const std::string& virtualPath);
	fs::path GetAssetRoot() const { return m_assetRoot; }
	fs::path GetConfigRoot() const { return m_configRoot; }

private:
	static fs::path GetExecutablePath();

	fs::path m_binPath;     // 실행 파일이 있는 Bin 폴더
	fs::path m_assetRoot;   // Bin/Assets
	fs::path m_configRoot;  // Bin/Config

	// pair<가상접두사, 절대경로>
	std::vector<std::pair<std::string_view, fs::path>> m_virtualPaths;
};
