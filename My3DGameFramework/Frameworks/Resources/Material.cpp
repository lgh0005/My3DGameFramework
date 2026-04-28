#include "FrameworkPch.h"
#include "Material.h"
#include "Managers/TypeManager.h"
#include "Assets/Image.h"
#include "Textures/GLTextureHandle.h"
#include "Buffers/GLUniformBuffer.h"

namespace MGF3D
{
	Material::Material(StringView name) : Super(name) 
	{
		m_images.fill(nullptr);
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
		// 1. 머티리얼 UBO 생성
		if (!m_materialBuffer)
		{
			MaterialData data; // 초기값으로 생성
			m_materialBuffer = GLUniformBuffer::Create(&data, sizeof(MaterialData));
		}

		// 2. 머티리얼 로드 완료
		m_state = EResourceState::Ready;
		return true;
	}

	void Material::SetTexture(ETextureSlot slot, const ImagePtr& image)
	{
		m_images[static_cast<usize>(slot)] = image;
	}

	ImagePtr Material::GetTexture(ETextureSlot slot) const
	{
		return m_images[static_cast<usize>(slot)];
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

		// 내부에서 glNamedBufferSubData 호출 및 바인딩
		m_materialBuffer->UpdateData(&data, 0, sizeof(MaterialData));
		m_materialBuffer->Bind(2);

		// 3. 텍스처 바인딩
		for (uint32 i = 0; i < static_cast<uint32>(ETextureSlot::Max); ++i)
		{
			bool success = false;
			const auto& image = m_images[i];

			// 에셋이 있고, 로드가 완료된 상태인지 확인
			if (image && image->GetState() >= EAssetState::Loaded)
			{
				const auto& resources = image->GetResources();
				if (!resources.empty())
				{
					auto texture = MGFTypeCaster::Cast<GLTextureHandle>(resources[0]);
					if (texture && texture->GetState() == EResourceState::Ready)
					{
						texture->Bind(i);
						success = true;
					}
				}
			}

			if (!success) GLTextureHandle::Unbind(i);
		}
	}
}
