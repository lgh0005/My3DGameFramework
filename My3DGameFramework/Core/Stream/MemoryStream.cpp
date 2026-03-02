#include "CorePch.h"
#include "MemoryStream.h"

namespace MGF3D
{
	MemoryStream::~MemoryStream()
	{
	}
	bool MemoryStream::CanRead() const
	{
		return false;
	}
	bool MemoryStream::CanWrite() const
	{
		return false;
	}
	bool MemoryStream::CanSeek() const
	{
		return false;
	}
	uint64 MemoryStream::GetLength() const
	{
		return uint64();
	}
	uint64 MemoryStream::GetPosition() const
	{
		return uint64();
	}
	void MemoryStream::SetPosition(uint64 pos)
	{
	}
	usize MemoryStream::Read(void* buffer, usize size)
	{
		return usize();
	}
	usize MemoryStream::Write(const void* buffer, usize size)
	{
		return usize();
	}
	void MemoryStream::Flush()
	{
	}
	void MemoryStream::Close()
	{
	}
}