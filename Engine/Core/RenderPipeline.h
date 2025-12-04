#pragma once

CLASS_PTR(RenderPipeline)
class RenderPipeline
{
public:
    virtual ~RenderPipeline();
    virtual void Render(Scene* scene) = 0;
    virtual void OnResize(int32 width, int32 height) = 0;

protected:
    RenderPipeline();
};