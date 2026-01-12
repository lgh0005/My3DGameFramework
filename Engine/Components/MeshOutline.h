#pragma once
#include "Object/Component.h"

CLASS_PTR(MeshOutline)
class MeshOutline : public Component
{
public:
	virtual ~MeshOutline();
	static MeshOutlineUPtr Create
	(
		const glm::vec3& color = { 1.0f, 1.0f, 0.5f }, // 기본 노란색
		float thickness = 0.03f						   // 기본 두께 : 0.03
	);
	static const ComponentType s_ComponentType = ComponentType::MeshOutline;
	virtual ComponentType GetComponentType() const override { return s_ComponentType;}

	void SetColor(const glm::vec3& color) { m_color = color; }
	const glm::vec3& GetColor() const { return m_color; }

	void SetThickness(float thickness) { m_thickness = thickness; }
	float GetThickness() const { return m_thickness; }

private:
	MeshOutline();
	void Init(const glm::vec3& color, float thickness);

	glm::vec3 m_color	{ 1.0f, 1.0f, 0.0f };
	float m_thickness	{ 0.03f };
};