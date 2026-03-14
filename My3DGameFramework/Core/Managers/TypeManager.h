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
		Ptr<const MGFType> GetType(StringHash id) const;

	public:
		void InitTypeObjects();
		void ResolveTypeHierarchy();
		void BuildHierarchy();

	private:
		bool ResolveSpecificType(const TypeToken& token);

	private:
		SVector<TypeToken> m_typeTokenList;
		SMap<StringHash, UniquePtr<MGFType>> m_tokenRegistry;
	};
}