#pragma once
#include "Pools/Texture2DPool.h"
#include "Pools/Texture3DPool.h"
#include "Pools/TextureCubePool.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTexture);

	struct TextureSlotState
	{
		uint32 handle	{ 0 };
		uint32 target	{ 0 };
	};

	class TextureManager
	{
		MGF_DECLARE_SINGLE(TextureManager)

	private:
		TextureManager();
		~TextureManager();

	public:
		bool Init();
		void Shutdown();

	public:
		int32  Bind(GLTexturePtr texture);
		void   Unbind(GLTexturePtr texture);
		void   UnbindSlot(uint32 slot);
		void   Release(GLTexturePtr texture);
		void   Flush();
		
	/*=============================//
	//   Texture request methods   //
	//=============================*/
	public:
		// 1. Texture2D 요청
		GLTexture2DPtr RequestTexture2D
		(
			uint32 w, uint32 h, uint32 vkFormat,
			uint32 minF = GL_LINEAR, uint32 magF = GL_LINEAR,
			uint32 wrapS = GL_CLAMP_TO_EDGE, 
			uint32 wrapT = GL_CLAMP_TO_EDGE
		);

		// 2. Texture3D 요청
		GLTexture3DPtr RequestTexture3D
		(
			uint32 w, uint32 h, uint32 d, uint32 vkFormat,
			uint32 minF = GL_LINEAR, uint32 magF = GL_LINEAR,
			uint32 wrapS = GL_CLAMP_TO_EDGE, 
			uint32 wrapT = GL_CLAMP_TO_EDGE, 
			uint32 wrapR = GL_CLAMP_TO_EDGE
		);

		// 3. TextureCube 요청
		GLTextureCubePtr RequestTextureCube
		(
			uint32 size, uint32 vkFormat,
			uint32 minF = GL_LINEAR, uint32 magF = GL_LINEAR,
			uint32 wrapS = GL_CLAMP_TO_EDGE, 
			uint32 wrapT = GL_CLAMP_TO_EDGE, 
			uint32 wrapR = GL_CLAMP_TO_EDGE
		);

	private:
		int32 FindEmptySlot() const;
		void  ClearSlots();

	private:
		int32 m_maxTextureUnits	{ 0 };
		SStaticLinkedList<TextureSlotState> m_textureHandleCache;
		SBitset m_textureSlotUsage;

		// 타입별 인스턴스 풀
		Texture2DPool   m_pool2D;
		Texture3DPool   m_pool3D;
		TextureCubePool m_poolCube;
	};
}