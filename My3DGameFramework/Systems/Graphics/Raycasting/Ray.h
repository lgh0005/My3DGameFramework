#pragma once

namespace MGF3D
{
    MGF_CLASS_PTR(Ray)
    class Ray
    {
    public:
        Ray();
        Ray(const vec3& origin, const vec3& direction);

    public:
        // t만큼 갔을 때의 위치 구하기 (P = O + tD)
        vec3 GetPoint(float distance) const;
        const vec3& GetOrigin() const { return m_origin; }
        const vec3& GetDirection() const { return m_direction; }

    private:
        vec3 m_origin;
        vec3 m_direction;
    };
}