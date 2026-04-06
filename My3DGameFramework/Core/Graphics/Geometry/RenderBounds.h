#pragma once

class RenderBounds
{
public:
	~RenderBounds();
	static RenderBounds Empty();
	static RenderBounds Create(const glm::vec3& center, const glm::vec3& extents);
	static RenderBounds CreateFromMinMax(const glm::vec3& min, const glm::vec3& max);

	RenderBounds Transform(const glm::mat4& mat) const;
	RenderBounds Union(const RenderBounds& other) const;
	const glm::vec3& GetCenter() const { return m_center; }
	const glm::vec3& GetExtents() const { return m_extents; }

private:
	RenderBounds();
	glm::vec3 m_center{ 0.0f };
	glm::vec3 m_extents{ 0.0f };
};