#pragma once
#include "Resources/Resource.h"
#include "Resources/Animations/AnimChannel.h"
#include "Misc/AssetFormat.h"
#include "Misc/Utils.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Model)
CLASS_PTR(Bone)
#pragma endregion

CLASS_PTR(Animation)
class Animation : public Resource
{
    DEFINE_RESOURCE_TYPE(ResourceType::Animation)

public:
    virtual ~Animation();

    // TODO : .myanim 전용 Load 메서드와 애니메이션 파일 전용 Load 메서드를 따로 만들 필요는 있어보인다.
	static AnimationUPtr Load(const std::string& filePath, Model* model = nullptr);

    AnimChannel* FindChannel(const std::string& name);
    AnimChannel* FindChannel(uint32 nameHash);

    float GetTicksPerSecond() const { return m_ticksPerSecond; }
    float GetDuration() const { return m_duration; }
    const std::string& GetName() const { return m_name; }

/*===================================//
//   keyframe load process methods   //
//===================================*/
private:
    bool LoadByAssimp(const std::string& filePath, Model* model);
    bool LoadByBinary(const std::string& filePath);
    void ParseAssimpChannels(const aiAnimation* animation);

private:
    Animation();

    std::string m_name;
    float m_duration            { 0.0f };
    float m_ticksPerSecond      { 0.0f };
    std::vector<AnimChannelUPtr> m_channels;
    std::unordered_map<uint32, AnimChannel*> m_channelMap;
};