#pragma once
#include "Misc/AssetFormat.h"
#include "Resources/Pose.h"

CLASS_PTR(AnimChannel)
class AnimChannel
{
public:
    virtual ~AnimChannel();
	static AnimChannelUPtr Create(const std::string& name, int32 id, const aiNodeAnim* channel);
    static AnimChannelUPtr Create
    (
        const std::string& name,
        uint32 id, 
        std::vector<AssetFmt::RawKeyPosition>&& positions,
        std::vector<AssetFmt::RawKeyRotation>&& rotations,
        std::vector<AssetFmt::RawKeyScale>&&    scales
    );

    Pose GetPose(float time) const;

/*==============================================//
//   animation channel default getter methods   //
//==============================================*/
public:
    uint32 GetPositionIndex(float animationTime) const;
    uint32 GetRotationIndex(float animationTime) const;
    uint32 GetScaleIndex(float animationTime) const;
    uint32 GetBoneID() const;
    std::string GetBoneName() const;
    glm::mat4 GetLocalTransform(float animationTime) const;

/*===========================//
//   interpolation methods   //
//===========================*/
private:
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;
    glm::vec3 InterpolatePosition(float animationTime) const;
    glm::quat InterpolateRotation(float animationTime) const;
    glm::vec3 InterpolateScaling(float animationTime) const;

private:
    AnimChannel();

    bool Init
    (
        const std::string& name, 
        int id, const aiNodeAnim* channel
    );

    void Init
    (
        const std::string& name,
        uint32 id, 
        std::vector<AssetFmt::RawKeyPosition>&& positions,
        std::vector<AssetFmt::RawKeyRotation>&& rotations,
        std::vector<AssetFmt::RawKeyScale>&& scales
    );

    uint32      m_id;
    std::string m_name;

    std::vector<AssetFmt::RawKeyPosition> m_positions;
    std::vector<AssetFmt::RawKeyRotation> m_rotations;
    std::vector<AssetFmt::RawKeyScale>    m_scales;

    uint32 m_numPositions;
    uint32 m_numRotations;
    uint32 m_numScalings;
};