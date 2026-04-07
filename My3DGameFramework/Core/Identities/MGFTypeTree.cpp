#include "CorePch.h"
#include "MGFTypeTree.h"

namespace MGF3D
{
	MGFTypeTree::MGFTypeTree(StringView treeName) : m_treeName(treeName) { }
	MGFTypeTree::~MGFTypeTree() = default;

	int16 MGFTypeTree::Register(StringView name, StringView parentName)
	{
		// 1. 최대 개수 초과 검사
		if (m_currentCount >= MAX_TYPE_COUNT)
		{
			MGF_LOG_ERROR("MGFTypeTree: Registration failed. Max type count exceeded.");
			return -1;
		}

		// 2. 새 타입 생성 및 기초 정보 기입
		int16 newIndex = static_cast<int16>(m_currentCount++);
		MGFType& type = m_types[newIndex];

		type.id = TypeHash(name);
		type.typeName = name.data();
		type.selfIndex = newIndex;

		if (!parentName.empty()) type.parentIndex = FindIndex(TypeHash(parentName));
		else type.parentIndex = -1;

		return newIndex;
	}

	int16 MGFTypeTree::FindIndex(TypeHash id) const
	{
		for (uint32 i = 0; i < m_currentCount; ++i)
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
		if (!CommonUtils::IsBetween<int16>(index, 0, static_cast<int16>(m_currentCount)))
			return nullptr;

		return &m_types[index];
	}

	void MGFTypeTree::Bake()
	{
		for (uint32 i = 0; i < m_currentCount; ++i)
		{
			MGFType& type = m_types[i];

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
				for (uint32 d = 0; d < parent.depth + 1; ++d)
					type.chain[d] = parent.chain[d];
			}
			else
			{
				type.depth = 0;
			}

			// 3. 자기 자신을 족보의 해당 깊이에 등록
			if (type.depth < MAX_TYPE_DEPTH)
				type.chain[type.depth] = type.id;
		}

		MGF_LOG_INFO("MGFTypeTree: Baked {0} types successfully.", m_currentCount);
	}
}