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
		
	private:
		int32 FindEmptySlot() const;
		void  ClearSlots();

	private:
		int32 m_maxTextureUnits	{ 0 };
		SStaticLinkedList<TextureSlotState> m_textureHandleCache;
		SBitset m_textureSlotUsage;
	};
}