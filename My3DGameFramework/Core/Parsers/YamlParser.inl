#pragma once

namespace MGF3D
{
	template<typename T>
	inline bool YamlParser::HasTypeOf(const SString& key) const
	{
		ryml::ConstNodeRef node = GetNode(key);
		if (!node.valid()) return false;

		// 1. 구조적 타입 체크
		if constexpr (std::is_same_v<T, ryml::ConstNodeRef>) 
			return node.is_map();

		else if constexpr (std::is_same_v<T, vec3> || std::is_same_v<T, vec4>) 
			return node.is_seq();

		// 2. 스칼라 데이터 체크
		else if constexpr (std::is_arithmetic_v<T> || std::is_same_v<T, str>)
			return node.has_val();

		return false;
	}

	template<typename T>
	inline T YamlParser::Get(const SString& key, const T& defaultValue) const
	{
		ryml::ConstNodeRef node = GetNode(key);

		// 노드가 유효하지 않거나 값을 가지고 있지 않으면 기본값 반환
		if (!node.valid() || !node.has_val()) return defaultValue;

		T value = defaultValue;
		node >> value;

		return value;
	}

	/*===============================//
	//   OpenGL glm specialization   //
	//===============================*/
	template<>
	inline vec3 YamlParser::Get<vec3>(const SString& key, const vec3& defaultValue) const
	{
		ryml::ConstNodeRef node = GetNode(key);
		if (!node.valid() || !node.is_seq())
			return defaultValue;

		vec3 result = defaultValue;
		usize numChildren = (usize)node.num_children();
		usize count = Math::Min(numChildren, (usize)3);

		for (usize i = 0; i < count; ++i)
		{
			if (node[i].has_val())
				node[i] >> result[static_cast<len_t>(i)];
		}

		return result;
	}

	template<>
	inline vec4 YamlParser::Get<vec4>(const SString& key, const vec4& defaultValue) const
	{
		ryml::ConstNodeRef node = GetNode(key);
		if (!node.valid() || !node.is_seq())
			return defaultValue;

		glm::vec4 result = defaultValue;
		usize numChildren = (usize)node.num_children();
		usize count = Math::Min(numChildren, (usize)4);

		for (usize i = 0; i < count; ++i)
		{
			if (node[i].has_val())
				node[i] >> result[static_cast<len_t>(i)];
		}

		return result;
	}
}