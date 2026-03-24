#include "CorePch.h"
#include "StreamManager.h"
#include "Managers/TaskManager.h"

namespace MGF3D
{
	StreamManager::StreamManager() = default;
	StreamManager::~StreamManager() = default;

	bool StreamManager::Init()
	{
		// TODO : 메모리 스트림 버퍼 인스턴스 생성 및 대입
		// 그리고 검증성 체크
		return false;
	}

	void StreamManager::Shutdown()
	{
		// TODO : 메모리 스트림 버퍼 정리
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

		MemoryStreamPtr memStream = MakeShared<MemoryStream>(static_cast<usize>(length));

		// 파일의 모든 내용을 메모리 스트림으로 전송
		// TODO : 이후에 메모리 스트림 풀을 만들 필요가 있음
		SVector<byte> tempBuffer;
		tempBuffer.Resize(static_cast<usize>(length));

		if (file->Read(tempBuffer.Data(), static_cast<usize>(length)) == length)
		{
			memStream->Write(tempBuffer.Data(), static_cast<usize>(length));
			memStream->SetPosition(0);
			return memStream;
		}

		return None;
	}
}