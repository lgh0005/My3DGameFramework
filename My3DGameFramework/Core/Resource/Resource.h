#pragma once
#include "Entities/Entity/Waitables/IWaitable.h"
#include "Resource/IResourceDescriptor.h"

namespace MGF3D
{
	MGF_CLASS_PTR(MGFType)

	MGF_CLASS_PTR(Resource)
	class Resource : public IWaitable
	{
		using Super = IWaitable;

	public:
		Resource(const IResourceDescriptor& desc);
		virtual ~Resource() override;
	};
}