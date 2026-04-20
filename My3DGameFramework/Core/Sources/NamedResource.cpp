#include "CorePch.h"
#include "NamedResource.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	NamedResource::NamedResource(StringView name) : m_name(name) { }
	NamedResource::~NamedResource() = default;

	/*==============================//
	//       NamedResource Type     //
	//==============================*/
	int16 NamedResource::s_typeIndex = -1;
	const MGFType* NamedResource::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	const String& NamedResource::GetResourceName() const
	{
		return m_name;
	}

	StringHash NamedResource::GetResourceNameHash() const
	{
		return StringHash(m_name);
	}
}