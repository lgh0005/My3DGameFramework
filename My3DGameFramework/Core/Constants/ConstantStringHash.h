#pragma once

namespace MGF3D
{
	/*===================================//
	//     FNV-1a string hash constants  //
	//===================================*/
	static constexpr uint32 OFFSET_BASIS	{ 2166136261u };
	static constexpr uint32 PRIME		    { 16777619u };
	static constexpr uint64 OFFSET_BASIS_64 { 0xcbf29ce484222325ULL };
	static constexpr uint64 PRIME_64		{ 0x100000001b3ULL };
}