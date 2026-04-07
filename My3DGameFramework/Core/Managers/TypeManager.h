#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(MGFTypeTree)

	class TypeManager
	{
		MGF_DECLARE_SINGLE(TypeManager)

	private:
		TypeManager();
		~TypeManager();

	public:
		MGFTypeTree& CreateTree(StringView treeName);
		MGFTypeTree* GetTree(StringView treeName);
		void BakeAll();

	private:
		HashMap<StringHash, MGFTypeTreeUPtr> m_typeTrees;
	};
}