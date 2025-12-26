#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
#pragma endregion

// TODO : 이제 PBR 전용 GBuffer, SRP 전용 GBuffer를 따로 분리할 필요가 있다
// TODO : 보아하니 포스트 프로세싱 전용 Framebuffer에 대해서는 조금 변수, 메서드 이름의 수정이
// 필요해 보인다. SRP와 URP에서 쓰이는 포스트 프로세스 로직이 조금씩 틀릴 수 있기에 각각에 대해서
// Create가 필요할 수 있을 듯 하다.
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

	int32 GetWidth()  { return m_width; }
	int32 GetHeight() { return m_height; }

private:
	Framebuffer() = default;
	bool Init(int32 width, int32 height, int32 samples);
	bool InitGBuffer(int32 width, int32 height);
	bool InitSSAO(int32 width, int32 height);
	bool InitBRDFLUT(int32 width, int32 height);
	bool InitPostProcess(int32 width, int32 height, bool useDepth); // TEST

	int32 m_width						{ 0 };
	int32 m_height						{ 0 };
	int32 m_samples						{ 0 };

	uint32 m_msaaFbo					{ 0 };
	uint32 m_msaaDepthStencilBuffer		{ 0 }; 
	uint32 m_resolveFbo					{ 0 };

	std::vector<TexturePtr> m_resolveTextures;
	std::vector<uint32>		m_msaaColorBuffers;
};