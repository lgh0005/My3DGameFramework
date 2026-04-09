#include "EnginePch.h"
#include "RenderBounds.h"

DECLARE_DEFAULTS_IMPL(RenderBounds)

RenderBounds RenderBounds::Empty()
{
    return Create(glm::vec3(0.0f), glm::vec3(0.0f));
}

RenderBounds RenderBounds::Create(const glm::vec3& center,
    const glm::vec3& extents)
{
    RenderBounds bound;
    bound.m_center = center;
    bound.m_extents = extents;
    return bound;
}

RenderBounds RenderBounds::CreateFromMinMax(const glm::vec3& min, const glm::vec3& max)
{
    return RenderBounds::Create((max + min) * 0.5f, (max - min) * 0.5f);
}

RenderBounds RenderBounds::Transform(const glm::mat4& mat) const
{
    glm::vec3 newCenter = glm::vec3(mat * glm::vec4(m_center, 1.0f));
    glm::vec3 right = glm::vec3(mat[0]);
    glm::vec3 up = glm::vec3(mat[1]);
    glm::vec3 fwd = glm::vec3(mat[2]);

    glm::vec3 newExtents;
    newExtents.x = glm::abs(right.x) * m_extents.x + glm::abs(up.x) * m_extents.y + glm::abs(fwd.x) * m_extents.z;
    newExtents.y = glm::abs(right.y) * m_extents.x + glm::abs(up.y) * m_extents.y + glm::abs(fwd.y) * m_extents.z;
    newExtents.z = glm::abs(right.z) * m_extents.x + glm::abs(up.z) * m_extents.y + glm::abs(fwd.z) * m_extents.z;

    return RenderBounds::Create(newCenter, newExtents);
}

RenderBounds RenderBounds::Union(const RenderBounds& other) const
{
    // 1. 현재 bounds를 Min/Max 좌표로 변환
    glm::vec3 minA = m_center - m_extents;
    glm::vec3 maxA = m_center + m_extents;

    // 2. other bounds를 Min/Max 좌표로 변환
    glm::vec3 minB = other.m_center - other.m_extents;
    glm::vec3 maxB = other.m_center + other.m_extents;

    // 3. 두 Min/Max를 합쳐서 새로운 최종 Min/Max 계산 (Union)
    glm::vec3 newMin = Utils::GlmVec3Min(minA, minB);
    glm::vec3 newMax = Utils::GlmVec3Max(maxA, maxB);

    return RenderBounds::CreateFromMinMax(newMin, newMax);
}
