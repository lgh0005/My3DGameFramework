#pragma once

namespace MGF3D
{
    template <typename T>
    class MGFLockFreeQueue
    {
    public:
        MGFLockFreeQueue();
        ~MGFLockFreeQueue();

        struct Node
        {
            T data;
            Node* next;
            Node(const T& val) : data(val), next(nullptr) { }
            Node(T&& val) : data(std::move(val)), next(nullptr) { }
        };

    public:
        void Push(T item);
        Node* Pop();
        void Clear();

    private:
        Atomic<Node*> head { nullptr };
    };
}

#include "Thread/LockFreeQueue.inl"