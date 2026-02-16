#pragma once
#include <c4/yml/yml.hpp>
#include <c4/yml/std/string.hpp>
namespace ryml = c4::yml;

CLASS_PTR(YamlParser)
class YamlParser
{
public:
	YamlParser();
	~YamlParser();

	bool LoadFromYamlFile(const std::string& path);
	bool IsArray(const std::string& key) const;
	ryml::ConstNodeRef GetRoot() const;

	template<typename T>
	bool IsTypeOf(const std::string& key) const;

	template<typename T>
	T Get(const std::string& key, const T& defaultValue = T());

private:
	std::string m_source;
	ryml::Tree m_tree;
};

#include "Parsers/YamlParser.inl"