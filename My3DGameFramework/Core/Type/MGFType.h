#pragma once
#include "Pointer/PoolAlloc.h"
#include "Identity/MGFName.h"

namespace MGF3D
{
	MGF_STRUCT_PTR(MGFType)
	struct MGFType : public PoolAlloc
	{
		MGFType();
		virtual ~MGFType() override;

		MGFName name;
		StringHash id;
		Ptr<const MGFType> parent { nullptr };

		static constexpr uint32 MAX_DEPTH  { 16 };
		Array<StringHash, MAX_DEPTH> chain { 0 };
		uint32 depth = 0;

		bool IsA(const MGFType* other) const;
	};
}