#pragma once
#include <future>

// INFO : 퓨처와 프로미스는 
// 이게 thread에 들어갈 함수는 void 형 함수 뿐인데,
// 마치 반환형이 있도록 만들 때 쓰이는 도구라고 생각해보자.
// 여기서 프로미스는 가방, 퓨처는 영수증(?) 같은 개념임.

// ex)
//// 1. 비동기로 텍스트 파일을 읽어서 SString을 반환하고 싶을 때
//void LoadFileAsync(MGFPromise<SString> promise, SString filePath)
//{
//	// 파일을 읽는 무거운 작업 수행...
//	SString result = FileSystem::ReadAllText(filePath);
//
//	// [핵심] 결과를 약속(Promise)에 담아 전달
//	promise.SetValue(result);
//}
//
//// 2. 호출부 (메인 스레드 등)
//MGFPromise<SString> promise;
//MGFFuture<SString> future = promise.GetFuture();
//
//// 스레드 기동 (Action으로 래핑)
//ThreadManager::Instance().CreateThread("FileLoader", [promise, path]() mutable {
//	LoadFileAsync(std::move(promise), path);
//	});
//
//// ... 다른 작업 수행 (Non-blocking) ...
//
//// 3. 결과가 필요할 때 꺼내기
//if (future.IsRead())
//{
//	SString content = future.Get();
//	MGF_LOG_INFO("File Content: {0}", content.CStr());
//}

namespace MGF3D
{
	template<typename T>
	class MGFFuture
	{
	public:
		MGFFuture();
		MGFFuture(Future<T>&& future);

		T Get();
		bool IsRead() const;
		bool IsValid() const;

	private:
		Future<T> m_future;
	};
}

#include "Thread/MGFFuture.inl"