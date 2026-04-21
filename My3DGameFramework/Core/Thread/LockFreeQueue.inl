#pragma once

namespace MGF3D
{
	template <typename T> inline MGFLockFreeQueue<T>::MGFLockFreeQueue() = default;
    template <typename T>
    inline MGFLockFreeQueue<T>::~MGFLockFreeQueue()
    {
        Clear();
    }

    template <typename T>
    inline void MGFLockFreeQueue<T>::Push(T item)
    {
        // 1. 새로운 노드 생성
        Node* newNode = new Node(std::move(item));

        // 2. 현재 head 값을 읽어서 새 노드의 next가 가리키게 합니다.
        // (이 시점에서는 다른 스레드와 순서가 꼬여도 상관없으므로 relaxed 사용)
        newNode->next = head.load(std::memory_order_relaxed);

        // 3. CAS(Compare-And-Swap) 알고리즘
        // head가 여전히 newNode->next와 같다면, head를 newNode로 교체합니다.
        // 만약 다른 스레드가 먼저 Push해서 head가 바뀌었다면, 
        // compare_exchange_weak가 실패하고 newNode->next를 최신 head 값으로 자동 갱신한 뒤 재시도합니다.
        while (!head.compare_exchange_weak
        (
            newNode->next,              // 예상하는 현재 값 (실패 시 최신 값으로 갱신됨)
            newNode,                    // 바꿀 새 값
            std::memory_order_release,  // 성공 시: 이전에 쓰인 메모리(item)를 다른 스레드에 동기화
            std::memory_order_relaxed)) // 실패 시: 순서 보장 없이 가볍게 재시도
        { }
    }

    template <typename T>
    inline typename MGFLockFreeQueue<T>::Node* MGFLockFreeQueue<T>::Pop()
    {
        // 하나씩 빼는 게 아니라, 현재까지 연결된 '모든' 노드를 통째로 뜯어옵니다.
        // head를 nullptr로 원자적 스왑하여, 이후 워커 스레드들의 Push는 새로운 빈 큐에 시작되게 합니다.
        return head.exchange(nullptr, std::memory_order_acquire);
    }

    template <typename T>
    inline void MGFLockFreeQueue<T>::Clear()
    {
        // 락이 없으므로, 안전하게 지우려면 먼저 큐를 통째로 내 소유로 가져와야 합니다.
        Node* current = Pop();

        // 내 소유가 된 연결 리스트를 순회하며 안전하게 delete 합니다.
        while (current != nullptr)
        {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }
}