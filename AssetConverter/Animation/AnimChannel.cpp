#include "ConverterPch.h"
#include "AnimChannel.h"

namespace MGF3D
{
    AnimChannel::AnimChannel() = default;
    AnimChannel::~AnimChannel() = default;

    AnimChannel::AnimChannel
    (
        const std::string& name,
        std::vector<RawKeyPosition>&& positions,
        std::vector<RawKeyRotation>&& rotations,
        std::vector<RawKeyScale>&& scales
    )
        : m_name(name)
        , m_positions(std::move(positions))
        , m_rotations(std::move(rotations))
        , m_scales(std::move(scales)) {}

    Pose AnimChannel::GetPose(float time) const
    {
        vec3 translation = InterpolatePosition(time);
        quat rotation = InterpolateRotation(time);
        vec3 scale = InterpolateScaling(time);
        return Pose(translation, rotation, scale);
    }

    std::string AnimChannel::GetBoneName() const { return m_name; }

    mat4 AnimChannel::GetLocalTransform(float animationTime) const
    {
        return GetPose(animationTime).ToMat4();
    }

    /*==============================================//
    //   animation channel default getter methods   //
    //==============================================*/
    uint32 AnimChannel::GetPositionIndex(float animationTime) const
    {
        usize posSize = m_positions.size();
        if (posSize < 2) return 0;

        for (usize index = 0; index < posSize - 1; ++index)
        {
            if (animationTime < m_positions[index + 1].time)
                return (uint32)index;
        }

        return (uint32)m_positions.size() - 2;
    }

    uint32 AnimChannel::GetRotationIndex(float animationTime) const
    {
        usize rotSize = m_rotations.size();
        if (rotSize < 2) return 0;

        for (usize index = 0; index < rotSize - 1; ++index)
        {
            if (animationTime < m_rotations[index + 1].time)
                return (uint32)index;
        }

        return (uint32)m_rotations.size() - 2;
    }

    uint32 AnimChannel::GetScaleIndex(float animationTime) const
    {
        usize sclSize = m_scales.size();
        if (sclSize < 2) return 0;

        for (usize index = 0; index < sclSize - 1; ++index)
        {
            if (animationTime < m_scales[index + 1].time)
                return index;
        }

        return sclSize - 2;
    }

    /*===========================//
    //   interpolation methods   //
    //===========================*/
    float AnimChannel::GetScaleFactor
    (
        float lastTimeStamp,
        float nextTimeStamp,
        float animationTime) const
    {
        float framesDiff = nextTimeStamp - lastTimeStamp;
        if (framesDiff <= 0.0001f) return 0.0f; // TODO : HasLength

        float midWayLength = animationTime - lastTimeStamp;
        float factor = midWayLength / framesDiff;
        if (factor < 0.0f) factor = 0.0f;
        if (factor > 1.0f) factor = 1.0f;

        return factor;
    }

    vec3 AnimChannel::InterpolatePosition(float animationTime) const
    {
        if (m_positions.empty()) return vec3(0.0f);
        if (m_positions.size() == 1) return m_positions[0].position;

        int p0Index = GetPositionIndex(animationTime);
        int p1Index = p0Index + 1;
        if (p1Index >= m_positions.size()) return m_positions[p0Index].position;

        float scaleFactor = GetScaleFactor
        (
            m_positions[p0Index].time,
            m_positions[p1Index].time,
            animationTime
        );

        return glm::mix
        (
            m_positions[p0Index].position,
            m_positions[p1Index].position,
            scaleFactor
        );
    }

    quat AnimChannel::InterpolateRotation(float animationTime) const
    {
        if (m_rotations.empty()) return quat(1.0f, 0.0f, 0.0f, 0.0f);
        if (m_rotations.size() == 1) return glm::normalize(m_rotations[0].rotation);

        int p0Index = GetRotationIndex(animationTime);
        int p1Index = p0Index + 1;
        if (p1Index >= m_rotations.size()) return m_rotations[p0Index].rotation;

        float scaleFactor = GetScaleFactor
        (
            m_rotations[p0Index].time,
            m_rotations[p1Index].time,
            animationTime
        );

        quat finalRotation = glm::slerp
        (
            m_rotations[p0Index].rotation,
            m_rotations[p1Index].rotation,
            scaleFactor
        );

        return normalize(finalRotation);
    }

    vec3 AnimChannel::InterpolateScaling(float animationTime) const
    {
        if (m_scales.empty()) return vec3(1.0f);
        if (m_scales.size() == 1) return m_scales[0].scale;

        int p0Index = GetScaleIndex(animationTime);
        int p1Index = p0Index + 1;
        if (p1Index >= m_scales.size()) return m_scales[p0Index].scale;

        float scaleFactor = GetScaleFactor
        (
            m_scales[p0Index].time,
            m_scales[p1Index].time,
            animationTime
        );

        return glm::mix
        (
            m_scales[p0Index].scale,
            m_scales[p1Index].scale,
            scaleFactor
        );
    }
}

