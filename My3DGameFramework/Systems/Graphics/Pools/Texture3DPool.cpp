#include "GraphicsPch.h"
#include "Texture3DPool.h"
#include "Utils/TextureUtils.h"
#include "Hashing/TextureHash.h"

namespace MGF3D
{
	Texture3DPool::Texture3DPool(uint32 maxCapacity) : Super(maxCapacity) { }
	Texture3DPool::~Texture3DPool() = default;

	GLTexture3DPtr Texture3DPool::Request
	(
		uint32 w, uint32 h, uint32 d,
		uint32 vkFormat,
		uint32 minFilter, uint32 magFilter,
		uint32 wrapS, uint32 wrapT, uint32 wrapR
	)
	{
		// 1. 요청 사양 기반으로 3D 해시 생성
		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		TextureHash textureHash(GL_TEXTURE_3D, internalFormat, w, h, d);

		// 2. 가용 리스트 확인 (SMap::Find 사용)
		auto pAvailableList = m_available.Find(textureHash.value);
		if (pAvailableList != nullptr && !pAvailableList->Empty())
		{
			// 3. 재활용: 리스트에서 마지막 인스턴스 추출
			auto texture = pAvailableList->PopBack();

			// 4. DSA를 이용한 샘플러 파라미터 업데이트
			uint32 handle = texture->GetHandle();
			glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, minFilter);
			glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, magFilter);
			glTextureParameteri(handle, GL_TEXTURE_WRAP_S, wrapS);
			glTextureParameteri(handle, GL_TEXTURE_WRAP_T, wrapT);
			glTextureParameteri(handle, GL_TEXTURE_WRAP_R, wrapR);

			return texture;
		}

		// 5. 없으면 지연 할당 생성
		return GLTexture3D::Create(w, h, d, vkFormat, nullptr, wrapS, wrapT, wrapR, minFilter, magFilter);
	}
}