#pragma once
#include "ISingleton.h"

class Camera;
class IRenderable;

class RenderManager : public ISingleton<RenderManager>
{
	DECLARE_SINGLE(RenderManager);

public:
	virtual ~RenderManager() override;

public:
	virtual void Init() override;
	virtual void Update() override;
	void Render();
	
public:
	void SetClearColor(const array<GLclampf, 4>& clearColor);

private:
	SDL_Window* _window;
	SDL_GLContext _glContext;
	int _bufferWidth = 0;
	int _bufferHeight = 0;
	array<GLclampf, 4> _clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
};