#pragma once
#include "Stream/FileStream.h"
#include "Stream/MemoryStream.h"
#include "Identity/MGFPath.h"

namespace MGF3D
{
	class StreamManager
	{
		MGF_DECLARE_SINGLE(StreamManager)

	private:
		StreamManager();
		~StreamManager();

	public:
		bool Init();
		void Shutdown();

	/*==================================//
	//       Synchronous Operations     //
	//==================================*/
	public:
		Nullable<FileStreamPtr> OpenRead(const MGFPath& path);
		Nullable<MemoryStreamPtr> ReadAllBytes(const MGFPath& path);

	/*==================================//
	//      Asynchronous Operations     //
	//==================================*/
	public:
		template <typename T>
		void ReadFileAsync
		(
			const MGFPath& path,
			Func<Nullable<T>, FileStreamPtr> processor,
			Action<Nullable<T>> onComplete
		);

		template <typename T>
		void ReadBytesAsync
		(
			const MGFPath& path,
			Func<Nullable<T>, MemoryStreamPtr> processor,
			Action<Nullable<T>> onComplete
		);

	private:
		// TODO : 메모리 스트림과 관련해서는 여기에 해당 버퍼 풀을 
		// 만들어 두는 것도 좋은 방향이 될 수 있음
	};
}

#include "Managers/StreamManager.inl"