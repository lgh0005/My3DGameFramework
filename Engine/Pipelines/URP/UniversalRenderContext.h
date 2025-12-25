#pragma once
#include "Core/RenderContext.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Framebuffer)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Light)
CLASS_PTR(Texture)
CLASS_PTR(CubeTexture)
#pragma endregion

class UniversalRenderContext : public RenderContext
{
public:
	UniversalRenderContext() = default;

	// For DeferredLightingPass : Target Framebuffer
	void SetTargetFramebuffer(Framebuffer* fbo) { m_targetFramebuffer = fbo; }
	Framebuffer* GetTargetFramebuffer() const { return m_targetFramebuffer; }

	// For GeometryPass
	void SetGBuffer(Framebuffer* gBuffer);
	Framebuffer* GetGBuffer() const { return m_gBuffer; }
	Texture* GetGBufferPosition() const { return m_gPosition; }
	Texture* GetGBufferNormal()   const { return m_gNormal; }
	Texture* GetGBufferAlbedo() const { return m_gAlbedoSpec; }
	Texture* GetGBufferEmission() const { return m_gEmission; }

	// For UniversalSSAOPass
	void SetSSAOTexture(Texture* texture) { m_ssaoTexture = texture; }
	Texture* GetSSAOTexture() const { return m_ssaoTexture; }

private:

	// For DeferredLightingPass : Target Framebuffer
	Framebuffer* m_targetFramebuffer{ nullptr };

	// For GeometryPass 
	Framebuffer* m_gBuffer{ nullptr };
	Texture* m_gPosition{ nullptr };
	Texture* m_gNormal{ nullptr };
	Texture* m_gAlbedoSpec{ nullptr };
	Texture* m_gEmission{ nullptr };

	// For UniversalSSAOPass
	Texture* m_ssaoTexture{ nullptr };
};