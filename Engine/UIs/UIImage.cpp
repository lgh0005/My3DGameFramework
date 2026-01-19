#include "EnginePch.h"
#include "UIImage.h"
#include "Resources/Program.h"
#include "Resources/Meshes/UIMesh.h"
#include "Resources/Material.h"

DECLARE_DEFAULTS_IMPL(UIImage)

UIImageUPtr UIImage::Create(UIMeshPtr mesh, MaterialPtr material)
{
	UIImageUPtr ui = UIImageUPtr(new UIImage());
	if (!ui->Init(mesh, material)) return nullptr;
	return std::move(ui);
}

bool UIImage::Init(UIMeshPtr mesh, MaterialPtr material)
{
	m_uiMesh = mesh;
	m_material = material;
	if (!m_uiMesh || !m_material) return false;
	return true;
}

void UIImage::Render(Program* program)
{
	if (m_material) m_material->SetToProgram(program);
	m_uiMesh->Draw();
}

