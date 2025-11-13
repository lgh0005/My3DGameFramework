#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
#pragma endregion

CLASS_PTR(Renderer)
class Renderer
{
public:
    virtual ~Renderer() = default;
    virtual void Render(Scene* scene)            = 0;
    virtual bool Init(int32 width, int32 height) = 0;

protected:
	Renderer() = default;
};