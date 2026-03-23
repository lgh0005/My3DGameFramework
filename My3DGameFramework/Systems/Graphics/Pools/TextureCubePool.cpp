#include "GraphicsPch.h"
#include "TextureCubePool.h"
#include "Utils/TextureUtils.h"
#include "Hashing/TextureHash.h"

namespace MGF3D
{
	TextureCubePool::TextureCubePool(uint32 maxCapacity) : Super(maxCapacity) { }
	TextureCubePool::~TextureCubePool() = default;

	GLTextureCubePtr TextureCubePool::Request
	(
		uint32 size,
		uint32 vkFormat,
		uint32 minFilter, uint32 magFilter,
		uint32 wrapS, uint32 wrapT, uint32 wrapR
	)
	{
		// 1. 큐브맵 사양 기반 해시 생성
		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		TextureHash textureHash(GL_TEXTURE_CUBE_MAP, internalFormat, size, size, 1);

		// 2. 가용 리스트 확인
		auto pAvailableList = m_available.Find(textureHash.value);
		if (pAvailableList != nullptr && !pAvailableList->Empty())
		{
			// 3. 재활용: 리스트에서 인스턴스 추출
			auto texture = pAvailableList->PopBack();

			// 4. DSA 파라미터 업데이트 (S, T, R 모두 대응)
			uint32 handle = texture->GetHandle();
			glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, minFilter);
			glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, magFilter);
			glTextureParameteri(handle, GL_TEXTURE_WRAP_S, wrapS);
			glTextureParameteri(handle, GL_TEXTURE_WRAP_T, wrapT);
			glTextureParameteri(handle, GL_TEXTURE_WRAP_R, wrapR);

			return texture;
		}

		// 5. 없으면 지연 생성 (6개 면의 픽셀 데이터는 nullptr로 전달하여 도화지만 생성)
		const void* noPixels[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
		return GLTextureCube::Create
		(
			static_cast<int32>(size),
			vkFormat,
			noPixels,
			wrapS, wrapT, wrapR,
			minFilter, magFilter
		);
	}
}