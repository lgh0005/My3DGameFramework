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

		if (!parentName.empty()) newType.parentIndex = FindIndex(TypeHash(parentName));
		else newType.parentIndex = -1;

		// 3. Vector에 추가합니다.
		m_types.push_back(newType);
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

	void MGFTypeTree::Bake()
	{
		m_types.shrink_to_fit();

		for (usize i = 0; i < m_types.size(); ++i)
		{
			MGFType& type = m_types[i];
			type.ownerTree = this;

			// 1. 트리 구조 (FirstChild / NextSibling) 연결
			if (type.parentIndex != -1)
			{
				MGFType& parent = m_types[type.parentIndex];
				if (parent.firstChildIndex == -1)
				{
					parent.firstChildIndex = type.selfIndex;
				}
				else
				{
					// 형제들 끝에 붙이기
					int16 siblingIdx = parent.firstChildIndex;
					while (m_types[siblingIdx].nextSiblingIndex != -1)
						siblingIdx = m_types[siblingIdx].nextSiblingIndex;
					m_types[siblingIdx].nextSiblingIndex = type.selfIndex;
				}

				// 2. 깊이 설정 및 족보(chain) 전파
				type.depth = parent.depth + 1;

				// 부모의 족보를 그대로 복사 (MAX_TYPE_DEPTH까지만)
				if (parent.depth + 1 <= MAX_TYPE_DEPTH)
				{
					for (int32 d = 0; d < parent.depth + 1; ++d)
						type.chain[d] = parent.chain[d];
				}
			}
			else
			{
				type.depth = 0;
			}

			// 3. 자기 자신을 족보의 해당 깊이에 등록
			if (type.depth < MAX_TYPE_DEPTH)
				type.chain[type.depth] = type.id;
		}

		MGF_LOG_INFO("MGFTypeTree: Baked {0} types successfully.", m_types.size());
	}
}