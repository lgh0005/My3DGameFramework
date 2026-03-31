#pragma once

namespace MGF3D
{
	inline constexpr InstanceIDHash::InstanceIDHash(uint32 index, uint32 generation)
		: IHashFunctor<InstanceIDHash, uint64>((static_cast<uint64>(generation) << 32) | index) { }

	inline constexpr uint32 InstanceIDHash::GetIndex() const
	{
		return static_cast<uint32>(value & 0xFFFFFFFF);
	}

	inline constexpr uint32 InstanceIDHash::GetGeneration() const
	{
		return static_cast<uint32>((value >> 32) & 0xFFFFFFFF);
	}
}