#include "FrameworkPch.h"
#include "Material.h"
#include "Managers/TypeManager.h"
#include "Programs/GraphicsProgram.h"
#include "Textures/GLTextureHandle.h"
#include "Buffers/GLUniformBuffer.h"

namespace MGF3D
{
	Material::Material(StringView name) : Super(name) 
	{
		m_textures.fill(nullptr);
	}
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

	MaterialPtr Material::Create(StringView matName)
	{
		auto material = MaterialPtr(new Material(matName));
		material->SetState(EResourceState::Loaded);
		return material;
	}

	bool Material::OnSyncCreate()
	{
		// 2. 머티리얼이 가질 모든 텍스쳐 로드 완료 검사
		for (const auto& texture : m_textures)
		{
			if (texture && texture->GetState() != EResourceState::Ready)
				return false;
		}

		// 3. 머티리얼 UBO 생성
		if (!m_materialBuffer)
		{
			MaterialData data; // 초기값으로 생성
			m_materialBuffer = GLUniformBuffer::Create(&data, sizeof(MaterialData));
		}

		// 3. 머티리얼 로드 완료
		m_state = EResourceState::Ready;
		return true;
	}

	void Material::SetTexture(ETextureSlot slot, const GLTextureHandlePtr& texture)
	{
		m_textures[static_cast<usize>(slot)] = texture;
	}

	GLTextureHandlePtr Material::GetTexture(ETextureSlot slot) const
	{
		return m_textures[static_cast<usize>(slot)];
	}

	void Material::Bind() const
	{
		if (m_state != EResourceState::Ready || !m_materialBuffer) return;

		// 2. 래퍼 클래스를 이용한 데이터 업데이트 및 바인딩
		MaterialData data;
		data.albedoFactor = albedoFactor;
		data.emissiveFactor = emissiveFactor;
		data.emissionStrength = emissionStrength;
		data.shininess = shininess;
		data.heightScale = heightScale;
		data.metallicFactor = metallicFactor;
		data.roughnessFactor = roughnessFactor;

		// 내부에서 glNamedBufferSubData 호출
		m_materialBuffer->UpdateData(&data, 0, sizeof(MaterialData));

		// 내부에서 glBindBufferBase 호출 (Binding Index: 2)
		m_materialBuffer->Bind(2);

		// 3. 텍스처 바인딩
		for (uint32 i = 0; i < static_cast<uint32>(ETextureSlot::Max); ++i)
		{
			const auto& texture = m_textures[i];
			if (texture && texture->GetState() == EResourceState::Ready) texture->Bind(i);
			else texture->Unbind(i);
		}
	}
}
