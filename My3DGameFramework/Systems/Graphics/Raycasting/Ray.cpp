#include "CorePch.h"
#include "Ray.h"

namespace MGF3D
{
    Ray::Ray() : m_origin(0.f), m_direction(0.f, 0.f, -1.f) {}
    Ray::Ray(const vec3& origin, const vec3& direction) : m_origin(origin)
    {
        // 1. 방향 벡터가 유효한 길이(0이 아님)를 가지는지 확인
        if (Math::HasLength(direction)) m_direction = Math::Normalize(direction);
        else  m_direction = vec3(0.f, 0.f, -1.f);
    }

    vec3 Ray::GetPoint(float distance) const
    {
        return m_origin + (m_direction * distance);
    }
}
