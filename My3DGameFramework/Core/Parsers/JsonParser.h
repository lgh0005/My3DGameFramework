#pragma once
#include <nlohmann/json.hpp>

namespace MGF3D
{
	MGF_CLASS_PTR(Stream)

	MGF_CLASS_PTR(JsonParser)
	class JsonParser
	{
	public:
		JsonParser();
		~JsonParser();

	public:
		bool LoadFromJsonFile(const MGFPath& path);
		bool LoadFromStream(StreamPtr stream);

	public:
		bool IsArray(const SString& key) const;
		bool Contains(const SString& key) const;
		const nlohmann::json& GetRoot() const { return m_data; }

		template<typename T>
		bool HasTypeOf(const SString& key) const;

		template<typename T>
		T Get(const SString& key, const T& defaultValue = T()) const;

	private:
		nlohmann::json m_data;
	};
}

#include "Parsers/JsonParser.inl"