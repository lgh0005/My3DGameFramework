#include "EnginePch.h"
#include "YamlParser.h"

DECLARE_DEFAULTS_IMPL(YamlParser)

bool YamlParser::LoadFromYamlFile(const std::string& path)
{
	// 1. 기존 데이터 초기화
	m_tree.clear();
	m_source.clear();

	// 2. 파일 열기 (바이너리 모드 + 끝 위치로 이동하여 크기 확인)
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file.is_open()) return false;

	// 3. 파일 크기 가져오기
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	if (size <= 0) return false;

	// 4. 버퍼 리사이징 및 읽기
	m_source.resize(static_cast<usize>(size));
	if (!file.read(m_source.data(), size)) return false;

	// 5. 파싱 (RapidYAML 핵심)
	m_tree = ryml::parse_in_place
	(
		c4::to_csubstr(path.c_str()),
		c4::to_substr(m_source)
	);

	// 6. 유효성 검사 (트리가 비어있지 않고, 루트가 Map/Dict 형태인지)
	if (m_tree.empty() || !m_tree.rootref().is_map())
		return false;

	return true;
}

bool YamlParser::IsArray(const std::string& key) const
{
	if (m_tree.empty()) return false;

	ryml::ConstNodeRef root = m_tree.rootref();
	c4::csubstr k = c4::to_csubstr(key);

	// 1. 키가 있는지 확인 (has_child)
	// 2. 해당 노드가 시퀀스(Sequence = Array)인지 확인 (is_seq)
	return root.has_child(k) && root[k].is_seq();
}

ryml::ConstNodeRef YamlParser::GetRoot() const
{
	if (m_tree.empty()) return {};
	return m_tree.rootref();
}