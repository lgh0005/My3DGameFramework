#include "GraphicsPch.h"
#include "CullingPlane.h"

namespace MGF3D
{
    CullingPlane::CullingPlane() : m_normal(0.0f, 1.0f, 0.0f), m_distance(0.0f) { }
    CullingPlane::CullingPlane(const vec4& eq) : m_normal(vec3(eq.x, eq.y, eq.z)), m_distance(eq.w) { }
    CullingPlane::CullingPlane(const vec3& normal, float distance)
        : m_normal(normal), m_distance(distance)
    {
        Normalize();
    }
    CullingPlane::~CullingPlane() = default;

    void CullingPlane::Normalize()
    {
        // 1. 유효성 체크
        if (!Math::HasLength(m_normal)) return;

        // 2. 정규화 계수 계산
        float len2 = Math::LengthSquared(m_normal);
        float invLength = Math::InverseSqrt(len2);

        // 3. 평면 방정식(Ax + By + Cz + D = 0) 전체에 역수 곱하기
        m_normal *= invLength;
        m_distance *= invLength;
    }


    float CullingPlane::GetDistanceToPoint(const vec3& point) const
    {
        return Math::Dot(m_normal, point) + m_distance;
    }
}