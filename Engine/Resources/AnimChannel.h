#pragma once
#include "Resources/Resource.h"
#include "Misc/AssetFormat.h"

CLASS_PTR(AnimChannel)
class AnimChannel : public Resource
{
public:
    static const ResourceType s_ResourceType = ResourceType::AnimChannel;
    virtual ResourceType GetResourceType() const override { return ResourceType::AnimChannel; }
	static AnimChannelUPtr Create(const std::string& name, int32 id, const aiNodeAnim* channel);
    static AnimChannelUPtr Create
    (
        uint32 id, 
        std::vector<AssetFmt::RawKeyPosition>&& positions,
        std::vector<AssetFmt::RawKeyRotation>&& rotations,
        std::vector<AssetFmt::RawKeyScale>&& scales
    );

public:
    // void Update(float animationTime);
    uint32 GetPositionIndex(float animationTime) const;
    uint32 GetRotationIndex(float animationTime) const;
    uint32 GetScaleIndex(float animationTime) const;

    uint32 GetBoneID() const;
    std::string GetBoneName() const;
    glm::mat4 GetLocalTransform(float animationTime) const;

private:
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;
    glm::mat4 InterpolatePosition(float animationTime) const;
    glm::mat4 InterpolateRotation(float animationTime) const;
    glm::mat4 InterpolateScaling(float animationTime) const;

private:
    AnimChannel() = default;
    bool Init(const std::string& name, int id, const aiNodeAnim* channel);
    void Init
    (
        uint32 id, 
        std::vector<AssetFmt::RawKeyPosition>&& positions,
        std::vector<AssetFmt::RawKeyRotation>&& rotations,
        std::vector<AssetFmt::RawKeyScale>&& scales
    );

    std::vector<AssetFmt::RawKeyPosition> m_positions;
    std::vector<AssetFmt::RawKeyRotation> m_rotations;
    std::vector<AssetFmt::RawKeyScale>    m_scales;

    uint32 m_numPositions;
    uint32 m_numRotations;
    uint32 m_numScalings;

    glm::mat4   m_localTransform { glm::mat4(1.0f)};
    std::string m_name;
    uint32      m_id;
};