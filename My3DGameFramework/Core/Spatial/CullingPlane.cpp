#include "CorePch.h"
#include "CullingPlane.h"
#include "Ray.h"

namespace MGF3D
{
    CullingPlane::CullingPlane() = default;
    CullingPlane::~CullingPlane() = default;
    CullingPlane::CullingPlane(const vec4& equation)
        : m_normal(vec3(equation.x, equation.y, equation.z)), m_distance(equation.w) { }
    CullingPlane::CullingPlane(const vec3& normal, float dist)
        : m_normal(normal), m_distance(dist) { }
    CullingPlane::CullingPlane(const vec3& normal, const vec3& pointOnPlane)
    {
        m_normal = Math::Normalize(normal);
        m_distance = -Math::Dot(m_normal, pointOnPlane);
    }

    void CullingPlane::Normalize()
    {
        float len = Math::Length(m_normal);
        if (Math::IsNearlyZero(len)) return;

        float invLen = 1.0f / len;
        m_normal *= invLen;
        m_distance *= invLen;
    }

    float CullingPlane::GetDistanceToPoint(const vec3& point) const
    {
        return Math::Dot(m_normal, point) + m_distance;
    }

    bool CullingPlane::Intersects(const Ray& ray, float& outDist) const
    {
        float denom = Math::Dot(m_normal, ray.direction);

        // 평면과 평행하지 않은 경우에만 교차 계산
        if (Math::Abs(denom) > Epsilon)
        {
            float t = -(Math::Dot(m_normal, ray.origin) + m_distance) / denom;
            if (t >= 0.0f)
            {
                outDist = t;
                return true;
            }
        }

        return false;
    }
}