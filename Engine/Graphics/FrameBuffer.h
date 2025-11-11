#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(Framebuffer)
class Framebuffer
{
public:
	static FramebufferUPtr Create(const TexturePtr colorAttachment);
	static void BindToDefault();
	~Framebuffer();

	const uint32 Get() const { return m_framebuffer; }
	void Bind() const;
	const TexturePtr GetColorAttachment() const { return m_colorAttachment; }

private:
	Framebuffer() = default;
	bool InitWithColorAttachment(const TexturePtr colorAttachment);

	uint32 m_framebuffer			{ 0 };
	uint32 m_depthStencilBuffer		{ 0 };
	TexturePtr m_colorAttachment;
};