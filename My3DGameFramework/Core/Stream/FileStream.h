#pragma once
#include "Stream.h"
#include "FilePath.h"
#include <fstream>

namespace MGF3D
{
	enum class FileMode { Create, Open, OpenOrCreate, Append };
	enum class FileAccess { Read, Write, ReadWrite };

	class FileStream : public Stream
	{
	public:
		virtual ~FileStream();
		FileStream(const FilePath& path, FileMode mode, FileAccess access);

	public:
		bool CanRead() const override { return m_file.is_open(); }
		bool CanWrite() const override { return m_file.is_open(); }

		uint64 GetPosition() const override { return static_cast<uint64>(m_file.tellg()); }
		void   SetPosition(uint64 pos) override { m_file.seekg(pos); m_file.seekp(pos); }

		void Flush() override { m_file.flush(); }
		void Close() override { if (m_file.is_open()) m_file.close(); }
		void Reopen() { if (m_file.is_open()) m_file.close(); m_file.open(m_path.GetCStr(), m_openMode); 
		}

		usize Read(void* buffer, usize size) override;
		usize Write(const void* buffer, usize size) override;

	private:
		static std::ios_base::openmode CalculateOpenMode(FileMode mode, FileAccess access);

	private:
		FilePath m_path;
		std::ios_base::openmode m_openMode;
		mutable std::fstream m_file;
	};
}
