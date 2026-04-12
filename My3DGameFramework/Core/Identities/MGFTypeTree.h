#pragma once
#include "Constants/ConstantType.h"

namespace MGF3D
{
	MGF_CLASS_PTR(MGFTypeTree)
	class MGFTypeTree
	{
	public:
		MGFTypeTree(StringView treeName);
		~MGFTypeTree();

	public:
		int16 Register(StringView name, StringView parentName = "");
		bool IsA(int16 objectIndex, int16 targetIndex) const;
		int16 FindIndex(TypeHash id) const;

	public:
		const String& GetTreeName() const;
		const MGFType* GetType(int16 index) const;
		usize GetTypeCount() const { return m_types.size(); }

	private:
		String m_treeName;
		Vector<MGFType> m_types;
	};
}