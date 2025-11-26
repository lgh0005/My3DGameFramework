#pragma once
#include "Core/Renderer.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Framebuffer)
CLASS_PTR(CubeTexture)
CLASS_PTR(SpotLight)
CLASS_PTR(ShadowMap)
#pragma endregion

CLASS_PTR(DevRenderer)
class DevRenderer : public Renderer
{
public:
	static DevRendererUPtr Create(int32 width, int32 height);
	virtual void Render(Scene* scene) override;

private:
	DevRenderer() = default;

	// TEMP : 디버그를 위한 ImGUIManager의 컨텍스트
	void RenderImGUIContext(Scene* scene);
};