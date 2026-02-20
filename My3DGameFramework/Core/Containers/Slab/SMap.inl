#pragma once

namespace MGF3D
{
	template<typename K, typename V, typename Hash, typename KeyEqual>
	inline usize SMap<K, V, Hash, KeyEqual>::MemoryUsage() const
	{
		// 컨테이너 자체 + 버킷 포인터 배열 + 노드 오버헤드(데이터 및 포인터)
		usize usage = sizeof(*this);
		usage += this->bucket_count() * sizeof(void*);
		usage += this->size() * (sizeof(std::pair<const K, V>) + (sizeof(void*) * 2));
		return usage;
	}

	template<typename K, typename V, typename Hash, typename KeyEqual>
	inline V* SMap<K, V, Hash, KeyEqual>::Find(const K& key)
	{
		auto it = this->find(key);
		if (it != this->end()) return &it->second;
		return nullptr;
	}

	template<typename K, typename V, typename Hash, typename KeyEqual>
	inline const V* SMap<K, V, Hash, KeyEqual>::Find(const K& key) const
	{
		auto it = this->find(key);
		if (it != this->end()) return &it->second;
		return nullptr;
	}

	template<typename K, typename V, typename Hash, typename KeyEqual>
	inline bool SMap<K, V, Hash, KeyEqual>::Insert(const K& key, const V& value)
	{
		auto result = this->emplace(key, value);
		return result.second;
	}

	template<typename K, typename V, typename Hash, typename KeyEqual>
	inline bool SMap<K, V, Hash, KeyEqual>::Remove(const K& key)
	{
		return this->erase(key) > 0;
	}
}