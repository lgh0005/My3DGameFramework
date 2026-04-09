#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Camera)
#pragma endregion

CLASS_PTR(Ray)
class Ray
{
public:
    Ray();
    Ray(const glm::vec3& origin, const glm::vec3& direction);
    Ray(const Ray&) = default;
    Ray& operator=(const Ray&) = default;

public:
    // t만큼 갔을 때의 위치 구하기 (P = O + tD)
    glm::vec3 GetPoint(float distance) const;
    const glm::vec3& GetOrigin() const { return m_origin; }
    const glm::vec3& GetDirection() const { return m_direction; }

private:
    glm::vec3 m_origin;
    glm::vec3 m_direction;
};