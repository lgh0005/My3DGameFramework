#pragma once
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>

namespace MGF3D
{
	// std::array alias
	template <typename T, usize N> using Array = std::array<T, N>;

	// std::vector alias
	template <typename T> using Vector = std::vector<T>;

	// std::deque alias
	template <typename T> using Queue = std::deque<T>;

	// std::unordered_set alias
	template <typename T> using HashSet = std::unordered_set<T>;

	// std::unordered_map alias
	template <typename K, typename V> using HashMap = std::unordered_map<K, V>;
}