#pragma once
#include "Collider.h"

CLASS_PTR(BoxCollider)
class BoxCollider : public Collider
{
public:
	virtual ~BoxCollider();
	static BoxColliderUPtr Create
	(
		const glm::vec3& size = glm::vec3(1.0f),
		const glm::vec3& center = glm::vec3(0.0f)
	);
	static const ComponentType s_ComponentType = ComponentType::BoxCollider;
	virtual ComponentType GetComponentType() const override { return ComponentType::BoxCollider; }
	virtual JPH::ShapeRefC CreateShape() override;

	void SetSize(const glm::vec3& size) { m_size = size; }
	void SetCenter(const glm::vec3& center);

private:
	BoxCollider();
	bool Init(const glm::vec3& size, const glm::vec3& center);

	glm::vec3 m_size	{ 1.0f };
	glm::vec3 m_center	{ 0.0f };
};