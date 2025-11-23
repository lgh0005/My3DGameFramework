#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
#pragma endregion

CLASS_PTR(Renderer)
class Renderer
{
public:
    virtual ~Renderer();
    virtual void Render(Scene* scene)            = 0;

protected:
	Renderer() = default;
};