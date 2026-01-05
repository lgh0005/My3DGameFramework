#include "EnginePch.h"
#include "JoltGizmo.h"
#include "Resources/StaticMesh.h"

DECLARE_DEFAULTS_IMPL(JoltGizmo)

JoltGizmoUPtr JoltGizmo::Create()
{
	auto gizmo = JoltGizmoUPtr(new JoltGizmo());
	if (!gizmo->Init()) return nullptr;
	return std::move(gizmo);
}

bool JoltGizmo::Init()
{
	m_cubeMesh = RESOURCE.GetResource<StaticMesh>("Cube");
	m_sphereMesh = RESOURCE.GetResource<StaticMesh>("Sphere");
	return (m_cubeMesh && m_sphereMesh);
}

void JoltGizmo::Clear()
{
	m_meshCommands.clear();
	m_lineCommands.clear();
}

void JoltGizmo::DrawCube
(
	const glm::vec3& center, 
	const glm::vec3& size, 
	const glm::vec4& color
)
{
	if (!m_cubeMesh) return;
	
	glm::mat4 model = glm::translate(glm::mat4(1.0f), center);
	model = glm::scale(model, size);

	m_meshCommands.push_back({ m_cubeMesh, model, color });
}

void JoltGizmo::DrawSphere
(
	const glm::vec3& center, 
	float radius, const 
	glm::vec4& color
)
{
	if (!m_sphereMesh) return;

	glm::mat4 model = glm::translate(glm::mat4(1.0f), center);
	model = glm::scale(model, glm::vec3(radius));

	m_meshCommands.push_back({ m_sphereMesh, model, color });
}

void JoltGizmo::DrawLine
(
	const glm::vec3& start, 
	const glm::vec3& end, 
	const glm::vec4& color
)
{
	m_lineCommands.push_back({ start, end, color });
}