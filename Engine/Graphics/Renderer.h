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

    void SetRefreshRate(int32 refreshRate);
    int32 GetRefreshRate() const { return m_refreshRate; }
    float GetTargetShutterSpeed() const { return m_targetShutterSpeed; }

protected:
	Renderer();
    bool Init();
    
    RenderPipelineUPtr m_pipeline;
    int32 m_refreshRate           { 60 };
    float m_targetShutterSpeed    { 1.0f / 60.0f };
};