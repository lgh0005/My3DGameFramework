#pragma once

namespace MGF3D
{
	inline constexpr PathHash operator""_ph(cstr s, usize n)
	{
		return PathHash{ StringView{s, n} };
	}

	inline constexpr uint64 PathHash::Calculate(cstr str, usize len)
	{
		if (str == nullptr || len == 0) return 0;

		uint32 hash = OFFSET_BASIS_64;
		for (usize i = 0; i < len; ++i)
		{
			char c{ str[i] };
			if (CommonUtils::IsBetween<char>(c, 'A', 'Z')) c += 32;
			if (c == '\\') c = '/';
			hash ^= static_cast<uint64>(c);
			hash *= PRIME_64;
		}

		return hash;
	}

	inline constexpr PathHash::PathHash(StringView sv)
		: IHashFunctor<PathHash>(Calculate(sv.data(), sv.size())) {}
}