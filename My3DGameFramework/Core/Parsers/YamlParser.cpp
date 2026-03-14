#include "CorePch.h"
#include <fstream>
#include "YamlParser.h"

namespace MGF3D
{
	YamlParser::YamlParser() = default;
	YamlParser::~YamlParser() = default;

	bool YamlParser::LoadFromYamlFile(const MGFPath& path)
	{
		// 1. 기존 데이터 초기화
		m_tree.clear();
		m_source.clear();

		// 2. 파일 열기 (바이너리 모드 + 끝 위치로 이동하여 크기 확인)
		std::ifstream file(path.GetCStr(), std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			MGF_LOG_ERROR("YamlParser: Failed to open file: {}", path.GetCStr());
			return false;
		}

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
			c4::to_csubstr(path.GetCStr()),
			c4::to_substr(m_source)
		);

		// 6. 유효성 검사 (트리가 비어있지 않고, 루트가 Map/Dict 형태인지)
		if (m_tree.empty() || !m_tree.rootref().is_map())
		{
			MGF_LOG_ERROR("YamlParser: Invalid YAML or empty root in {}", path.GetCStr());
			return false;
		}

		return true;
	}

	ryml::ConstNodeRef YamlParser::GetNode(const SString& key) const
	{
		if (m_tree.empty()) return ryml::ConstNodeRef();

		ryml::ConstNodeRef root = m_tree.rootref();
		
		c4::csubstr k(key.CStr(), key.Length());
		if (root.has_child(k)) return root[k];

		return ryml::ConstNodeRef();
	}

	bool YamlParser::Contains(const SString& key) const
	{
		return GetNode(key).valid();
	}

	bool YamlParser::IsArray(const SString& key) const
	{
		ryml::ConstNodeRef node = GetNode(key);
		return node.valid() && node.is_seq();
	}

	ryml::ConstNodeRef YamlParser::GetRoot() const
	{
		if (m_tree.empty()) return ryml::ConstNodeRef();
		return m_tree.rootref();
	}
}