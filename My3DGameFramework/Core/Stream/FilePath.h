#pragma once
#include <filesystem>
#include <string>

namespace MGF3D
{
	namespace fs = std::filesystem;

	class FilePath
	{
	public:
		FilePath();
		FilePath(const fs::path& path);
		~FilePath();

	public:
		const char* GetCStr() const { return m_rawPath.c_str(); }
		std::string ToString() const { return m_path.string(); }

	// 경로 정보 추출
	public:
		FilePath GetDirectory() const { return m_path.parent_path(); }
		std::string GetFileName() const { return m_path.filename().string(); }
		std::string GetExtension() const { return m_path.extension().string(); }
		std::string GetNameWithoutExtension() const { return m_path.stem().string(); }

	// 상태 확인
	public:
		bool IsRelative() const { return m_path.is_relative(); }
		bool IsAbsolute() const { return m_path.is_absolute(); }
		bool HasExtension() const { return m_path.has_extension(); }

	// 경로 결합 (operator / 사용)
	public:
		FilePath operator/(const FilePath& other) const { return m_path / other.m_path; }
		FilePath& operator/=(const FilePath& other) { m_path /= other.m_path; UpdateRawPath(); return *this; }
		operator const fs::path& () const { return m_path; }

	private:
		void UpdateRawPath() { m_rawPath = m_path.string(); }

		fs::path m_path;
		std::string m_rawPath;
	};
}