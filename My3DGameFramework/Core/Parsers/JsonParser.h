#pragma once
#include <nlohmann/json.hpp>

namespace MGF3D
{
	MGF_CLASS_PTR(JsonParser)
	class JsonParser
	{
	public:
		JsonParser();
		~JsonParser();

	public:
		bool LoadFromJsonFile(const String& path);
		bool IsArray(const String& key) const;
		const nlohmann::json& GetRoot() const { return m_data; }

		template<typename T>
		bool HasTypeOf(const String& key);

		template<typename T>
		T Get(const String& key, const T& defaultValue = T());

	private:
		nlohmann::json m_data;
	};
}

#include "Parsers/JsonParser.inl"