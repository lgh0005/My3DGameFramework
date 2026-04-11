#include "CorePch.h"
#include "Frustum.h"

namespace MGF3D
{
    Frustum::Frustum()
    {
        for (int i = 0; i < 6; i++) 
            m_planes[i] = CullingPlane();
    }
    Frustum::~Frustum() = default;

    void Frustum::Update(const glm::mat4& vp)
    {
        // 1. Math 유틸리티를 사용하여 행렬의 행(Row) 추출
        vec4 r0 = Math::GetRow(vp, 0);
        vec4 r1 = Math::GetRow(vp, 1);
        vec4 r2 = Math::GetRow(vp, 2);
        vec4 r3 = Math::GetRow(vp, 3);

        // 2. Gribb-Hartmann 방식으로 평면 추출
        // 생성자 호출 시 내부에서 자동으로 Normalize()가 수행됩니다.
        m_planes[Left] = CullingPlane(r3 + r0);
        m_planes[Right] = CullingPlane(r3 - r0);
        m_planes[Bottom] = CullingPlane(r3 + r1);
        m_planes[Top] = CullingPlane(r3 - r1);
        m_planes[Near] = CullingPlane(r3 + r2);
        m_planes[Far] = CullingPlane(r3 - r2);
    }

    bool Frustum::CheckBounds(const RenderBounds& bounds) const
    {
        const vec3& center = bounds.GetCenter();
        const vec3& extents = bounds.GetExtents();

        // 6개의 평면 모두에 대해 검사
        for (int i = 0; i < 6; i++)
        {
            const CullingPlane& p = m_planes[i];
            const vec3& normal = p.GetNormal();

            // 1. 투영 반지름(Projected Radius) 계산
            // 평면 법선 벡터의 각 성분 절대값 * AABB의 반너비
            // (AABB가 평면 법선 방향으로 얼마나 길게 뻗어있는지 계산)
            float r = (extents.x * Math::Abs(normal.x)) +
                      (extents.y * Math::Abs(normal.y)) +
                      (extents.z * Math::Abs(normal.z));

            // 2. 평면과 AABB 중심점 사이의 거리(Signed Distance) 계산
            // d > 0 : 평면 앞쪽, d < 0 : 평면 뒤쪽
            float d = p.GetDistanceToPoint(center);

            // 3. 판정
            // 중심까지의 거리(d)가 -r 보다 작다는 것은
            // AABB가 평면의 뒤쪽으로 반지름(r)보다 더 멀리 떨어져 있다는 뜻
            // 즉, 완전히 평면 바깥(Outside)임.
            if (d < -r - m_margin) return false;
        }

        // 모든 평면을 통과했으면(안쪽이거나 걸쳐있음) 그리기
        return true;
    }
}