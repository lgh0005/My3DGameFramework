#pragma once
#include "Parsers/JsonParser.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(ResourceDesc)
#pragma endregion

CLASS_PTR(ResourceConfigParser)
class ResourceConfigParser : public JsonParser
{
    using ExtensionMap = std::unordered_map<std::string, std::string>;
    using AssetRegistry = std::unordered_map<std::string, ResourceDescPtr>;

public:
    ResourceConfigParser();
    virtual ~ResourceConfigParser() override;
    ExtensionMap ParseExtensionMap(const std::string& rootKey = "ResourceExtensions");
    AssetRegistry ScanDirectory(const std::string& alias, const std::string& actualRoot);

private:
    ExtensionMap m_extensionMap;
    AssetRegistry m_assetRegistry;
};