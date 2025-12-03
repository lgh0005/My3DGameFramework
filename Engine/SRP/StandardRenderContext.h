#pragma once
#include "Core/RenderContext.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Framebuffer)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Light)
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(StandardRenderContext)
class StandardRenderContext : public RenderContext
{
public:
	StandardRenderContext() = default;

	void SetGBuffer(Framebuffer* gBuffer);

	Framebuffer* GetGBuffer() const { return m_gBuffer; }
	
	Texture* GetGBufferPosition() const { return m_gPosition; }
	Texture* GetGBufferNormal()   const { return m_gNormal; }
	Texture* GetGBufferAlbedo() const { return m_gAlbedoSpec; }
	Texture* GetGBufferEmission() const { return m_gEmission; }

	void SetSSAOTexture(Texture* texture) { m_ssaoTexture = texture; }
	Texture* GetSSAOTexture() const { return m_ssaoTexture; }

private:
	Framebuffer* m_gBuffer{ nullptr };
	Texture* m_gPosition{ nullptr };
	Texture* m_gNormal{ nullptr };
	Texture* m_gAlbedoSpec{ nullptr };
	Texture* m_gEmission{ nullptr };
	Texture* m_ssaoTexture{ nullptr };

// TEMP : CullingPass 구현 전, 그냥 씬에 있는 것을 그대로 복사.
public:
	void AddStaticMeshRenderer(MeshRenderer* renderer);
	void AddSkinnedMeshRenderer(MeshRenderer* renderer);
	void AddLight(Light* light);
};