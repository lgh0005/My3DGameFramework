#pragma once
#include "AssetTypes/AssetFormat.h"
#include "Utils/Pose.h"

class AnimChannel
{
public:
    ~AnimChannel();
    AnimChannel();
    AnimChannel
    (
        const std::string& name,
        std::vector<AssetFmt::RawKeyPosition>&& positions,
        std::vector<AssetFmt::RawKeyRotation>&& rotations,
        std::vector<AssetFmt::RawKeyScale>&&    scales
    );

    Pose GetPose(float time) const;
    std::string GetBoneName() const;
    glm::mat4 GetLocalTransform(float animationTime) const;

/*==============================================//
//   animation channel default getter methods   //
//==============================================*/
public:
    uint32 GetPositionIndex(float animationTime) const;
    uint32 GetRotationIndex(float animationTime) const;
    uint32 GetScaleIndex(float animationTime) const;

/*===========================//
//   interpolation methods   //
//===========================*/
private:
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;
    glm::vec3 InterpolatePosition(float animationTime) const;
    glm::quat InterpolateRotation(float animationTime) const;
    glm::vec3 InterpolateScaling(float animationTime) const;

private:
    std::string m_name;
    std::vector<AssetFmt::RawKeyPosition> m_positions;
    std::vector<AssetFmt::RawKeyRotation> m_rotations;
    std::vector<AssetFmt::RawKeyScale>    m_scales;
};