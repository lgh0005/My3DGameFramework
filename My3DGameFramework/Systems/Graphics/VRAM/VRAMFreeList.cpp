#include "GraphicsPch.h"
#include "VRAMFreeList.h"
#include "VRAMNode.h"
#include "Utils/MemoryUtils.h"

namespace MGF3D
{
	VRAMFreeList::VRAMFreeList(uint64 totalSize) : m_totalSize(totalSize)
	{
		// 초기에는 전체 크기를 가지는 단일 빈 노드 하나만 존재
		m_head = new VRAMNode(0, m_totalSize, true);
	}
	
	VRAMFreeList::~VRAMFreeList()
	{
		// 연결 리스트 순회 및 노드 메모리 해제
		Ptr<VRAMNode> curr = m_head;
		while (curr != nullptr)
		{
			Ptr<VRAMNode> nextNode = curr->next;
			delete curr;
			curr = nextNode;
		}
		m_head = nullptr;
	}
	
	uint64 VRAMFreeList::Allocate(uint64 size, uint32 alignment)
	{
		Ptr<VRAMNode> curr = m_head;

		while (curr != nullptr)
		{
			// 1. 현재 노드가 사용 중이라면 즉시 다음 노드로 이동
			if (!curr->isFree)
			{
				curr = curr->next;
				continue;
			}

			// 1. 정렬된 오프셋 및 패딩 계산
			uint64 alignedOffset = (uint64)(MemoryUtils::AlignUp((uintptr)curr->offset, (usize)alignment));
			uint64 padding = alignedOffset - curr->offset;

			// 2. 현재 노드가 패딩과 요청 크기를 수용할 수 있는지 검사
			if (curr->size < padding + size)
			{
				curr = curr->next;
				continue;
			}

			// 3. 앞부분 패딩이 존재한다면 노드 분리 로직 수행
			if (padding > 0) 
				SplitPaddingNode(curr, padding);

			// 4. 실제 사용 영역 할당 및 뒷부분 잉여 공간 분할
			SplitNode(curr, size);

			// 5. 메모리 풀 상의 위치 반환
			m_usedMemory += size;
			return curr->offset;
		}

		MGF_LOG_ERROR("Out of VRAM! Could not allocate {0} bytes.", size);
		return static_cast<uint64>(-1);
	}
	
	void VRAMFreeList::SplitPaddingNode(Ptr<VRAMNode> target, uint64 padding)
	{
		MGF_ASSERT(target != nullptr, "SplitPaddingNode: target is nullptr");
		MGF_ASSERT(target->isFree, "SplitPaddingNode: target node must be free");

		// 1. 새로운 패딩 노드 생성 (기존 target의 시작 위치와 패딩 크기 사용)
		Ptr<VRAMNode> paddingNode = new VRAMNode(target->offset, padding, true);

		// 리스트 연결 수정: paddingNode를 target의 앞에 삽입
		paddingNode->prev = target->prev;
		paddingNode->next = target;

		if (target->prev != nullptr)
			target->prev->next = paddingNode;
		else
			m_head = paddingNode;

		target->prev = paddingNode;

		// target 노드의 정보 갱신
		target->offset += padding;
		target->size -= padding;
	}

	void VRAMFreeList::Deallocate(uint64 offset)
	{
		Ptr<VRAMNode> target = FindNode(offset);
		if (target != nullptr)
		{
			m_usedMemory -= target->size;
			target->isFree = true;
			MergeNode(target);
		}
	}
	
	Ptr<VRAMNode> VRAMFreeList::FindNode(uint64 offset)
	{
		Ptr<VRAMNode> curr = m_head;
		while (curr != nullptr)
		{
			if (curr->offset == offset && curr->isFree == false) return curr;
			curr = curr->next;
		}
		return nullptr;
	}

	void VRAMFreeList::SplitNode(Ptr<VRAMNode> target, uint64 size)
	{
		if (target == nullptr) return;

		// 타겟 노드의 크기가 요청 크기보다 크면 뒷부분을 새로운 빈 노드로 분리
		if (target->size > size)
		{
			Ptr<VRAMNode> newNode = new VRAMNode
			(
				target->offset + size, 
				target->size - size, 
				true
			);
			newNode->prev = target;
			newNode->next = target->next;
			if (target->next != nullptr)
				target->next->prev = newNode;

			target->next = newNode;
			target->size = size;
		}

		target->isFree = false;
	}

	Ptr<VRAMNode> VRAMFreeList::MergeNode(Ptr<VRAMNode> target)
	{
		if (target == nullptr) return nullptr;

		auto result = target;
		result = MergeWithNext(result);
		result = MergeWithPrev(result);

		return result;
	}

	Ptr<VRAMNode> VRAMFreeList::MergeWithNext(Ptr<VRAMNode> target)
	{
		if (target == nullptr || target->next == nullptr || !target->next->isFree)
			return target;

		// 병합 조건 만족: 오른쪽 노드를 흡수
		Ptr<VRAMNode> nextNode = target->next;
		target->size += nextNode->size;
		target->next = nextNode->next;
		if (target->next != nullptr)
			target->next->prev = target;

		delete nextNode;
		return target;
	}

	Ptr<VRAMNode> VRAMFreeList::MergeWithPrev(Ptr<VRAMNode> target)
	{
		if (target == nullptr || target->prev == nullptr || !target->prev->isFree)
			return target;

		// 병합 조건 만족: 왼쪽 노드로 흡수되고 현재 노드는 삭제
		Ptr<VRAMNode> prevNode = target->prev;
		prevNode->size += target->size;
		prevNode->next = target->next;
		if (target->next != nullptr)
			target->next->prev = prevNode;

		delete target;
		return prevNode;
	}
}