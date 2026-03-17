#pragma once

namespace MGF3D
{
    MGF_CLASS_PTR(Pose)
    class Pose
    {
    public:
        vec3 position   { 0.0f };
        quat rotation   { 1.0f, 0.0f, 0.0f, 0.0f };
        vec3 scale      { 1.0f };

    public:
        Pose();
        Pose(const vec3& p, const quat& r, const vec3& s);
        ~Pose();

    public:
        // 행렬 변환 (최종 단계에서만 호출)
        mat4 ToMat4() const;

        // 보간 함수
        static Pose Interpolate(const Pose& a, const Pose& b, float factor);

        // 행렬에서 Pose 추출
        static Pose FromMat4(const mat4& mat);

        // 단위 포즈
        static Pose Identity();
    };
}