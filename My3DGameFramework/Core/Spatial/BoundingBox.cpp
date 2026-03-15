#include "CorePch.h"
#include "BoundingBox.h"
#include "Ray.h"
#include "RaycastHit.h"

namespace MGF3D
{
    BoundingBox::BoundingBox() = default;
    BoundingBox::BoundingBox(const vec3& min, const vec3& max) : m_data(min, max) { }
    BoundingBox::~BoundingBox() = default;

    BoundingBox BoundingBox::Empty()
    {
        return BoundingBox(vec3(FloatMax), vec3(FloatLowest));
    }

    BoundingBox BoundingBox::FromCenterExtents(const vec3& center, const vec3& extents)
    {
        return BoundingBox(center - extents, center + extents);
    }

    bool BoundingBox::Contains(const vec3& point) const
    {
        if (!m_data.IsValid()) return false;
        return CommonUtils::IsBetween(point.x, m_data.min.x, m_data.max.x) &&
               CommonUtils::IsBetween(point.y, m_data.min.y, m_data.max.y) &&
               CommonUtils::IsBetween(point.z, m_data.min.z, m_data.max.z);
    }

    bool BoundingBox::Intersects(const Ray& ray, float& outDist) const
    {
        if (!m_data.IsValid()) return false;

        float tmin = FloatLowest;
        float tmax = FloatMax;

        for (int i = 0; i < 3; ++i)
        {
            if (Math::IsNearlyZero(ray.direction[i]))
            {
                if (!CommonUtils::IsBetween(ray.origin[i], m_data.min[i], m_data.max[i]))
                    return false;
            }
            else
            {
                float invDir = 1.0f / ray.direction[i];
                float t1 = (m_data.min[i] - ray.origin[i]) * invDir;
                float t2 = (m_data.max[i] - ray.origin[i]) * invDir;

                if (t1 > t2) CommonUtils::Swap(t1, t2);

                tmin = Math::Max(tmin, t1);
                tmax = Math::Min(tmax, t2);

                if (tmin > tmax) return false;
            }
        }

        if (tmax < 0.0f) return false;
        outDist = CommonUtils::Select(tmin < 0.0f, 0.0f, tmin);
        return true;
    }

    bool BoundingBox::Raycast(const Ray& ray, RaycastHit& outHit) const
    {
        float dist = 0.0f;
        if (!Intersects(ray, dist)) return false;

        vec3 hitPoint = ray.GetPoint(dist);
        vec3 center = m_data.Center();
        vec3 extents = m_data.Extents();
        vec3 localPt = hitPoint - center;

        // 각 성분의 비율을 계산하여 가장 외곽에 있는 축 찾기
        vec3 d = Math::Abs(localPt / Math::Max(extents, vec3(Epsilon)));

        vec3 normal(0.0f);
        if      (d.x > d.y && d.x > d.z) normal.x = CommonUtils::Select(localPt.x > 0.0f, 1.0f, -1.0f);
        else if (d.y > d.x && d.y > d.z) normal.y = CommonUtils::Select(localPt.y > 0.0f, 1.0f, -1.0f);
        else                             normal.z = CommonUtils::Select(localPt.z > 0.0f, 1.0f, -1.0f);

        outHit.Set(nullptr, hitPoint, normal, dist);
        return true;
    }

    bool BoundingBox::Raycast
    (
        const Ray& ray, 
        const mat4& transform, 
        const mat4& invTransform, 
        RaycastHit& outHit
    ) const
    {
        // 1. 전달받은 역행렬을 사용하여 월드 레이를 로컬로 즉시 변환
        vec3 localOrigin = vec3(invTransform * vec4(ray.origin, 1.0f));
        vec3 localDirection = vec3(invTransform * vec4(ray.direction, 0.0f));

        // 스케일 보정 (로컬 방향 벡터 정규화 및 길이 저장)
        float lengthScale = Math::Length(localDirection);
        if (Math::IsNearlyZero(lengthScale)) return false;

        localDirection /= lengthScale;
        Ray localRay(localOrigin, localDirection);

        // 2. 로컬 공간(박스가 똑바로 서 있는 상태)에서 정밀 레이캐스팅 수행
        RaycastHit localHit;
        if (!this->Raycast(localRay, localHit))
            return false;

        // 3. 결과를 다시 월드 공간으로 복원
        vec3 worldHitPt = vec3(transform * vec4(localHit.point, 1.0f));

        // 거리 복원: 로컬 거리를 스케일로 나누어 월드 거리를 산출합니다.
        float worldDist = localHit.distance / lengthScale;

        // 법선 복원: 법선은 인버스 트랜스포즈 행렬을 사용하는 것이 정석입니다.
        mat3 normalMatrix = mat3(glm::transpose(invTransform));
        vec3 worldNormal = Math::Normalize(normalMatrix * localHit.normal);

        outHit.Set(nullptr, worldHitPt, worldNormal, worldDist);
        return true;
    }

    AABB BoundingBox::GetAABB(const mat4& transform) const
    {
        if (!m_data.IsValid()) return AABB();

        // 1. 로컬 중심점을 월드로 변환
        vec3 worldCenter = vec3(transform * vec4(m_data.Center(), 1.0f));

        // 2. 절대값 행렬을 사용하여 월드 Extents 계산
        // 행렬의 각 열 벡터(Right, Up, Forward)가 축에 끼치는 절대적 기여도를 합산합니다.
        mat3 absMat = mat3(
            Math::Abs(vec3(transform[0])),
            Math::Abs(vec3(transform[1])),
            Math::Abs(vec3(transform[2]))
        );
        vec3 worldExtents = absMat * m_data.Extents();

        return AABB(worldCenter - worldExtents, worldCenter + worldExtents);
    }

    BoundingBox BoundingBox::Transformed(const mat4& mat) const
    {
        AABB transformedAABB = GetAABB(mat);
        return BoundingBox(transformedAABB.min, transformedAABB.max);
    }

    BoundingBox BoundingBox::Combined(const BoundingBox& other) const
    {
        AABB combinedData = m_data;
        combinedData.Encapsulate(other.m_data);
        return BoundingBox(combinedData.min, combinedData.max);
    }
}