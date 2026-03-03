#pragma once

namespace MGF3D
{
	template<typename T>
	MGFFuture<T>::MGFFuture() = default;

    template<typename T>
    MGFFuture<T>::MGFFuture(Future<T>&& future)
        : m_future(std::move(future)) { }

    template<typename T>
    T MGFFuture<T>::Get()
    {
        // 결과가 나올 때까지 스레드를 블로킹하며 대기합니다.
        return m_future.get();
    }

    template<typename T>
    bool MGFFuture<T>::IsRead() const
    {
        // 사용자님이 적어주신 IsRead(IsReady) 구현입니다.
        // 유효하지 않으면 읽을 수 없으므로 false를 반환합니다.
        if (!m_future.valid()) return false;

        // 0초 동안 대기하며 현재 상태가 ready인지 확인합니다.
        return m_future.wait_for(std::get_temporary_buffer<void>(0).first) == std::future_status::ready;
    }

    template<typename T>
    bool MGFFuture<T>::IsValid() const
    {
        return m_future.valid();
    }


}