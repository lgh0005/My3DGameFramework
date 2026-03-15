#include "CorePch.h"
#include "Frustum.h"
#include "AABB.h"

namespace MGF3D
{
	Frustum::Frustum() = default;
	Frustum::~Frustum() = default;

	void Frustum::Update(const mat4& vp)
	{
		/*===========================//
		//   Gribb-Hartmann Method   //
		//===========================*/
		// 1. 행렬의 각 행(Row) 추출 (Math 래핑이 필요하다면 추가 가능)
		vec4 row0 = Math::GetRow(vp, 0);
		vec4 row1 = Math::GetRow(vp, 1);
		vec4 row2 = Math::GetRow(vp, 2);
		vec4 row3 = Math::GetRow(vp, 3);

		// 2. 평면 추출 및 생성
		m_planes[Left]	 = CullingPlane(row3 + row0);
		m_planes[Right]  = CullingPlane(row3 - row0);
		m_planes[Bottom] = CullingPlane(row3 + row1);
		m_planes[Top]	 = CullingPlane(row3 - row1);
		m_planes[Near]	 = CullingPlane(row3 + row2);
		m_planes[Far]	 = CullingPlane(row3 - row2);

		// 3. 모든 평면 정규화
		for (int32 i = 0; i < (int32)Side::MAX; ++i)
			m_planes[i].Normalize();
	}

	bool Frustum::IsIn(const AABB& bounds) const
	{
		if (!bounds.IsValid()) return false;

		const vec3 center = bounds.Center();
		const vec3 extents = bounds.Extents();

		for (int32 i = 0; i < (int32)Side::MAX; ++i)
		{
			const CullingPlane& p = m_planes[i];
			const vec3& normal = p.Normal();

			// 1. 투영 반지름(Projected Radius) 계산
			// AABB의 반너비가 평면 법선 방향으로 투영된 최대 길이를 구함
			float r = (extents.x * Math::Abs(normal.x)) +
					  (extents.y * Math::Abs(normal.y)) +
					  (extents.z * Math::Abs(normal.z));

			// 2. 평면에서 중심점까지의 거리
			float d = p.GetDistanceToPoint(center);

			// 3. 판정 (Margin 적용)
			// 평면 뒤쪽으로 반지름보다 더 멀리 있다면 완전히 밖에 있는 것임
			if (d < -r - m_margin)
				return false;
		}

		return true;
	}
}