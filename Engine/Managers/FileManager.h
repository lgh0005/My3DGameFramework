#pragma once

class FileManager
{
	DECLARE_SINGLE(FileManager)
	DECLARE_NONINSTANTIABLE(FileManager)

public:
	bool Init();
	fs::path GetAssetRoot() const { return m_assetRoot; }
	fs::path GetConfigRoot() const { return m_configRoot; }

/*==================================//
//   file path resolution methods   //
//==================================*/
public:
	std::string Resolve(const std::string& virtualPath);

private:
	bool IsVirtualPath(const std::string& path);
	std::string ResolveVirtualPath(const std::string& path);
	std::string ResolveAbsolutePath(const std::string& path);

private:
	static fs::path GetExecutablePath();

	fs::path m_binPath;     // 실행 파일이 있는 Bin 폴더
	fs::path m_assetRoot;   // Bin/Assets
	fs::path m_configRoot;  // Bin/Config

	// 가상 경로 맵
	std::unordered_map<std::string, fs::path> m_virtualPathMap;
};
