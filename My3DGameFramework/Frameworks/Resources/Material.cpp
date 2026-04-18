#include "FrameworkPch.h"
#include "Material.h"
#include "Managers/TypeManager.h"
#include "Programs/GraphicsProgram.h"
#include "Textures/GLTextureHandle.h"

namespace MGF3D
{
	Material::Material() = default;
	Material::~Material() = default;

	/*========================//
	//     Material Type      //
	//========================*/
	int16 Material::s_typeIndex = -1;
	const MGFType* Material::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	MaterialPtr Material::Create(StringView matName, const GraphicsProgramPtr& program)
	{
		auto material = MakeShared<Material>();
		material->SetHash(matName);
		if (program) material->SetProgram(program);
		material->SetState(EResourceState::Loaded);
		return material;
	}

	bool Material::OnSyncCreate()
	{
		// 1. 프로그램 로드 완료 검사
		if (m_program && m_program->GetState() != EResourceState::Ready)
			return false;

		// 2. 머티리얼이 가질 모든 텍스쳐 로드 완료 검사
		for (const auto& [slot, texture] : m_textures)
		{
			if (texture && texture->GetState() != EResourceState::Ready)
				return false;
		}

		// 3. 머티리얼 로드 완료
		m_state = EResourceState::Ready;
		return true;
	}

	void Material::SetProgram(const GraphicsProgramPtr& program)
	{
		m_program = program;
	}

	void Material::SetTexture(ETextureSlot slot, const GLTextureHandlePtr& texture)
	{
		m_textures[slot] = texture;
	}

	GLTextureHandlePtr Material::GetTexture(ETextureSlot slot) const
	{
		auto it = m_textures.find(slot);
		if (it != m_textures.end()) return it->second;
		return nullptr;
	}

	void Material::Bind() const
	{
		// TODO
		// 1. 머티리얼 인자값들 (수치값) UBO 바인딩
 
		// 2. 텍스쳐 바인딩

	}
}
