#pragma once
#include "Pointer/RefCount.h"

namespace MGF3D
{
    template <typename T>
    class MGFAsyncResult : public RefCount
    {
    public:
        MGFAsyncResult();
        virtual ~MGFAsyncResult() override;

    public:
        void Set(T&& data);
        void Set(Nullable<T>&& data);

    public:
        bool IsReady() const;
        Nullable<T>& Get();

    private:
        Nullable<T>       m_data    { None };
        Atomic<bool>      m_isReady { false };
    };
}

#include "Task/MGFAsyncResult.inl"