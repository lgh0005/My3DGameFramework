#include "FrameworkPch.h"
#include "Material.h"
#include "Managers/TypeManager.h"
#include "Assets/Image.h"
#include "Textures/GLTextureHandle.h"
#include "Textures/GLTexture2D.h"
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
		if (GetState() != EResourceState::Ready || !m_materialBuffer) return;

		// 1. 래퍼 클래스를 이용한 데이터 업데이트 및 바인딩
		MaterialData data;
		data.albedoFactor = albedoFactor;
		data.emissiveFactor = emissiveFactor;
		data.emissionStrength = emissionStrength;
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

			if (image)
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

			if (!success)
			{
				ETextureSlot slot = static_cast<ETextureSlot>(i);
				switch (slot)
				{
				case ETextureSlot::Albedo:
					MaterialUtils::GetDefaultAlbedo()->Bind(i);
					break;
				case ETextureSlot::Normal:
					MaterialUtils::GetDefaultNormal()->Bind(i);
					break;
				case ETextureSlot::ORM:
					MaterialUtils::GetDefaultORM()->Bind(i);
					break;
				case ETextureSlot::Emission:
					MaterialUtils::GetDefaultEmission()->Bind(i);
					break;
				default:
					// Specular, Height 등 더미가 정의되지 않은 슬롯은 안전하게 Unbind 처리
					GLTextureHandle::Unbind(i);
					break;
				}
			}
		}
	}
}
