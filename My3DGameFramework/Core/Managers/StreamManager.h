#pragma once
#include "Stream/FileStream.h"
#include "Identity/MGFPath.h"

namespace MGF3D
{
	MGF_CLASS_PTR(MemoryStream)
	MGF_CLASS_PTR(MemoryStreamBufferPool)

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
		MemoryStreamBufferPool* GetBufferPool() const;

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
		mutable Mutex m_streamMutex;
		MemoryStreamBufferPoolUPtr m_memoryStreamBufferPool;
	};
}

#include "Managers/StreamManager.inl"