#pragma once
#include "Core/RenderContext.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Framebuffer)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Light)
CLASS_PTR(Texture)
CLASS_PTR(CubeTexture)
#pragma endregion

CLASS_PTR(StandardRenderContext)
class StandardRenderContext : public RenderContext
{
public:
	StandardRenderContext() = default;

	void SetGBuffer(Framebuffer* gBuffer);
	Framebuffer* GetGBuffer() const { return m_gBuffer; }\

	Texture* GetGBufferPosition() const { return m_gPosition; }
	Texture* GetGBufferNormal()   const { return m_gNormal; }
	Texture* GetGBufferAlbedo() const { return m_gAlbedoSpec; }
	Texture* GetGBufferEmission() const { return m_gEmission; }

	void SetSSAOTexture(Texture* texture) { m_ssaoTexture = texture; }
	Texture* GetSSAOTexture() const { return m_ssaoTexture; }

	void SetTargetFramebuffer(Framebuffer* fbo) { m_targetFramebuffer = fbo; }
	Framebuffer* GetTargetFramebuffer() const { return m_targetFramebuffer; }

	void SetShadowMap(int32 index, Texture* texture);
	Texture* GetShadowMap(int32 index) const;

	void SetSkyboxTexture(CubeTexture* texture) { m_skyboxTexture = texture; }
	CubeTexture* GetSkyboxTexture() const { return m_skyboxTexture; }

private:
	Framebuffer* m_gBuffer{ nullptr };
	Texture* m_gPosition{ nullptr };
	Texture* m_gNormal{ nullptr };
	Texture* m_gAlbedoSpec{ nullptr };
	Texture* m_gEmission{ nullptr };
	Texture* m_ssaoTexture{ nullptr };
	Framebuffer* m_targetFramebuffer{ nullptr };
	Texture* m_shadowMaps[MAX_SHADOW_CASTER] = { nullptr, };
	CubeTexture* m_skyboxTexture{ nullptr };
};