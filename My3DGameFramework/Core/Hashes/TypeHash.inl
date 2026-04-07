#pragma once

namespace MGF3D
{
	inline constexpr TypeHash operator""_th(cstr s, usize n)
	{
		return TypeHash{ StringView{ s, n } };
	}

	constexpr TypeHash::TypeHash(StringView sv)
		: IHashFunctor<TypeHash>(Calculate(sv.data(), sv.size())) { }

	constexpr uint64 TypeHash::Calculate(cstr str, usize len)
	{
		uint64 hash = OFFSET_BASIS_64;

		for (usize i = 0; i < len; ++i)
		{
			hash ^= static_cast<uint64>(str[i]);
			hash *= PRIME_64;
		}

		return hash;
	}
}