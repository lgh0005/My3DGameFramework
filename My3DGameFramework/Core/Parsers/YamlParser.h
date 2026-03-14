#pragma once
#include <c4/yml/yml.hpp>
#include <c4/yml/std/string.hpp>

namespace MGF3D
{
	namespace ryml = c4::yml;

	class YamlParser
	{
	public:
		YamlParser();
		~YamlParser();

	public:
		bool LoadFromYamlFile(const MGFPath& path);

		ryml::ConstNodeRef GetNode(const SString& key) const;
		bool Contains(const SString& key) const;
		bool IsArray(const SString& key) const;
		ryml::ConstNodeRef GetRoot() const;

	public:
		template<typename T>
		bool HasTypeOf(const SString& key) const;

		template<typename T>
		T Get(const SString& key, const T& defaultValue = T()) const;

	private:
		str m_source;
		ryml::Tree m_tree;
	};
}

#include "Parsers/YamlParser.inl"