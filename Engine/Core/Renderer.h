#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Framebuffer)
#pragma endregion

CLASS_PTR(Renderer)
class Renderer
{
public:
    virtual ~Renderer();
    virtual void Render(Scene* scene)            = 0;

protected:
	Renderer() = default;
    void BlitCopyDepth(Framebuffer* src, Framebuffer* dst, 
                       int32 width = WINDOW_WIDTH, 
                       int32 height = WINDOW_HEIGHT);
};