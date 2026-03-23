#include "GraphicsPch.h"
#include "Texture2DPool.h"
#include "Utils/TextureUtils.h"
#include "Hashing/TextureHash.h"

namespace MGF3D
{
	Texture2DPool::Texture2DPool(uint32 maxCapacity) : Super(maxCapacity) { }
	Texture2DPool::~Texture2DPool() = default;

	GLTexture2DPtr Texture2DPool::Request
	(
		uint32 w, uint32 h, 
		uint32 vkFormat, 
		uint32 minFilter, uint32 magFilter, 
		uint32 wrapS, uint32 wrapT
	)
	{
		// 1. 요청 사양 기반으로 즉석 해시 생성
		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		TextureHash textureHash(GL_TEXTURE_2D, internalFormat, w, h, 1);

		// 2. 부모 클래스의 가용 리스트 검색
		auto pAvailableList = m_available.Find(textureHash);
		if (pAvailableList != nullptr && !pAvailableList->Empty())
		{
			// 리스트에서 하나 꺼내기 (재활용)
			auto texture = pAvailableList->PopBack();

			// 3. DSA를 이용한 샘플러 파라미터 갱신
			uint32 handle = texture->GetHandle();
			glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, minFilter);
			glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, magFilter);
			glTextureParameteri(handle, GL_TEXTURE_WRAP_S, wrapS);
			glTextureParameteri(handle, GL_TEXTURE_WRAP_T, wrapT);

			return texture;
		}

		// 4. 가용 자원이 없는 경우 지연 생성
		return GLTexture2D::Create(w, h, vkFormat, nullptr, wrapS, wrapT, minFilter, magFilter);
	}
}