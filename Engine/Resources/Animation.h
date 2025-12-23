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
	static AnimationUPtr Load(const std::string& filePath, Model* model = nullptr);

    AnimChannel* FindChannel(const std::string& name);
    float GetTicksPerSecond() const { return m_ticksPerSecond; }
    float GetDuration() const { return m_duration; }

/*====================================================================//
//   keyframe load process methods : assimp (raw 3d keyframe files)   //
//====================================================================*/
private:
    bool LoadByAssimp(const std::string& filePath, Model* model);
    void ReadMissingBones(const aiAnimation* animation, Model& model);

/*=======================================================//
//   .myanim file load process methods : .myanim file    //
//=======================================================*/
private:
    bool LoadByBinary(const std::string& filePath);

private:
    Animation() = default;

    std::string m_name;
    float m_duration;
    float m_ticksPerSecond;
    std::vector<AnimChannelUPtr> m_channels;
};