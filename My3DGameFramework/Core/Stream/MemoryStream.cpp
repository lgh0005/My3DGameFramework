#include "CorePch.h"
#include "MemoryStream.h"
#include "Common/CommonUtils.h"
#include "Managers/StreamManager.h"
#include "Utils/MemoryUtils.h"
#include "Stream/MemoryStreamBuffer.h"
#include "Stream/MemoryStreamBufferPool.h"
#include "Math/Math.h"

namespace MGF3D
{
	MemoryStream::MemoryStream(MemoryStreamBufferPtr buffer, usize dataSize)
		: m_buffer(std::move(buffer)), m_length(dataSize) 
	{
		if (m_buffer && m_length > m_buffer->GetCapacity())
			m_length = m_buffer->GetCapacity();
	}

	MemoryStream::~MemoryStream()
	{
		Close();
	}

	uint64 MemoryStream::GetLength() const
	{
		return m_length;
	}

	uint64 MemoryStream::GetPosition() const
	{
		return m_position;
	}

	void MemoryStream::SetPosition(uint64 pos)
	{
		m_position = Math::Min(pos, GetLength());
	}

	usize MemoryStream::Read(void* buffer, usize size)
	{
		if (m_position >= GetLength()) return 0;

		uint64 remain = GetLength() - m_position;
		uint64 readSize = Math::Min(remain, (uint64)size);
		if (readSize > 0)
		{
			MemoryUtils::Memcpy(buffer, m_buffer->GetPtr() + m_position, static_cast<usize>(readSize));
			m_position += readSize;
		}

		return static_cast<usize>(readSize);
	}

	usize MemoryStream::Write(const void* buffer, usize size)
	{
		// 1. 쓸 데이터가 없으면 즉시 리턴
		if (size == 0) return 0;

		uint64 endPos = m_position + size;

		// 2. 풀에서 가져온 버퍼의 용량을 초과하는지 체크
		if (endPos > m_buffer->GetCapacity())
		{
			MGF_LOG_ERROR("MemoryStream: Write out of pooled buffer capacity!");
			return 0;
		}

		MemoryUtils::Memcpy(m_buffer->GetPtr() + m_position, buffer, size);

		m_position = endPos;

		// 3. 데이터가 늘어났다면 길이 업데이트
		if (m_position > m_length) m_length = m_position;

		return size;
	}

	void MemoryStream::Close()
	{
		if (m_buffer)
		{
			// 스마트 포인터를 move로 넘겨주며 풀에 반납
			MGF_STREAM.GetBufferPool()->Release(std::move(m_buffer));
			m_buffer = nullptr;
		}
	}

	const byte* MemoryStream::GetData() const
	{
		return m_buffer ? m_buffer->GetPtr() : nullptr;
	}
}