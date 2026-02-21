#pragma once
#include <filesystem>
#include "Containers/Slab/SString.h"

namespace MGF3D
{
	namespace fs = std::filesystem;

	class FilePath
	{
	public:
		FilePath();
		FilePath(const fs::path& path);
		FilePath(const SString& pathStr);
		~FilePath();

	public:
		cstr GetCStr() const { return m_rawPath.c_str(); }
		const SString& ToSString() const { return m_rawPath; }
		// MGFName ToName() const { return MGFName(m_rawPath.CStr()); }

	// 경로 정보 추출
	public:
		FilePath GetDirectory() const { return m_path.parent_path(); }
		SString GetFileName() const { return SString(m_path.filename().string().c_str()); }
		SString GetExtension() const { return SString(m_path.extension().string().c_str()); }
		SString GetNameWithoutExtension() const { return SString(m_path.stem().string().c_str()); }

	// 상태 확인
	public:
		bool IsRelative() const { return m_path.is_relative(); }
		bool IsAbsolute() const { return m_path.is_absolute(); }
		bool HasExtension() const { return m_path.has_extension(); }
		bool Exists() const { return fs::exists(m_path); }

	// 경로 결합 (operator / 사용)
	public:
		FilePath operator/(const FilePath& other) const { return m_path / other.m_path; }
		FilePath& operator/=(const FilePath& other) { m_path /= other.m_path; UpdateRawPath(); return *this; }
		operator const fs::path& () const { return m_path; }

	private:
		void UpdateRawPath() { m_rawPath = m_path.string().c_str(); }

		fs::path m_path;
		SString m_rawPath;
	};
}