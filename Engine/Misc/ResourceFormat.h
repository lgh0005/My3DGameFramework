#pragma once

enum class AssetType
{
    Unknown = 0,
    Texture,
    Model,
    Audio,
    Animation
};

NLOHMANN_JSON_SERIALIZE_ENUM
(   
    AssetType, 
    {
        {AssetType::Unknown,   nullptr},
        {AssetType::Texture,   "Texture"},
        {AssetType::Model,     "Model"},
        {AssetType::Audio,     "Audio"},
        {AssetType::Animation, "Animation"}
    }
);

struct AssetConfigData
{
    std::unordered_map<std::string, std::vector<std::string>> mappings;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AssetConfigData, mappings)
