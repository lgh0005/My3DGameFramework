#pragma once
#include "Core/RenderContext.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Framebuffer)
CLASS_PTR(PostProcessFramebuffer)
CLASS_PTR(GBufferFramebuffer)
CLASS_PTR(Texture)
#pragma endregion

class UniversalRenderContext : public RenderContext
{
public:
	UniversalRenderContext();
	virtual ~UniversalRenderContext();

	// For DeferredLightingPass : draw on Post-processing framebuffer
	void SetTargetFramebuffer(PostProcessFramebuffer* fbo) { m_targetFramebuffer = fbo; }
	PostProcessFramebuffer* GetTargetFramebuffer() const { return m_targetFramebuffer; }

	// For GeometryPass
	void SetGBuffer(GBufferFramebuffer* gBuffer);
	GBufferFramebuffer* GetGBuffer() const { return m_gBuffer; }
	Texture* GetGBufferPosition() const { return m_gPosition; }
	Texture* GetGBufferNormal()   const { return m_gNormal; }
	Texture* GetGBufferAlbedo() const { return m_gAlbedoSpec; }
	Texture* GetGBufferEmission() const { return m_gEmission; }

	// For UniversalSSAOPass
	void SetSSAOTexture(Texture* texture) { m_ssaoTexture = texture; }
	Texture* GetSSAOTexture() const { return m_ssaoTexture; }

private:

	// For DeferredLightingPass : draw on Post-processing framebuffer
	PostProcessFramebuffer* m_targetFramebuffer{ nullptr };

	// For GeometryPass 
	GBufferFramebuffer* m_gBuffer{ nullptr };
	Texture* m_gPosition{ nullptr };
	Texture* m_gNormal{ nullptr };
	Texture* m_gAlbedoSpec{ nullptr };
	Texture* m_gEmission{ nullptr };

	// For UniversalSSAOPass
	Texture* m_ssaoTexture{ nullptr };
};