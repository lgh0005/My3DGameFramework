#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
#pragma endregion

CLASS_PTR(RenderPass)
class RenderPass
{
public:
	virtual ~RenderPass();
	virtual void Render(Scene* scene, Camera* camera) = 0;

protected:
	RenderPass() = default;
};