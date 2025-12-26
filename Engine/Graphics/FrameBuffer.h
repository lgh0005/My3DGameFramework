#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(Framebuffer)
class Framebuffer
{
public:
	static FramebufferUPtr Create(int32 width, int32 height, int32 samples = 4);
	static FramebufferUPtr CreateGBuffer(int32 width, int32 height);
	static FramebufferUPtr CreateSSAO(int32 width, int32 height);
	static FramebufferUPtr CreateBRDFLUT(int32 width, int32 height);
	static FramebufferUPtr CreatePostProcess(int32 width, int32 height, bool useDepth); // TEST
	static void BindToDefault();
	~Framebuffer();

	const uint32 Get() const;
	void Bind() const;
	void Resolve() const;
	void AttachTexture(Texture* texture, int32 attachmentIndex = 0);
	const TexturePtr GetColorAttachment(int32 index = 0) const;

	int32 GetWidth() { return m_width; }
	int32 GetHeight() { return m_height; }

private:
	Framebuffer() = default;
	bool Init(int32 width, int32 height, int32 samples);
	bool InitGBuffer(int32 width, int32 height);
	bool InitSSAO(int32 width, int32 height);
	bool InitBRDFLUT(int32 width, int32 height);
	bool InitPostProcess(int32 width, int32 height, bool useDepth); // TEST

	int32 m_width{ 0 };
	int32 m_height{ 0 };
	int32 m_samples{ 0 };

	uint32 m_msaaFbo{ 0 };
	uint32 m_msaaDepthStencilBuffer{ 0 };
	uint32 m_resolveFbo{ 0 };

	std::vector<TexturePtr> m_resolveTextures;
	std::vector<uint32>		m_msaaColorBuffers;
};