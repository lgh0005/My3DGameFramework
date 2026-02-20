#pragma once
#include <unordered_set>
#include "Memory/SlabAllocator.h"
#include "Containers/IContainer.h"

namespace MGF3D
{
	template <typename K, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<K>>
	class SSet : public std::unordered_set<K, Hash, KeyEqual, SlabAllocator<K>>,
				 public IContainer
	{
	public:
		using Base = std::unordered_set<K, Hash, KeyEqual, SlabAllocator<K>>;
		using Base::unordered_set;
		SSet() = default;
		virtual ~SSet() = default;

	public:
		usize MemoryUsage() const override;
		usize Count()   const override { return this->size(); }
		bool  Empty()   const override { return this->empty(); }
		void  Clear()         override { this->clear(); }

	public:
		const K* Find(const K& key) const;
		bool Insert(const K& key);
		bool Remove(const K& key);

	public:
		using iterator = typename Base::iterator;
		using const_iterator = typename Base::const_iterator;

		iterator       begin()        noexcept { return Base::begin(); }
		iterator       end()          noexcept { return Base::end(); }
		const_iterator begin()  const noexcept { return Base::begin(); }
		const_iterator end()    const noexcept { return Base::end(); }
		const_iterator cbegin() const noexcept { return Base::cbegin(); }
		const_iterator cend()   const noexcept { return Base::cend(); }
	};
}

#include "Containers/Slab/SSet.inl"