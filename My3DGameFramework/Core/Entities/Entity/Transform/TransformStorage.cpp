#include "CorePch.h"
#include "TransformStorage.h"
#include "Utils/TransformUtils.h"

namespace MGF3D
{
	TransformStorage::TransformStorage() = default;
	TransformStorage::~TransformStorage() = default;

	usize TransformStorage::CreateTransform
	(
		const Position& pos, 
		const Rotation& rot, 
		const Scale& scale
	)
	{
		Transform t;
		t.m_localPosition = pos;
		t.m_localRotation = rot;
		t.m_localScale = scale;
		t.m_isLocalTransformDirty = true;
		t.m_isWorldTransformDirty = true;
		t.m_parentIndex = -1;

		usize newIndex = m_rootCount;
		if (m_rootCount == m_transforms.Count())
		{
			// 자식이 아직 하나도 없거나 첫 생성인 경우, 
			// 배열 맨 뒤가 Root 구역의 끝
			m_transforms.PushBack(Move(t));
		}
		else
		{
			// 이미 자식들이 존재한다면, 
			// m_rootCount 인덱스에 있는 '자식'을 배열 맨 뒤로 밀어냅니다.
			// 그리고 확보된 m_rootCount 자리에 새로운 Root(t)를 넣습니다.
			usize childToMoveIndex = m_rootCount;
			m_transforms.PushBack(Move(m_transforms[childToMoveIndex]));
			m_transforms[childToMoveIndex] = Move(t);

			// 뒤로 밀려난 자식의 인덱스가 변경되었으므로, 해당 자식과 연결된 부모/자식의 참조를 갱신합니다.
			usize newChildIndex = m_transforms.Count() - 1;
			UpdateIndexReferences(childToMoveIndex, newChildIndex);
		}

		m_rootCount++;
		return newIndex;
	}

	void TransformStorage::UpdateIndexReferences(usize oldIndex, usize newIndex)
	{
		Transform& movedTransform = m_transforms[newIndex];

		// 1. 내 부모의 자식 목록에서 내 인덱스를 새 인덱스로 갱신
		usize invalidIndex = -1;
		if (movedTransform.m_parentIndex != invalidIndex)
		{
			auto& parent = m_transforms[movedTransform.m_parentIndex];
			for (usize& childIdx : parent.m_childrenIndices)
			{
				if (childIdx == oldIndex)
				{
					childIdx = newIndex;
					break;
				}
			}
		}

		// 2. 내 자식들의 부모 인덱스를 새 인덱스로 갱신
		for (usize childIdx : movedTransform.m_childrenIndices)
			m_transforms[childIdx].m_parentIndex = newIndex;
	}

	void TransformStorage::UpdateWorldMatrices()
	{
		// 1. Root 구역 업데이트
		for (usize i = 0; i < m_rootCount; ++i)
		{
			Transform& t = m_transforms[i];

			if (t.m_isLocalTransformDirty)
			{
				t.m_localMatrix = TransformUtils::ComputeLocalMatrix(t.m_localPosition, t.m_localRotation, t.m_localScale);
				t.m_worldMatrix = t.m_localMatrix;

				t.m_isLocalTransformDirty = false;
				t.m_isWorldTransformDirty = true;
			}
			else
			{
				t.m_isWorldTransformDirty = false;
			}
		}

		// 2. Child 구역 업데이트
		const usize totalCount = m_transforms.Count();
		for (usize i = m_rootCount; i < totalCount; ++i)
		{
			Transform& t = m_transforms[i];
			const Transform& parent = m_transforms[t.m_parentIndex];

			// 내가 변했거나, 내 부모가 방금 위의 루프에서 변했다면 갱신
			if (t.m_isLocalTransformDirty || parent.m_isWorldTransformDirty)
			{
				if (t.m_isLocalTransformDirty)
				{
					t.m_localMatrix = TransformUtils::ComputeLocalMatrix(t.m_localPosition, t.m_localRotation, t.m_localScale);
					t.m_isLocalTransformDirty = false;
				}

				// 부모 행렬 적용
				t.m_worldMatrix = TransformUtils::CombineMatrices(parent.m_worldMatrix, t.m_localMatrix);
				t.m_isWorldTransformDirty = true;
			}
			else
			{
				t.m_isWorldTransformDirty = false;
			}
		}
	}

	void TransformStorage::SetParent(usize childIndex, usize parentIndex)
	{
		usize invalidIndex = -1;

		// 예외 처리
		if (childIndex == parentIndex || childIndex == invalidIndex) return;

		Transform& child = m_transforms[childIndex];
		usize oldParentIndex = child.m_parentIndex;

		if (oldParentIndex == parentIndex) return;

		// 1. 기존 부모에서 분리 (Detach)
		// 1. 기존 부모에서 분리 (Detach)
		if (oldParentIndex != invalidIndex)
		{
			auto& children = m_transforms[oldParentIndex].m_childrenIndices;
			for (usize i = 0; i < children.Count(); ++i) // .Size() 혹은 .Count()
			{
				if (children[i] == childIndex)
				{
					// 찾은 인덱스를 전달하여 O(1) 삭제 수행
					children.RemoveSwap(i);
					break; // 하나만 지우면 되므로 즉시 탈출
				}
			}
		}

		// 2. 새 부모에 연결 (Attach)
		child.m_parentIndex = parentIndex;
		if (parentIndex != invalidIndex)
		{
			m_transforms[parentIndex].m_childrenIndices.PushBack(childIndex);
		}

		child.m_isWorldTransformDirty = true;

		// 3. 파티션 이동 로직 (Swap & Pop 방식)
		bool wasRoot = (oldParentIndex == invalidIndex);
		bool isNowRoot = (parentIndex == invalidIndex);

		if (wasRoot && !isNowRoot)
		{
			// Root -> Child 강등
			// Root 구역의 마지막 요소와 자리를 바꾼 뒤, Root 구역의 크기를 줄입니다.
			usize lastRootIndex = m_rootCount - 1;
			if (childIndex != lastRootIndex)
			{
				Transform temp = Move(m_transforms[childIndex]);
				m_transforms[childIndex] = Move(m_transforms[lastRootIndex]);
				m_transforms[lastRootIndex] = Move(temp);

				UpdateIndexReferences(childIndex, lastRootIndex);
				UpdateIndexReferences(lastRootIndex, childIndex);

				childIndex = lastRootIndex;
			}
			m_rootCount--;
		}
		else if (!wasRoot && isNowRoot)
		{
			// Child -> Root 승격
			// Child 구역의 첫 번째 요소(현재 m_rootCount 위치)와 자리를 바꾼 뒤, Root 구역을 늘립니다.
			usize firstChildIndex = m_rootCount;
			if (childIndex != firstChildIndex)
			{
				Transform temp = Move(m_transforms[childIndex]);
				m_transforms[childIndex] = Move(m_transforms[firstChildIndex]);
				m_transforms[firstChildIndex] = Move(temp);

				UpdateIndexReferences(childIndex, firstChildIndex);
				UpdateIndexReferences(firstChildIndex, childIndex);

				childIndex = firstChildIndex;
			}
			m_rootCount++;
		}

		// 4. 위상 정렬 필요 여부 체크
		// 자식의 인덱스가 부모의 인덱스보다 작아졌다면, 다음 프레임 업데이트 전에 정렬을 예약합니다.
		if (parentIndex != invalidIndex && childIndex < parentIndex)
		{
			m_needsTopologicalSort = true;
		}
	}

	void TransformStorage::SortTopologically()
	{
		if (!m_needsTopologicalSort) return;

		SVector<Transform> sortedTransforms;
		sortedTransforms.Reserve(m_transforms.Count());

		// 과거 인덱스가 새 배열에서 몇 번 인덱스로 갔는지 기록하는 지도
		usize invalidIndex = -1;
		usize count = m_transforms.Count();
		SVector<usize> oldToNew;
		oldToNew.Resize(count, invalidIndex);

		// 1. DFS 방식으로 트리 순회하며 새 배열에 차곡차곡 담기 (람다 함수 활용)
		auto dfs = [&](auto& self, usize oldIdx) -> void 
		{
			usize newIdx = sortedTransforms.Count();
			oldToNew[oldIdx] = newIdx;
			sortedTransforms.PushBack(Move(m_transforms[oldIdx]));

			for (usize childOldIdx : sortedTransforms.Back().m_childrenIndices) 
			{
				self(self, childOldIdx);
			}
		};

		// 2. Root 구역(0 ~ m_rootCount-1)에 있던 애들부터 탐색 시작
		for (usize i = 0; i < m_rootCount; ++i) 
		{
			// 이미 방문한 노드는 스킵 (안전장치)
			if (oldToNew[i] == invalidIndex) 
			{
				dfs(dfs, i);
			}
		}

		// 3. 인덱스 관계망 일괄 갱신 (이사 간 새 주소로 업데이트)
		for (auto& t : sortedTransforms) 
		{
			if (t.m_parentIndex != invalidIndex)
				t.m_parentIndex = oldToNew[t.m_parentIndex];

			for (usize& childIdx : t.m_childrenIndices)
				childIdx = oldToNew[childIdx];
		}

		// 4. 새 배열로 교체 완료
		m_transforms = Move(sortedTransforms);
		m_needsTopologicalSort = false;
	}
}