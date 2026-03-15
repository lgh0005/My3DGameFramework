#include "CorePch.h"
#include "BoundingSphere.h"
#include "Ray.h"
#include "RaycastHit.h"

namespace MGF3D
{
    BoundingSphere::BoundingSphere() = default;
    BoundingSphere::BoundingSphere(const vec3& inCenter, float inRadius)
        : center(inCenter), radius(inRadius) { }
    BoundingSphere::~BoundingSphere() = default;

    bool BoundingSphere::Contains(const vec3& point) const
    {
        float distSq = Math::DistanceSquared(center, point);
        return distSq <= Math::Square(radius);
    }

    bool BoundingSphere::Intersects(const Ray& ray, float& outDist) const
    {
        // 기하학적 해법 (Ray-Sphere Intersection)
        vec3 L = center - ray.origin;
        float tca = Math::Dot(L, ray.direction);

        // 구체가 레이 뒤에 있고 시작점이 구체 바깥에 있는 경우 충돌 불가
        if (tca < 0.0f && Math::LengthSquared(L) > Math::Square(radius))
            return false;

        float d2 = Math::Dot(L, L) - Math::Square(tca);
        float r2 = Math::Square(radius);
        if (d2 > r2) return false;

        float thc = Math::Sqrt(r2 - d2);
        float t0 = tca - thc;
        float t1 = tca + thc;

        // t0가 음수면 레이가 구체 내부에서 시작된 것이므로 t1을 선택
        outDist = CommonUtils::Select(t0 < 0.0f, t1, t0);
        return outDist >= 0.0f;
    }

    bool BoundingSphere::Raycast(const Ray& ray, RaycastHit& outHit) const
    {
        float dist = 0.0f;
        if (!Intersects(ray, dist)) return false;

        vec3 hitPoint = ray.GetPoint(dist);
        vec3 normal = Math::Normalize(hitPoint - center);

        outHit.Set(nullptr, hitPoint, normal, dist); // 충돌 대상을 설정을 해야 할 것 같은데?
        return true;
    }

    bool BoundingSphere::Raycast
    (
        const Ray& ray, 
        const mat4& transform, 
        const mat4& invTransform, 
        RaycastHit& outHit
    ) const
    {
        // 1. 전달받은 역행렬로 월드 레이를 로컬 공간(구체 공간)으로 변환
        vec3 localOrigin = vec3(invTransform * vec4(ray.origin, 1.0f));
        vec3 localDirection = vec3(invTransform * vec4(ray.direction, 0.0f));

        // 스케일 보정
        float lengthScale = Math::Length(localDirection);
        if (Math::IsNearlyZero(lengthScale)) return false;

        localDirection /= lengthScale;
        Ray localRay(localOrigin, localDirection);

        // 2. 로컬 공간(완벽한 구체)에서 레이캐스팅 수행
        RaycastHit localHit;
        if (!this->Raycast(localRay, localHit))
            return false;

        // 3. 결과를 월드 공간으로 복원
        vec3 worldHitPt = vec3(transform * vec4(localHit.point, 1.0f));

        // 거리는 로컬 거리를 월드 스케일로 보정
        float worldDist = localHit.distance / lengthScale;

        // 법선 복원 (Normal Matrix 적용)
        mat3 normalMatrix = mat3(Math::Transpose(invTransform));
        vec3 worldNormal = Math::Normalize(normalMatrix * localHit.normal);

        outHit.Set(nullptr, worldHitPt, worldNormal, worldDist);
        return true;
    }

    AABB BoundingSphere::GetAABB(const mat4& transform) const
    {
        // 1. 변환된 중심점
        vec3 worldCenter = vec3(transform * vec4(center, 1.0f));

        // 2. 각 월드 축 방향의 정밀 반너비 계산
        vec3 extent;
        extent.x = Math::Sqrt
        (
            Math::Square(transform[0].x * radius) +
            Math::Square(transform[1].x * radius) +
            Math::Square(transform[2].x * radius)
        );

        // 월드 Y축 범위 (행렬의 1번 행 성분들 이용)
        extent.y = Math::Sqrt
        (
            Math::Square(transform[0].y * radius) +
            Math::Square(transform[1].y * radius) +
            Math::Square(transform[2].y * radius)
        );

        // 월드 Z축 범위 (행렬의 2번 행 성분들 이용)
        extent.z = Math::Sqrt
        (
            Math::Square(transform[0].z * radius) +
            Math::Square(transform[1].z * radius) +
            Math::Square(transform[2].z * radius)
        );

        return AABB(worldCenter - extent, worldCenter + extent);
    }

    BoundingSphere BoundingSphere::Transformed(const mat4& mat) const
    {
        vec3 newCenter = vec3(mat * vec4(center, 1.0f));

        vec3 scale;
        scale.x = Math::Length(vec3(mat[0]));
        scale.y = Math::Length(vec3(mat[1]));
        scale.z = Math::Length(vec3(mat[2]));

        float maxScale = Math::Max(scale.x, Math::Max(scale.y, scale.z));
        return BoundingSphere(newCenter, radius * maxScale);
    }
}