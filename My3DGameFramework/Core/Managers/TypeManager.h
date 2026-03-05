#pragma once
#include "Type/MGFType.h"

namespace MGF3D
{
	class TypeManager
	{
		MGF_DECLARE_SINGLE(TypeManager)

		using TypeToken = struct PendingType 
		{ 
			MGFName name; 
			MGFName parentName; 
		};

	private:
		TypeManager();
		~TypeManager();

	public:
		void TagType(const MGFName& name, const MGFName& parentName);
		void BuildHierarchy();
		Ptr<const MGFType> GetType(StringHash id) const;

	private:
		SVector<TypeToken> m_typeTokenList;
		SMap<StringHash, UniquePtr<MGFType>> m_tokenRegistry;
	};
}