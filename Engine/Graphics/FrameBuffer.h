#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(Framebuffer)
class Framebuffer
{
public:
	static FramebufferUPtr Create(int32 width, int32 height, int32 samples = 4);
	static void BindToDefault();
	~Framebuffer();

	const uint32 Get() const { return m_msaaFbo; }
	void Bind() const;
	void Resolve() const;
	const TexturePtr GetColorAttachment() const { return m_resolveTexture; }

private:
	Framebuffer() = default;
	bool Init(int32 width, int32 height, int32 samples);

	int32 m_width						{ 0 };
	int32 m_height						{ 0 };
	int32 m_samples						{ 0 };

	uint32 m_msaaFbo					{ 0 };
	uint32 m_msaaColorBuffer			{ 0 };
	uint32 m_msaaDepthStencilBuffer		{ 0 }; 

	uint32 m_resolveFbo					{ 0 };
	TexturePtr m_resolveTexture;
};