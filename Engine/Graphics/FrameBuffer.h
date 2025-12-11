#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
#pragma endregion

// TODO : 이제 PBR 전용 GBuffer, SRP 전용 GBuffer를 따로 분리할 필요가 있다
CLASS_PTR(Framebuffer)
class Framebuffer
{
public:
	static FramebufferUPtr Create(int32 width, int32 height, int32 samples = 4);
	static FramebufferUPtr CreateGBuffer(int32 width, int32 height);
	static FramebufferUPtr CreateSSAO(int32 width, int32 height);
	static FramebufferUPtr CreateBRDFLUT(int32 width, int32 height);
	static void BindToDefault();
	~Framebuffer();

	const uint32 Get() const;
	void Bind() const;
	void Resolve() const;
	const TexturePtr GetColorAttachment(int32 index = 0) const;

	int32 GetWidth()  { return m_width; }
	int32 GetHeight() { return m_height; }

private:
	Framebuffer() = default;
	bool Init(int32 width, int32 height, int32 samples);
	bool InitGBuffer(int32 width, int32 height);
	bool InitSSAO(int32 width, int32 height);
	bool InitBRDFLUT(int32 width, int32 height);

	int32 m_width						{ 0 };
	int32 m_height						{ 0 };
	int32 m_samples						{ 0 };

	uint32 m_msaaFbo					{ 0 };
	uint32 m_msaaDepthStencilBuffer		{ 0 }; 
	uint32 m_resolveFbo					{ 0 };

	std::vector<TexturePtr> m_resolveTextures;
	std::vector<uint32>		m_msaaColorBuffers;
};