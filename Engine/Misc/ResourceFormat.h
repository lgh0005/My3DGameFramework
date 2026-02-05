#pragma once
#include "Misc/ResourceJsonKeys.h"
using namespace ResourceJsonKeys;

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
        {AssetType::Texture,   KEY_TEXTURE},
        {AssetType::Model,     KEY_MODEL},
        {AssetType::Audio,     KEY_AUDIO},
        {AssetType::Animation, KEY_ANIMATION}
    }
);

struct AssetConfigData
{
    std::unordered_map<std::string, std::vector<std::string>> mappings;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AssetConfigData, mappings)
