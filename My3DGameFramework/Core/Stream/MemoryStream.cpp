#include "CorePch.h"
#include "MemoryStream.h"
#include "Utils/MemoryUtils.h"
#include "Math/Math.h"

namespace MGF3D
{
	MemoryStream::MemoryStream(usize initialCapacity)
	{
		m_buffer.Reserve(initialCapacity);
	}
	MemoryStream::~MemoryStream() = default;

	uint64 MemoryStream::GetLength() const
	{
		return static_cast<uint64>(m_buffer.Count());
	}

	uint64 MemoryStream::GetPosition() const
	{
		return m_position;
	}

	void MemoryStream::SetPosition(uint64 pos)
	{
		m_position = CommonUtils::Select(pos > GetLength(), GetLength(), pos);
	}

	usize MemoryStream::Read(void* buffer, usize size)
	{
		if (m_position >= GetLength()) return 0;

		uint64 remain = GetLength() - m_position;
		uint64 readSize = Math::Min(remain, size);
		if (readSize > 0)
		{
			MemoryUtils::Memcpy(buffer, m_buffer.Data() + m_position, static_cast<usize>(readSize));
			m_position += readSize;
		}

		return static_cast<usize>(readSize);
	}

	usize MemoryStream::Write(const void* buffer, usize size)
	{
		// 1. 쓸 데이터가 없으면 즉시 리턴
		if (size == 0) return 0;

		uint64 endPos = m_position + size;

		// 2. 필요 시 버퍼 확장
		if (endPos > GetLength())
			m_buffer.Resize(static_cast<usize>(endPos));

		MemoryUtils::Memcpy(m_buffer.Data() + m_position, buffer, size);

		m_position = endPos;
		return size;
	}

	void MemoryStream::Close()
	{
		m_buffer.Clear(); 
		m_position = 0;
	}
}