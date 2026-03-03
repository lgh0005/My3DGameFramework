#pragma once

template<typename T>
inline T YamlParser::Get(const std::string& key, const T& defaultValue)
{
	// 1. 트리가 비어있거나 루트가 유효하지 않으면 기본값 반환
	if (m_tree.empty()) return defaultValue;


	ryml::ConstNodeRef root = m_tree.rootref();
	c4::csubstr k = c4::to_csubstr(key);

	// 2. 해당 키(자식 노드)가 존재하는지 확인
	if (!root.has_child(k)) return defaultValue;

	// 3. 노드가 값을 가지고 있는지 확인
	ryml::ConstNodeRef node = root[k];
	if (!node.has_val()) return defaultValue;

	// 4. 값 추출 (예외 없이 동작)
	// ryml의 >> 연산자는 타입에 맞는 역직렬화를 수행합니다.
	// 변환에 실패하더라도 예외를 던지지 않습니다.
	T value = defaultValue;
	node >> value;

	return value;
}

template<typename T>
inline bool YamlParser::IsTypeOf(const std::string& key) const
{
	if (m_tree.empty()) return false;

	ryml::ConstNodeRef root = m_tree.rootref();
	c4::csubstr k = c4::to_csubstr(key);

	if (!root.has_child(k)) return false;

	ryml::ConstNodeRef node = root[k];

	// 1. 구조적 타입 확인
	if constexpr (std::is_same_v<T, ryml::ConstNodeRef> || std::is_same_v<T, ryml::NodeRef>)
		return node.is_map();
	else if constexpr (std::is_same_v<T, void*>) return node.is_seq();

	// 2. 데이터 타입 확인 (Scalar)
	if constexpr (std::is_arithmetic_v<T> || std::is_same_v<T, std::string>)
		return node.is_val();

	return false;
}

#pragma region OPENGL_SPECIALIZATION

template<>
inline glm::vec4 YamlParser::Get<glm::vec4>
(
	const std::string& key,
	const glm::vec4& defaultValue
)
{
	if (m_tree.empty()) return defaultValue;

	ryml::ConstNodeRef root = m_tree.rootref();
	c4::csubstr k = c4::to_csubstr(key);

	// 1. 해당 키가 존재하지 않거나, 시퀀스(배열)가 아니면 기본값 반환
	if (!root.has_child(k) || !root[k].is_seq())
		return defaultValue;

	ryml::ConstNodeRef node = root[k];
	glm::vec4 result = defaultValue;

	// 2. 최대 4개 성분만 안전하게 파싱 (std::min 활용)
	int32 numElements = static_cast<int32>(node.num_children());
	int32 count = std::min(numElements, 4);

	for (int32 i = 0; i < count; ++i)
	{
		// 각 노드 성분이 값을 가지고 있는지 확인 후 역직렬화
		if (node[i].has_val())
			node[i] >> result[i];
	}
	return result;
}

template<>
inline glm::vec3 YamlParser::Get<glm::vec3>
(
	const std::string& key, 
	const glm::vec3& defaultValue
)
{
	if (m_tree.empty()) return defaultValue;

	ryml::ConstNodeRef root = m_tree.rootref();
	c4::csubstr k = c4::to_csubstr(key);

	if (!root.has_child(k) || !root[k].is_seq())
		return defaultValue;

	ryml::ConstNodeRef node = root[k];
	glm::vec3 result = defaultValue;

	int32 numElements = static_cast<int32>(node.num_children());
	int32 count = std::min(numElements, 3);

	for (int32 i = 0; i < count; ++i)
	{
		if (node[i].has_val())
			node[i] >> result[i];
	}

	return result;
}

#pragma endregion