#pragma once
#include "Graphics/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext) // TODO : 어차피 UIManager가 있음.
#pragma endregion

CLASS_PTR(UIWorldSpacePass)
class UIWorldSpacePass : public ContextRenderPass
{
public:
	virtual ~UIWorldSpacePass();
	virtual void Render(RenderContext* context);

private:
	UIWorldSpacePass();
};