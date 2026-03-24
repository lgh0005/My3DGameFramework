#pragma once
#include "Identity/MGFType.h"

namespace MGF3D
{
	MGF_STRUCT_PTR(MGFType)

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
		SMap<StringHash, MGFTypeUPtr> m_tokenRegistry;
	};
}