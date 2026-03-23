#include "GraphicsPch.h"
#include "TextureManager.h"
#include "Textures/GLTexture.h"
#include "Textures/GLTexture2D.h"
#include "Textures/GLTexture3D.h"
#include "Textures/GLTextureCube.h"

namespace MGF3D
{
	TextureManager::TextureManager() = default;
	TextureManager::~TextureManager() = default;

	bool TextureManager::Init()
	{
		// 1. 전역 그래픽 설정
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		// 2. 최대 텍스쳐 바인딩 가능 수 확보 및 자료구조 초기화
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_maxTextureUnits);
		m_textureHandleCache.Resize(static_cast<usize>(m_maxTextureUnits));
		m_textureSlotUsage.Resize(static_cast<usize>(m_maxTextureUnits));

		// 3. 텍스처 하드웨어 크기 한계치 쿼리 및 캐싱
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
		glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &m_max3DTextureSize);
		glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &m_maxCubeMapTextureSize);

		return true;
	}

	void TextureManager::Shutdown()
	{
		// 1. 모든 하드웨어 슬롯 바인딩 해제
		ClearSlots();

		// 2. 각 타입별 인스턴스 풀 완전 정리
		m_pool2D.Clear();
		m_pool3D.Clear();
		m_poolCube.Clear();

		// 3. 관리 자료구조 메모리 해제
		m_textureHandleCache.Release();
		m_textureSlotUsage.Release();
	}

	int32 TextureManager::Bind(GLTexturePtr texture)
	{
		if (texture == nullptr) return -1;

		uint32 handle = texture->GetHandle();
		uint32 target = texture->GetTarget();

		// 1. 캐시 히트 검사: 이미 바인딩된 슬롯이 있는지 확인
		for (int32 i = 0; i < m_maxTextureUnits; ++i)
		{
			auto& state = m_textureHandleCache.GetData(i);
			if (state.handle == handle)
			{
				// 사용되었으므로 리스트의 맨 앞(MRU)으로 이동
				m_textureHandleCache.MoveToFront(i);
				m_textureSlotUsage.Set(i, true);
				return i;
			}
		}

		// 2. 새로운 빈 슬롯 찾기
		int32 slot = m_textureSlotUsage.FindFirstUnset();

		// 3. 빈 슬롯이 없다면 LRU 교체 알고리즘 가동
		if (slot == -1)
		{
			// 리스트의 맨 뒤(Tail)가 가장 오랫동안 쓰이지 않은 녀석임
			slot = m_textureHandleCache.GetTail();
			if (slot != -1) UnbindSlot(static_cast<uint32>(slot));
		}

		// 3-1. LRU 교체를 수행해도 없는 경우 처리
		if (slot == -1) return -1;

		// 4. 실제 바인딩 수행 (다형성 활용)
		texture->Bind(static_cast<uint32>(slot));

		// 5. 캐시 및 상태 업데이트
		auto& state = m_textureHandleCache[slot];
		state.handle = handle;
		state.target = target;

		m_textureHandleCache.MoveToFront(slot);
		m_textureSlotUsage.Set(slot, true);

		// 6. 할당된 슬롯 번호 반환
		return slot;
	}

	void TextureManager::Unbind(GLTexturePtr texture)
	{
		if (texture == nullptr) return;

		uint32 handle = texture->GetHandle();

		// 해당 핸들을 가진 모든 슬롯을 찾아 해제
		for (int32 i = 0; i < m_maxTextureUnits; ++i)
		{
			if (m_textureHandleCache.GetData(i).handle == handle)
				UnbindSlot(static_cast<uint32>(i));
		}
	}

	void TextureManager::UnbindSlot(uint32 slot)
	{
		if (slot >= static_cast<uint32>(m_maxTextureUnits)) return;

		auto& state = m_textureHandleCache[slot];
		if (state.handle == 0) return;

		glBindTextureUnit(slot, 0);

		state.handle = 0;
		state.target = 0;
		m_textureSlotUsage.Set(slot, false);
	}

	void TextureManager::Release(GLTexturePtr texture)
	{
		if (texture == nullptr) return;

		// 0. 텍스쳐 언바인드
		Unbind(texture);

		// 2. 타겟에 따라 적절한 풀로 반납
		switch (texture->GetTarget())
		{
			case GL_TEXTURE_2D:
			{
				m_pool2D.Release(StaticSharedCast<GLTexture2D>(texture));
				break;
			}
			case GL_TEXTURE_3D:
			{
				m_pool3D.Release(StaticSharedCast<GLTexture3D>(texture));
				break;
			}
			case GL_TEXTURE_CUBE_MAP:
			{
				m_poolCube.Release(StaticSharedCast<GLTextureCube>(texture));
				break;
			}
		}
	}

	void TextureManager::Flush()
	{
		m_textureSlotUsage.Clear();
	}

	int32 TextureManager::FindEmptySlot() const
	{
		return m_textureSlotUsage.FindFirstUnset();
	}

	void TextureManager::ClearSlots()
	{
		for (int32 i = 0; i < m_maxTextureUnits; ++i)
			UnbindSlot(static_cast<uint32>(i));

		// 모든 상태가 초기화되었으므로 사용량 비트셋도 정리
		m_textureSlotUsage.Clear();
	}

	/*=============================//
	//   Texture request methods   //
	//=============================*/
	GLTexture2DPtr TextureManager::RequestTexture2D(uint32 w, uint32 h, uint32 vkFormat, uint32 minF, uint32 magF, uint32 wrapS, uint32 wrapT)
	{
		return m_pool2D.Request(w, h, vkFormat, minF, magF, wrapS, wrapT);
	}

	GLTexture3DPtr TextureManager::RequestTexture3D(uint32 w, uint32 h, uint32 d, uint32 vkFormat, uint32 minF, uint32 magF, uint32 wrapS, uint32 wrapT, uint32 wrapR)
	{
		return m_pool3D.Request(w, h, d, vkFormat, minF, magF, wrapS, wrapT, wrapR);
	}

	GLTextureCubePtr TextureManager::RequestTextureCube(uint32 size, uint32 vkFormat, uint32 minF, uint32 magF, uint32 wrapS, uint32 wrapT, uint32 wrapR)
	{
		return m_poolCube.Request(size, vkFormat, minF, magF, wrapS, wrapT, wrapR);
	}
}