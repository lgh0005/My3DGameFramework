#pragma once
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
	template<typename T>
	class MpscQueue
	{
	private:

		struct Node : public PoolAlloc
		{
			T Item;
			Atomic<Ptr<Node>> Next;
			Node() : Next(nullptr) {}
			Node(const T& item) : Item(item), Next(nullptr) {}
		};

	public:
		MpscQueue();
		~MpscQueue();

	public:
		void Push(const T& item) noexcept;
		bool Pop(T& outItem) noexcept;

	private:
		// MPSC 구조를 위한 원자적 포인터
		Atomic<Ptr<Node>> m_head;
		Atomic<Ptr<Node>> m_tail;
	};
}

#include "Request/MpscQueue.inl"