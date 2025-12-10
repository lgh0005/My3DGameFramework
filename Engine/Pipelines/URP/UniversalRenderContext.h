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

	void SetSkyboxTexture(CubeTexture* texture) { m_skyboxTexture = texture; }
	CubeTexture* GetSkyboxTexture() const { return m_skyboxTexture; }

private:
	CubeTexture* m_skyboxTexture{ nullptr };
};