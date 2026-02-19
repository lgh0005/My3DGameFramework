#pragma once
#include "Object/Component.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(MeshRenderer)
#pragma endregion

CLASS_PTR(MeshOutline)
class MeshOutline : public Component
{
	DEFINE_COMPONENT_TYPE(ComponentType::MeshOutline)

public:
	virtual ~MeshOutline();
	static MeshOutlineUPtr Create
	(
		const glm::vec3& color = { 1.0f, 1.0f, 0.5f }, // 기본 노란색
		float thickness = 0.03f						   // 기본 두께 : 0.03
	);
	virtual void OnStart() override;

	void SetColor(const glm::vec3& color) { m_color = color; }
	const glm::vec3& GetColor() const { return m_color; }

	void SetThickness(float thickness) { m_thickness = thickness; }
	float GetThickness() const { return m_thickness; }

	MeshRenderer* GetMeshRenderer() const { return m_targetRenderer; }

private:
	MeshOutline();
	void Init(const glm::vec3& color, float thickness);

	glm::vec3 m_color	{ 1.0f, 1.0f, 0.0f };
	float m_thickness	{ 0.03f };

	MeshRenderer* m_targetRenderer	{ nullptr };
};