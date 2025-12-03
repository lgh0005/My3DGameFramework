#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
#pragma endregion

class GlobalUniforms
{
public:
	virtual ~GlobalUniforms() = default;
	virtual void PreRender(RenderContext* context) = 0;

protected:
	GlobalUniforms() = default;
};