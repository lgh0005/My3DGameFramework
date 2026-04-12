#pragma once

namespace MGF3D
{
	inline constexpr ObjectIDHash::ObjectIDHash() : Super(0) { }
	inline constexpr ObjectIDHash::ObjectIDHash(uint32 val) : Super(val) { }
	inline constexpr ObjectIDHash::ObjectIDHash(uint32 index, uint32 generation)
		: Super(((generation << GEN_SHIFT)& GEN_MASK) | (index & INDEX_MASK)) { }

	inline uint32 ObjectIDHash::GetIndex() const
	{
		return value & INDEX_MASK;
	}

	inline uint32 ObjectIDHash::GetGeneration() const
	{
		return (value & GEN_MASK) >> GEN_SHIFT;
	}
}