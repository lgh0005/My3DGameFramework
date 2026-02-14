#pragma once
#include "Resources/Resource.h"
#include "Resources/ResourceDesc.h"
#include "Resources/Animations/AnimChannel.h"
#include "Resources/Animations/AnimClip.h"
#include "Misc/AssetFormat.h"
#include "Misc/Utils.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Model)
CLASS_PTR(Bone)
CLASS_PTR(Skeleton)
#pragma endregion

CLASS_PTR(Animation)
class Animation : public Resource
{
    DEFINE_RESOURCE_TYPE(ResourceType::Animation, AnimationDesc)

public:
    virtual ~Animation();
    static AnimationPtr Load(const AnimationDesc& desc);
    virtual const AnimationDesc& GetDesc() const override { return m_desc; }
    
    const AnimClip& GetAnimClip() const { return m_animClip; }
    void SetGlobalOffset(uint32 offset) { m_globalOffset = offset; }
    uint32 GetGlobalOffset() const { return m_globalOffset; }
    float GetTicksPerSecond() const { return m_ticksPerSecond; }
    float GetDuration() const { return m_duration; }
    const std::string& GetName() const { return m_name; }

    // [LEGACY: CPU Skinning] 채널 데이터 접근
    AnimChannel* FindChannel(const std::string& name);
    AnimChannel* FindChannel(uint32 nameHash);

/*===================================//
//   keyframe load process methods   //
//===================================*/
private:
    bool LoadByBinary();
    Animation();
    AnimationDesc m_desc;

    AnimClip m_animClip;
    uint32 m_globalOffset{ 0 };

    std::string m_name;
    float m_duration            { 0.0f };
    float m_ticksPerSecond      { 0.0f };

    std::vector<AnimChannelUPtr> m_channels;
    std::unordered_map<uint32, AnimChannel*> m_channelMap;
};