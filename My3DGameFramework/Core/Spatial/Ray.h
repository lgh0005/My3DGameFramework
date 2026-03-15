#pragma once

namespace MGF3D
{
    struct Ray
    {
        vec3 origin     { 0.0f };
        vec3 direction  { 0.0f, 0.0f, -1.0f };

        Ray();
        Ray(const vec3& _origin, const vec3& _direction);

        glm::vec3 GetPoint(float distance) const;
        void SetDirection(const vec3& dir);
    };
}