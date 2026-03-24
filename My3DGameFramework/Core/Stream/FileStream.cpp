#include "CorePch.h"
#include "FileStream.h"

namespace MGF3D
{
	FileStream::FileStream(const MGFPath& path, FileMode mode, FileAccess access)
		: m_path(path)
	{
		m_openMode = CalculateOpenMode(mode, access);
		m_file.open(m_path.GetCStr(), m_openMode);
	}

	uint64 FileStream::GetLength() const
	{
		if (!m_file.is_open()) return 0;

		// 현재 위치 기억
		std::streampos currentPos = m_file.tellg();

		// 끝으로 가서 위치 확인
		m_file.seekg(0, std::ios::end);
		std::streampos endPos = m_file.tellg();

		// 원래 위치로 복구
		m_file.seekg(currentPos);

		return static_cast<uint64>(endPos);
	}

	uint64 FileStream::GetPosition() const
	{
		return static_cast<uint64>(m_file.tellg());
	}

	void FileStream::SetPosition(uint64 pos)
	{
		m_file.clear();
		m_file.seekg(pos, std::ios::beg);
		m_file.seekp(pos, std::ios::beg);
	}

	FileStream::~FileStream()
	{
		Close();
	}

	usize FileStream::Read(void* buffer, usize size)
	{
		m_file.read(static_cast<char*>(buffer), size);
		return static_cast<usize>(m_file.gcount());
	}

	usize FileStream::Write(const void* buffer, usize size)
	{
		m_file.write(static_cast<const char*>(buffer), size);
		return size;
	}

	std::ios_base::openmode FileStream::CalculateOpenMode(FileMode mode, FileAccess access)
	{
		std::ios_base::openmode flags = std::ios::binary;

		switch (access)
		{
			case FileAccess::Read: flags |= std::ios::in; break;
			case FileAccess::Write: flags |= std::ios::out; break;
			case FileAccess::ReadWrite: flags |= (std::ios::in | std::ios::out); break;
			default: break;
		}

		switch (mode)
		{
			case FileMode::Create:   flags |= std::ios::trunc; break;
			case FileMode::Append:   flags |= std::ios::app;   break;
			default: break;
		}

		return flags;
	}
}