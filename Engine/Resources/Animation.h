#pragma once
#include "Resources/Resource.h"
#include "Resources/AnimChannel.h"
#include "Misc/AssetFormat.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Model)
CLASS_PTR(Bone)
#pragma endregion

CLASS_PTR(Animation)
class Animation : public Resource
{
public:
    static const ResourceType s_ResourceType = ResourceType::Animation;
    virtual ResourceType GetResourceType() const override { return ResourceType::Animation; }
	static AnimationUPtr Load(const std::string& filePath, Model* model);

    AnimChannel* FindBone(const std::string& name);
    float GetTicksPerSecond() { return m_ticksPerSecond; }
    float GetDuration() { return m_duration; }
    const std::unordered_map<std::string, AssetFmt::RawBoneInfo>& GetBoneIDMap() { return m_boneInfoMap; }

private:
	Animation() = default;

    bool LoadByAssimp(const std::string& filePath, Model* model);
    bool LoadByBinary(const std::string& filePath);
    void ReadMissingBones(const aiAnimation* animation, Model& model);

    std::string m_name;
    float m_duration;
    float m_ticksPerSecond;
    std::vector<AnimChannelUPtr> m_bones;
    std::unordered_map<std::string, AssetFmt::RawBoneInfo> m_boneInfoMap;
};