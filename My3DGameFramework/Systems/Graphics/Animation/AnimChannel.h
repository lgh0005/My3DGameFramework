#pragma once
#include "Utils/AssetFormat.h"
#include "Animation/Pose.h"

namespace MGF3D
{
    MGF_CLASS_PTR(AnimChannel)
    class AnimChannel
    {
    public:
        AnimChannel
        (
            const SString& name,
            SVector<AssetFmt::RawKeyPosition>&& positions,
            SVector<AssetFmt::RawKeyRotation>&& rotations,
            SVector<AssetFmt::RawKeyScale>&& scales
        );
        ~AnimChannel();

        Pose GetPose(float time) const;

    /*==============================================//
    //   animation channel default getter methods   //
    //==============================================*/
    public:
        uint32 GetPositionIndex(float animationTime) const;
        uint32 GetRotationIndex(float animationTime) const;
        uint32 GetScaleIndex(float animationTime) const;
        uint32 GetNameHash() const;
        const SString& GetBoneName() const;
        mat4 GetLocalTransform(float animationTime) const;

    /*===========================//
    //   interpolation methods   //
    //===========================*/
    private:
        float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;
        vec3 InterpolatePosition(float animationTime) const;
        quat InterpolateRotation(float animationTime) const;
        vec3 InterpolateScaling(float animationTime) const;

    private:
        SString m_name;

        SVector<AssetFmt::RawKeyPosition> m_positions;
        SVector<AssetFmt::RawKeyRotation> m_rotations;
        SVector<AssetFmt::RawKeyScale>    m_scales;

        uint32 m_numPositions;
        uint32 m_numRotations;
        uint32 m_numScalings;
    };
}
