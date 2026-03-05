#pragma once
#include "Identity/MGFName.h"

namespace MGF3D
{
	struct MGFType
	{
		MGFName name;
		StringHash id;
		Ptr<const MGFType> parent { nullptr };

		static constexpr uint32 MAX_DEPTH = 16;
		uint32 depth = 0;
		StringHash chain[MAX_DEPTH] = { 0 };

		bool IsA(const MGFType* other) const;
	};
}