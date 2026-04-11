#include "RenderBounds.h"
#include "GraphicsPch.h"

namespace MGF3D
{
    RenderBounds::RenderBounds() : m_center(0.0f), m_extents(0.0f) { }
    RenderBounds::RenderBounds(const vec3& center, const vec3& extents) : m_center(center), m_extents(extents) { }
    RenderBounds::~RenderBounds() = default;

    RenderBounds RenderBounds::Transform(const mat4& mat) const
    {
        vec3 newCenter = vec3(mat * vec4(m_center, 1.0f));
        vec3 right = vec3(mat[0]);
        vec3 up = vec3(mat[1]);
        vec3 fwd = vec3(mat[2]);

        vec3 newExtents;
        newExtents.x = Math::Abs(right.x) * m_extents.x + Math::Abs(up.x) * m_extents.y + Math::Abs(fwd.x) * m_extents.z;
        newExtents.y = Math::Abs(right.y) * m_extents.x + Math::Abs(up.y) * m_extents.y + Math::Abs(fwd.y) * m_extents.z;
        newExtents.z = Math::Abs(right.z) * m_extents.x + Math::Abs(up.z) * m_extents.y + Math::Abs(fwd.z) * m_extents.z;

        return RenderBounds(newCenter, newExtents);
    }

    RenderBounds RenderBounds::Union(const RenderBounds& other) const
    {
        // 1. 각각의 Min / Max 추출
        vec3 minA = GetMin();
        vec3 maxA = GetMax();
        vec3 minB = other.GetMin();
        vec3 maxB = other.GetMax();

        // 2. 전체를 아우르는 새로운 Min / Max 계산
        vec3 newMin = Math::Min(minA, minB);
        vec3 newMax = Math::Max(maxA, maxB);

        // 3. 생성자 호출 후 결과 반환
        RenderBounds result;
        result.SetFromMinMax(newMin, newMax);
        return result;
    }

    void RenderBounds::SetFromMinMax(const vec3& min, const vec3& max)
    {
        m_center = (max + min) * 0.5f;
        m_extents = (max - min) * 0.5f;
    }
}
