#pragma once

namespace MGF3D
{
	template<typename HashFucntor, typename T = uint64>
	struct IHashFunctor
	{
		T value { 0 };

		/*==============================//
		//   HashFunctor constrcutors   //
		//==============================*/
		constexpr IHashFunctor() : value(0) {}
		explicit constexpr IHashFunctor(T h) : value(h) {}

		/*======================================================//
		//   HashFunctor operator overloads for unordered STL   //
		//======================================================*/
		bool operator==(const HashFucntor& other) const { return value == other.value; }
		bool operator!=(const HashFucntor& other) const { return value != other.value; }
		bool operator<(const HashFucntor& other) const { return value < other.value; }
		constexpr operator T() const { return value; }

		/*=========================//
		//   HashFunctor Utility   //
		//=========================*/
		bool IsValid() const { return value != 0; }
	};
}