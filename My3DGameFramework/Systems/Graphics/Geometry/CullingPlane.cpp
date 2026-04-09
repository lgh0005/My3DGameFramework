#include "EnginePch.h"
#include "CullingPlane.h"

DECLARE_DEFAULTS_IMPL(CullingPlane)
CullingPlane::CullingPlane(const glm::vec3& normal, float dist) : m_normal(normal), m_distance(dist) {}

CullingPlane CullingPlane::Create(const glm::vec4& eq)
{
    return CullingPlane(glm::vec3(eq.x, eq.y, eq.z), eq.w);
}

void CullingPlane::Normalize()
{
    // 1. 유효성 체크
    if (!Utils::HasLength(m_normal)) return;

    // 2. 정규화 계수 계산
    float len2 = glm::length2(m_normal);
    float invLength = glm::inversesqrt(len2);

    // 3. 평면 방정식(Ax + By + Cz + D = 0) 전체에 역수 곱하기
    m_normal *= invLength;
    m_distance *= invLength;
}

float CullingPlane::GetDistanceToPoint(const glm::vec3& point) const
{
    return glm::dot(m_normal, point) + m_distance;
}