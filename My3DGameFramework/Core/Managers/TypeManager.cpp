#include "CorePch.h"
#include "TypeManager.h"
#include "Identities/MGFTypeTree.h"

namespace MGF3D
{
	TypeManager::TypeManager() = default;
	TypeManager::~TypeManager() = default;

	MGFTypeTree& TypeManager::CreateTree(StringView treeName)
	{
		StringHash treeId(treeName);
		auto it = m_typeTrees.find(treeId);
		if (it != m_typeTrees.end()) return *(it->second);

		// 새 트리 생성 (UniquePtr로 안전하게 소유)
		auto newTree = MakeUnique<MGFTypeTree>(treeName);
		MGFTypeTree& treeRef = *newTree;
		m_typeTrees[treeId] = std::move(newTree);

		MGF_LOG_INFO("TypeManager: New TypeTree created - {0}", treeName);
		return treeRef;
	}

	MGFTypeTree* TypeManager::GetTree(StringView treeName)
	{
		StringHash treeId(treeName);
		auto it = m_typeTrees.find(treeId);
		if (it == m_typeTrees.end()) return nullptr;
		return it->second.get();
	}

	void TypeManager::BakeAll()
	{
		MGF_LOG_INFO("TypeManager: Start baking all type trees...");
		for (auto& [hash, tree] : m_typeTrees) tree->Bake();
	}
}