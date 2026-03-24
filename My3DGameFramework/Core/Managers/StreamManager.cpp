#include "CorePch.h"
#include "StreamManager.h"
#include "Stream/MemoryStream.h"
#include "Stream/MemoryStreamBufferPool.h"
#include "Stream/MemoryStreamBuffer.h"

namespace MGF3D
{
	StreamManager::StreamManager() = default;
	StreamManager::~StreamManager() = default;

	bool StreamManager::Init()
	{
		// 메모리 스트림 버퍼 풀 생성
		m_memoryStreamBufferPool = MakeUnique<MemoryStreamBufferPool>();
		if (!m_memoryStreamBufferPool)
		{
			MGF_LOG_ERROR("StreamManager: Failed to initialize MemoryStreamBufferPool.");
			return false;
		}

		MGF_LOG_INFO("StreamManager: Initialized successfully with BufferPool.");
		return true;
	}

	void StreamManager::Shutdown()
	{
		if (m_memoryStreamBufferPool) m_memoryStreamBufferPool.Reset();
		MGF_LOG_INFO("StreamManager: Shutdown successfully.");
	}

	/*==================================//
	//       Synchronous Operations     //
	//==================================*/
	Nullable<FileStreamPtr> StreamManager::OpenRead(const MGFPath& path)
	{
		auto fileStream = MakeShared<FileStream>(path, FileMode::Open, FileAccess::Read);
		if (!fileStream->CanRead())
		{
			MGF_LOG_ERROR("StreamManager: Failed to open file for read: {}", path.GetCStr());
			return None;
		}

		return fileStream;
	}

	Nullable<MemoryStreamPtr> StreamManager::ReadAllBytes(const MGFPath& path)
	{
		auto fileResult = OpenRead(path);
		if (fileResult.IsNull()) return None;

		FileStreamPtr file = fileResult.Get();
		uint64 length = file->GetLength();
		if (length == 0) return None;

		// 풀에서 규격화된 버퍼를 가져와 직접 Read 수행
		auto buffer = m_memoryStreamBufferPool->Acquire(static_cast<usize>(length));
		if (file->Read(buffer->GetPtr(), static_cast<usize>(length)) == length)
		{
			// MemoryStream이 해당 버퍼를 소유하며, 소멸 시 자동으로 풀에 반납
			auto memStream = MakeShared<MemoryStream>(std::move(buffer), static_cast<usize>(length));
			memStream->SetPosition(0);
			return memStream;
		}

		MGF_LOG_ERROR("StreamManager: Failed to read all bytes from: {}", path.GetCStr());
		return None;
	}

	MemoryStreamBufferPool* StreamManager::GetBufferPool() const
	{
		return m_memoryStreamBufferPool.Get();
	}
}