#include "EnginePch.h"
#include "MeshOutline.h"
#include "Object/GameObject.h"
#include "Components/MeshRenderer.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"

DECLARE_DEFAULTS_IMPL(MeshOutline)

MeshOutlineUPtr MeshOutline::Create(const glm::vec3& color, float thickness)
{
	auto outline = MeshOutlineUPtr(new MeshOutline());
	outline->Init(color, thickness);
	return std::move(outline);
}

void MeshOutline::Init(const glm::vec3& color, float thickness)
{
	m_color = color;
	m_thickness = thickness;
}

void MeshOutline::OnStart()
{
	m_targetRenderer = GetOwner()->GetComponent<MeshRenderer>();
}
