#pragma once
#include "Identity/MGFName.h"

namespace MGF3D
{
	struct MGFType
	{
		MGFName name;
		StringHash id;
		Ptr<const MGFType> parent { nullptr };

		static constexpr uint32 MAX_DEPTH  { 16 };
		Array<StringHash, MAX_DEPTH> chain { 0 };
		uint32 depth = 0;

		bool IsA(const MGFType* other) const;
	};
}