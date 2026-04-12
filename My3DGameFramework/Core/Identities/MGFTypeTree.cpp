#include "CorePch.h"
#include "MGFTypeTree.h"

namespace MGF3D
{
	MGFTypeTree::MGFTypeTree(StringView treeName) : m_treeName(treeName) { }
	MGFTypeTree::~MGFTypeTree() = default;

	int16 MGFTypeTree::Register(StringView name, StringView parentName)
	{
		// 1. Vector의 현재 크기가 새로 부여될 인덱스입니다.
		int16 newIndex = static_cast<int16>(m_types.size());

		// 2. 새 타입 객체를 생성하고 데이터를 채웁니다.
		MGFType newType;
		newType.id = TypeHash(name);
		newType.typeName = name.data();
		newType.selfIndex = newIndex;
		newType.ownerTree = this;

		// 3. 부모가 있다면 족보(Chain)와 깊이(Depth)를 즉시 물려받습니다.
		if (!parentName.empty())
		{
			newType.parentIndex = FindIndex(TypeHash(parentName));
			if (newType.parentIndex != -1)
			{
				const MGFType& parent = m_types[newType.parentIndex];
				newType.depth = parent.depth + 1;
				if (newType.depth <= MAX_TYPE_DEPTH)
				{
					for (int32 d = 0; d <= parent.depth; ++d)
						newType.chain[d] = parent.chain[d];
				}
			}
			else
			{
				newType.depth = 0;
			}
		}
		else
		{
			newType.parentIndex = -1;
			newType.depth = 0;
		}

		// 4. 자기 자신을 족보 체인의 끝자락에 등록합니다.
		if (newType.depth < MAX_TYPE_DEPTH)
			newType.chain[newType.depth] = newType.id;

		// 5. Vector에 추가합니다.
		m_types.push_back(newType);

		// 6. 부모의 트리 구조(FirstChild / NextSibling)를 연결
		// push_back 이후이므로 안전하게 인덱스로 원본 배열에 접근합니다.
		if (m_types[newIndex].parentIndex != -1)
		{
			int16 parentIdx = m_types[newIndex].parentIndex;
			if (m_types[parentIdx].firstChildIndex == -1)
			{
				// 부모의 첫 자식인 경우
				m_types[parentIdx].firstChildIndex = newIndex;
			}
			else
			{
				// 이미 자식이 있다면, 형제들의 맨 끝을 찾아 연결합니다.
				int16 siblingIdx = m_types[parentIdx].firstChildIndex;
				while (m_types[siblingIdx].nextSiblingIndex != -1)
					siblingIdx = m_types[siblingIdx].nextSiblingIndex;
				m_types[siblingIdx].nextSiblingIndex = newIndex;
			}
		}

		MGF_LOG_INFO("MGFTypeTree: Registered Type '{0}' (Index: {1}, Depth: {2})", name, newIndex, m_types[newIndex].depth);
		return newIndex;
	}

	bool MGFTypeTree::IsA(int16 objectIndex, int16 targetIndex) const
	{
		// 1. 인덱스로 두 타입의 포인터를 안전하게 가져옴 (범위 초과 시 nullptr 반환됨)
		const MGFType* objType = GetType(objectIndex);
		const MGFType* targetType = GetType(targetIndex);

		// 2. 둘 중 하나라도 유효하지 않으면 실패
		if (!objType || !targetType)
			return false;

		// 3. 실제 O(1) 족보 검사는 MGFType 객체에게 위임
		return objType->IsA(*targetType);
	}

	int16 MGFTypeTree::FindIndex(TypeHash id) const
	{
		for (size_t i = 0; i < m_types.size(); ++i)
		{
			if (m_types[i].id == id)
				return static_cast<int16>(i);
		}
		return -1;
	}

	const String& MGFTypeTree::GetTreeName() const
	{
		return m_treeName;
	}

	const MGFType* MGFTypeTree::GetType(int16 index) const
	{
		if (!CommonUtils::IsBetween<int16>(index, 0, static_cast<int16>(m_types.size())))
			return nullptr;
		return &m_types[index];
	}
}