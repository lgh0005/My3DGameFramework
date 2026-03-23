#pragma once
#include "Containers/TVector.h"

namespace MGF3D
{
    template<typename T, typename Alloc>
    class TStaticLinkedList
    {
    public:
        struct Node
        {
            T data;
            int32 prev  { -1 };
            int32 next  { -1 };
        };

    public:
        TStaticLinkedList();
        TStaticLinkedList(usize inCapacity);

    public:
        void Resize(usize inCapacity);
        void Release();

        void MoveToFront(int32 inIndex);
        void Remove(int32 inIndex);
        void PushFront(int32 inIndex);

        int32 GetHead() const { return m_head; }
        int32 GetTail() const { return m_tail; }

    public:
        T& GetData(int32 inIndex) { return m_nodes[inIndex].data; }
        const T& GetData(int32 inIndex) const { return m_nodes[inIndex].data; }

        T& operator[](int32 inIndex) { return GetData(inIndex); }
        const T& operator[](int32 inIndex) const { return GetData(inIndex); }

    private:
        using NodeAllocator = typename Alloc::template rebind<Node>::other;
        TVector<Node, NodeAllocator> m_nodes;
        int32 m_head{ -1 };
        int32 m_tail{ -1 };
    };
}

#include "Containers/TStaticLinkedList.inl"