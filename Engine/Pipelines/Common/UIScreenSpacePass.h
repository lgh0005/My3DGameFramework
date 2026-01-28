#pragma once
#include "Graphics/Rendering/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
#pragma endregion

CLASS_PTR(UIScreenSpacePass)
class UIScreenSpacePass : public ContextRenderPass
{
public:
	virtual ~UIScreenSpacePass();
	virtual void Render(RenderContext* context);

private:
	UIScreenSpacePass();

};