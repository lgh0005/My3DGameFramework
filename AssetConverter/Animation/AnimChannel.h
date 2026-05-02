#pragma once

namespace MGF3D
{
    class AnimChannel
    {
    public:
        ~AnimChannel();
        AnimChannel();
        AnimChannel
        (
            const String& name,
            Vector<RawKeyPosition>&& positions,
            Vector<RawKeyRotation>&& rotations,
            Vector<RawKeyScale>&& scales
        );

        Pose GetPose(float time) const;
        String GetBoneName() const;
        mat4 GetLocalTransform(float animationTime) const;

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
        vec3 InterpolatePosition(float animationTime) const;
        quat InterpolateRotation(float animationTime) const;
        vec3 InterpolateScaling(float animationTime) const;

    private:
        String m_name;
        Vector<RawKeyPosition> m_positions;
        Vector<RawKeyRotation> m_rotations;
        Vector<RawKeyScale>    m_scales;
    };
}