#pragma once

namespace MGF3D
{
    template <typename TVertex>
    inline RenderBounds::RenderBounds(const Vector<TVertex>& vertices)
    {
        SetFromVertices(vertices);
    }

    template <typename TVertex>
    inline void RenderBounds::SetFromVertices(const Vector<TVertex>& vertices)
    {
        if (vertices.empty())
        {
            m_center = vec3(0.0f);
            m_extents = vec3(0.0f);
            return;
        }

        // 초기값 세팅 (Math 모듈 또는 정의된 상수에 맞춤)
        vec3 minBound(MAX_FLOAT);
        vec3 maxBound(MIN_FLOAT);

        for (const auto& v : vertices)
        {
            // 모든 정점 타입은 position 멤버를 가진다고 가정
            minBound = Math::Min(minBound, v.position);
            maxBound = Math::Max(maxBound, v.position);
        }

        SetFromMinMax(minBound, maxBound);
    }
}