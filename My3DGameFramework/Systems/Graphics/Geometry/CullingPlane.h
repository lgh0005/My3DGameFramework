#pragma once

class CullingPlane
{
public:
	CullingPlane();
	~CullingPlane();
	static CullingPlane Create(const glm::vec4& eq);

	void Normalize();
	float GetDistanceToPoint(const glm::vec3& point) const;

	const glm::vec3& Normal() const { return m_normal; }
	float NormalX() const { return m_normal.x; }
	float NormalY() const { return m_normal.y; }
	float NormalZ() const { return m_normal.z; }
	float Distance() const { return m_distance; }

private:
	CullingPlane(const glm::vec3& normal, float dist);
	glm::vec3 m_normal;
	float     m_distance;
};