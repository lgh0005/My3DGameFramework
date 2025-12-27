#include "EnginePch.h"
#include "MeshOutline.h"

MeshOutline::MeshOutline() = default;
MeshOutline::~MeshOutline() = default;

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