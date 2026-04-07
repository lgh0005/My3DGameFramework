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
		int16 FindIndex(TypeHash id) const;
		const String& GetTreeName() const;
		const MGFType* GetType(int16 index) const;
		void Bake();

	private:
		String m_treeName;
		Array<MGFType, MAX_TYPE_COUNT> m_types;
		uint32 m_currentCount{ 0 };
	};
}