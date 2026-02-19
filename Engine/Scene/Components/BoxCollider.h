#pragma once
#include "Collider.h"

CLASS_PTR(BoxCollider)
class BoxCollider : public Collider
{
	DEFINE_COMPONENT_TYPE(ComponentType::BoxCollider)

public:
	virtual ~BoxCollider();
	static BoxColliderUPtr Create(const glm::vec3& size = glm::vec3(1.0f));

	void SetSize(const glm::vec3& size) { m_size = size; }
	const glm::vec3& GetSize() const { return m_size; }

private:
	BoxCollider();
	bool Init(const glm::vec3& size);
	virtual JPH::ShapeRefC CreateRawShape() override;

	glm::vec3 m_size	{ 1.0f };
};