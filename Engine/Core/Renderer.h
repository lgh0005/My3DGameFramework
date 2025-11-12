#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Framebuffer)
CLASS_PTR(Program)
CLASS_PTR(Mesh)
CLASS_PTR(Material)
#pragma endregion

CLASS_PTR(Renderer)
class Renderer
{
public:
	static RendererUPtr Create(int32 width = WINDOW_WIDTH, 
							   int32 height = WINDOW_HEIGHT);

	void Render(Scene* scene);

protected:
	Renderer() = default;
	bool Init(int32 width = WINDOW_WIDTH, int32 height = WINDOW_HEIGHT);

// TEMP : 현재 작업 중인 렌더링 관련 멤버들을 이주
private:

};