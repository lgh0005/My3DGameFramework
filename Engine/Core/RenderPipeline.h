#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderPass)
#pragma endregion

CLASS_PTR(RenderPipeline)
class RenderPipeline
{
public:
    virtual ~RenderPipeline() = default;
    virtual void Render(Scene* scene) = 0;
    virtual void OnResize(int32 width, int32 height) = 0;
    void AddRenderPass(const std::string& name, RenderPassUPtr renderPass);
    RenderPass* GetRenderPass(const std::string& name);

protected:
    RenderPipeline() = default;

    // TODO : 업데이트 시 순서 보장이 안될 수 있음.
    // 그 때는 vector 도입을 고려해볼 필요가 있음.
    std::unordered_map<std::string, RenderPassUPtr> m_renderPasses;
};