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