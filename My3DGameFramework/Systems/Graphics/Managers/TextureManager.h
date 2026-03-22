#pragma once

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
		void   Flush();
		
	public:
		// TODO:
		// 1. RequestTexture2D
		// 2. RequestTexture3D
		// 3. RequestTextureCube

	private:
		int32 FindEmptySlot() const;
		void  ClearSlots();

	private:
		int32 m_maxTextureUnits	{ 0 };
		SStaticLinkedList<TextureSlotState> m_textureHandleCache;
		SBitset m_textureSlotUsage;

		// 1. Texture2D 실제 GPU 상으로 적재된 인스턴스 풀
		// 크기별로 256, 512, 1024, 2048, 4096을 미리 풀에다가 생성해둠. 개수는 고려 필요
	
		// 2. Texture3D 실제 GPU 상으로 적재된 인스턴스 풀
		

		// 3. TextureCube 실제 GPU 상으로 적재된 인스턴스 풀
		// 대체로 한 개는 미리 만들어두는 것이 좋을 듯 함
	};
}