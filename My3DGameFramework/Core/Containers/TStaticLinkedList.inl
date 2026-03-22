#include "TStaticLinkedList.h"
#pragma once

namespace MGF3D
{
    template<typename T, typename Alloc>
    inline TStaticLinkedList<T, Alloc>::TStaticLinkedList() = default;

    template<typename T, typename Alloc>
    inline TStaticLinkedList<T, Alloc>::TStaticLinkedList(usize inCapacity)
    {
        Resize(inCapacity);
    }

    template<typename T, typename Alloc>
    inline void TStaticLinkedList<T, Alloc>::Resize(usize inCapacity)
    {
        m_nodes.Assign(inCapacity, Node());
        m_head = -1;
        m_tail = -1;
    }

    template<typename T, typename Alloc>
    inline void TStaticLinkedList<T, Alloc>::Release()
    {
        m_nodes.Release();
        m_head = -1;
        m_tail = -1;
    }

    template<typename T, typename Alloc>
    inline void TStaticLinkedList<T, Alloc>::Remove(int32 inIndex)
    {
        if (inIndex == -1) return;

        auto& node = m_nodes[inIndex];

        // 이전 노드의 next를 나의 next로 연결
        if (node.prev != -1) m_nodes[node.prev].next = node.next;
        else m_head = node.next;

        // 다음 노드의 prev를 나의 prev로 연결
        if (node.next != -1) m_nodes[node.next].prev = node.prev;
        else m_tail = node.prev;

        // 나의 연결 고리 초기화
        node.prev = -1;
        node.next = -1;
    }

    template<typename T, typename Alloc>
    inline void TStaticLinkedList<T, Alloc>::PushFront(int32 inIndex)
    {
        if (inIndex == -1) return;

        auto& node = m_nodes[inIndex];
        node.next = m_head;
        node.prev = -1;

        if (m_head != -1)
            m_nodes[m_head].prev = inIndex;

        m_head = inIndex;

        // 리스트가 비어있었다면 Tail도 나임
        if (m_tail == -1)
            m_tail = inIndex;
    }

    template<typename T, typename Alloc>
    inline void TStaticLinkedList<T, Alloc>::MoveToFront(int32 inIndex)
    {
        if (inIndex == m_head || inIndex == -1) return;

        Remove(inIndex);
        PushFront(inIndex);
    }
}