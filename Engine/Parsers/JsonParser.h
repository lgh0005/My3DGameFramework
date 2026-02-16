#pragma once

CLASS_PTR(JsonParser)
class JsonParser
{
public:
	JsonParser();
	~JsonParser();

	bool LoadFromJsonFile(const std::string& path);
	bool IsArray(const std::string& key) const;
	const nlohmann::json& GetRoot() const { return m_data; }

	template<typename T>
	T Get(const std::string& key, const T& defaultValue = T());

private:
	nlohmann::json m_data;
};

#include "Parsers/JsonParser.inl"