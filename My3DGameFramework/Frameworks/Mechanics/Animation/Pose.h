#pragma once

CLASS_PTR(Pose)
class Pose
{
public:
    glm::vec3 position  { 0.0f };
    glm::quat rotation  { 1.0f, 0.0f, 0.0f, 0.0f }; // Identity
    glm::vec3 scale     { 1.0f };

    // 기본 생성자 / 소멸자
    Pose();
    Pose(const glm::vec3& p, const glm::quat& r, const glm::vec3& s);
    ~Pose();

    // 행렬 변환 (최종 단계에서만 호출)
    glm::mat4 ToMat4() const;

    // 보간 함수
    static Pose Interpolate(const Pose& a, const Pose& b, float factor);

    // 행렬에서 Pose 추출
    static Pose FromMat4(const glm::mat4& mat);

    // 단위 포즈
    static Pose Identity();
};