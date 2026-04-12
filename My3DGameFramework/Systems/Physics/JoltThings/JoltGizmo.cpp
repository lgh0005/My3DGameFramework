#include "EnginePch.h"
#include "JoltGizmo.h"
#include "Resources/Meshes/StaticMesh.h"

DECLARE_DEFAULTS_IMPL(JoltGizmo)

JoltGizmoUPtr JoltGizmo::Create()
{
	return JoltGizmoUPtr(new JoltGizmo());
}

void JoltGizmo::Clear()
{
	m_lineCommands.clear();
	m_triangleCommands.clear();
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

void JoltGizmo::DrawTriangle
(
	const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, 
	const glm::vec4& color
)
{
	m_triangleCommands.push_back({ v1, v2, v3, color });
}