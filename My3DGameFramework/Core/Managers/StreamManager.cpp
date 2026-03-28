#include "CorePch.h"
#include "StreamManager.h"
#include "Managers/PathManager.h"
#include "Stream/MemoryStream.h"
#include "Stream/MemoryStreamBufferPool.h"
#include "Stream/MemoryStreamBuffer.h"
#include "Utils/MemoryUtils.h"

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
		MGF_LOCK_SCOPE(m_streamMutex);
		if (m_memoryStreamBufferPool) m_memoryStreamBufferPool.Reset();
		MGF_LOG_INFO("StreamManager: Shutdown successfully.");
	}

	/*==================================//
	//       Synchronous Operations     //
	//==================================*/
	Nullable<FileStreamPtr> StreamManager::OpenRead(const MGFPath& path)
	{
		MGFPath physicalPath = MGF_PATH.Resolve(path); // TODO : path가 가상경로인지 아닌지에 대해서 처리 필요?
		auto fileStream = MakeShared<FileStream>(physicalPath, FileMode::Open, FileAccess::Read);
		if (!fileStream->CanRead())
		{
			MGF_LOG_ERROR("StreamManager: Failed to open file for read: {}", physicalPath.GetCStr());
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

		// 1. [Lock] 버퍼 풀에 접근하기 전에 락을 겁니다.
		MemoryStreamBufferPtr buffer = nullptr;
		{
			MGF_LOCK_SCOPE(m_streamMutex);
			buffer = m_memoryStreamBufferPool->Acquire(static_cast<usize>(length));
		}
		if (!buffer) return None;

		// 2. 이전 작업의 찌꺼기를 제거합니다.
		MemoryUtils::Memset(buffer->GetPtr(), 0, buffer->GetCapacity());

		// 3. 데이터 읽기
		if (file->Read(buffer->GetPtr(), static_cast<usize>(length)) == length)
        {
            auto memStream = MakeShared<MemoryStream>(Move(buffer), static_cast<usize>(length));
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