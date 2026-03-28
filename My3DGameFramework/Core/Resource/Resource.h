#pragma once
#include "Entities/Entity/Waitables/IWaitable.h"
#include "Resource/IResourceDescriptor.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Resource)
	class Resource : public IWaitable
	{
		using Super = IWaitable;

	/*==================================//
	//         Type System              //
	//==================================*/
	public:
		static UniquePtr<MGFType> s_type;
		static void InitializeType();
		virtual const MGFType* GetType() const;

	public:
		Resource(const IResourceDescriptor& desc);
		virtual ~Resource() override;
	};
}