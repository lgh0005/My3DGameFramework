#include "CorePch.h"
#include "Resource.h"
#include "Identity/MGFType.h"

namespace MGF3D
{
    Resource::Resource(const IResourceDescriptor& desc)
        : Super(desc.type, desc.name) { }
    Resource::~Resource() = default;

    /*==================================//
    //         Type System              //
    //==================================*/
    UniquePtr<MGFType> Resource::s_type = nullptr;

	void Resource::InitializeType()
	{
		s_type = MakeUnique<MGFType>();
		s_type->name = MGFName("Resource");
		s_type->id = StringHash("Resource");

		// 최상위 루트이므로 부모 없음
		s_type->parent = nullptr;
		s_type->depth = 0;
		s_type->chain[0] = s_type->id;
	}

	const MGFType* Resource::GetType() const
	{
		return s_type.Get();
	}



}