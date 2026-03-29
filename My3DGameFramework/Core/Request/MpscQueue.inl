#pragma once

namespace MGF3D
{
	template<typename T>
	inline MpscQueue<T>::MpscQueue()
	{
		// 락프리 큐의 전형적인 기법인 '더미 노드(Dummy Node)'를 하나 생성해 둡니다.
		// 이를 통해 Push와 Pop이 완전히 분리된 노드를 바라보게 하여 경합을 없앱니다.
		Ptr<Node> dummy = new Node();
		m_head.store(dummy);
		m_tail.store(dummy);
	}

	template<typename T>
	inline MpscQueue<T>::~MpscQueue()
	{
		T dummyItem;
		while (Pop(dummyItem)) {}
		delete m_head.load();
	}

	template<typename T>
	inline void MpscQueue<T>::Push(const T& item) noexcept
	{
		Ptr<Node> newNode = new Node(item);

		// exchange 연산을 통해 현재 꼬리(Head)를 가로채고, 그 자리에 새 노드를 등록합니다.
		// 이 단 한 줄의 원자적 연산으로 다중 생산자(워커 스레드) 간의 동기화가 끝납니다.
		Ptr<Node> prevHead = m_head.exchange(newNode);
		prevHead->Next.store(newNode);
	}

	template<typename T>
	inline bool MpscQueue<T>::Pop(T& outItem) noexcept
	{
		Ptr<Node> currentTail = m_tail.load();
		Ptr<Node> nextNode = currentTail->Next.load();

		if (nextNode != nullptr)
		{
			// 더미 노드의 다음 노드에 실제 데이터가 들어 있습니다.
			outItem = nextNode->Item;

			// 꼬리를 다음 노드로 전진시키고, 기존 더미 노드는 삭제합니다.
			// (단일 소비자 구조이므로 이 부분은 다른 스레드와 충돌하지 않습니다.)
			m_tail.store(nextNode);
			delete currentTail;
			return true;
		}

		return false;
	}
}