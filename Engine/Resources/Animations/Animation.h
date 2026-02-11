#pragma once
#include "Resources/Resource.h"
#include "Resources/Animations/AnimChannel.h"
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
    DEFINE_RESOURCE_TYPE(ResourceType::Animation)

public:
    struct BakedData
    {
        std::vector<glm::mat4> localMatrices;
        uint32 frameCount = 0;
        uint32 boneCount = 0;
        float frameRate = 30.0f;
    };

    void Bake(Skeleton* skeleton);
    const BakedData& GetBakedData() const { return m_bakedData; }
    void SetGlobalOffset(uint32 offset) { m_globalOffset = offset; }
    uint32 GetGlobalOffset() const { return m_globalOffset; }

public:
    virtual ~Animation();

    // TODO : .myanim 전용 Load 메서드와 애니메이션 파일 전용 Load 메서드를 따로 만들 필요는 있어보인다.
	static AnimationUPtr Load(const std::string& filePath);

    // [LEGACY: CPU Skinning] 
    // GPU 베이킹 방식이 정착되면 런타임에서는 사용되지 않을 수 있음
    AnimChannel* FindChannel(const std::string& name);
    AnimChannel* FindChannel(uint32 nameHash);

    float GetTicksPerSecond() const { return m_ticksPerSecond; }
    float GetDuration() const { return m_duration; }
    const std::string& GetName() const { return m_name; }

/*===================================//
//   keyframe load process methods   //
//===================================*/
private:
    bool LoadByBinary(const std::string& filePath);

private:
    Animation();

    BakedData m_bakedData;
    uint32 m_globalOffset{ 0 };

    std::string m_name;
    float m_duration            { 0.0f };
    float m_ticksPerSecond      { 0.0f };

    std::vector<AnimChannelUPtr> m_channels;
    std::unordered_map<uint32, AnimChannel*> m_channelMap;
};