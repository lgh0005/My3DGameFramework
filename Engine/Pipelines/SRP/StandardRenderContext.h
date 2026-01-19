#pragma once
#include "Graphics/RenderContext.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Framebuffer)
CLASS_PTR(PostProcessFramebuffer)
CLASS_PTR(GBufferFramebuffer)
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(StandardRenderContext)
class StandardRenderContext : public RenderContext
{
public:
	StandardRenderContext();
	virtual ~StandardRenderContext();

	// draw on Post-processing framebuffer
	void SetTargetFramebuffer(PostProcessFramebuffer* fbo) { m_targetFramebuffer = fbo; }
	PostProcessFramebuffer* GetTargetFramebuffer() const { return m_targetFramebuffer; }
	void BindTargetFramebuffer();

	// For StandardGeometryPass
	void SetGBuffer(GBufferFramebuffer* gBuffer);
	GBufferFramebuffer* GetGBuffer() const { return m_gBuffer; }
	Texture* GetGBufferPosition() const { return m_gPosition; }
	Texture* GetGBufferNormal()   const { return m_gNormal; }
	Texture* GetGBufferAlbedo() const { return m_gAlbedoSpec; }
	Texture* GetGBufferEmission() const { return m_gEmission; }
	Texture* GetGBufferVelocity() const { return m_gVelocity; }

	// For StandardSSAOPass
	void SetSSAOTexture(Texture* texture) { m_ssaoTexture = texture; }
	Texture* GetSSAOTexture() const { return m_ssaoTexture; }

private:

	// For DeferredLightingPass : draw on Post-processing framebuffer
	PostProcessFramebuffer* m_targetFramebuffer{ nullptr };

	// For StandardGeometryPass 
	GBufferFramebuffer* m_gBuffer{ nullptr };
	Texture* m_gPosition{ nullptr };
	Texture* m_gNormal{ nullptr };
	Texture* m_gAlbedoSpec{ nullptr };
	Texture* m_gEmission{ nullptr };
	Texture* m_gVelocity{ nullptr };

	// For StandardSSAOPass
	Texture* m_ssaoTexture{ nullptr };
};