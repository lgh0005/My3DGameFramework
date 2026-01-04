#include "EnginePch.h"
#include "Frustum.h"

DECLARE_DEFAULTS_IMPL(Frustum)

FrustumUPtr Frustum::Create()
{
    auto frustum = FrustumUPtr(new Frustum());
    frustum->Init();
    return std::move(frustum);
}

void Frustum::Init()
{
    for (int i = 0; i < 6; i++) m_planes[i] = CullingPlane();
}

void Frustum::Update(const glm::mat4& vp)
{
    // 1. 행렬의 각 행(Row) 추출
    glm::vec4 row0 = glm::row(vp, 0);
    glm::vec4 row1 = glm::row(vp, 1);
    glm::vec4 row2 = glm::row(vp, 2);
    glm::vec4 row3 = glm::row(vp, 3);

    // 2. 평면 추출 공식 적용
    m_planes[Side::Left] = CullingPlane::Create(row3 + row0);
    m_planes[Side::Right] = CullingPlane::Create(row3 - row0);
    m_planes[Side::Bottom] = CullingPlane::Create(row3 + row1);
    m_planes[Side::Top] = CullingPlane::Create(row3 - row1);

    m_planes[Side::Near] = CullingPlane::Create(row3 + row2);
    m_planes[Side::Far] = CullingPlane::Create(row3 - row2);

    for (int i = 0; i < 6; i++)
        m_planes[i].Normalize();
}

bool Frustum::CheckBounds(const RenderBounds& bounds) const
{
    // 6개의 평면 모두에 대해 검사
    for (int i = 0; i < 6; i++)
    {
        const CullingPlane& p = m_planes[i];

        const glm::vec3& center = bounds.GetCenter();
        const glm::vec3& extents = bounds.GetExtents();

        // 1. 투영 반지름(Projected Radius) 계산
        // 평면 법선 벡터의 각 성분 절대값 * AABB의 반너비
        // (AABB가 평면 법선 방향으로 얼마나 길게 뻗어있는지 계산)
        float r = (extents.x * glm::abs(p.NormalX())) +
                  (extents.y * glm::abs(p.NormalY())) +
                  (extents.z * glm::abs(p.NormalZ()));

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