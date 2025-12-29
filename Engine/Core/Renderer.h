#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Framebuffer)
CLASS_PTR(RenderPipeline)
#pragma endregion

CLASS_PTR(Renderer)
class Renderer
{
public:
    ~Renderer();
    static RendererUPtr Create();

    void Render(Scene* scene);
    void OnResize(int32 width, int32 height);
    void SetPipeline(RenderPipelineUPtr pipeline);
    RenderPipeline* GetPipeline() const { return m_pipeline.get(); }

protected:
	Renderer();
    void Init();
    RenderPipelineUPtr m_pipeline;
};