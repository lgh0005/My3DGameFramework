#pragma once
#include "ISingleton.h"

class Graphics : public ISingleton<Graphics>
{
	DECLARE_SINGLE(Graphics);

public:
	virtual ~Graphics() override;

public: 
	virtual void Init() override;
	virtual void Update() override {};

public:
	void SetClearColor(const array<GLclampf, 4>& clearColor);
	SDL_Window* GetWindow() { return _window; }

private:
	SDL_Window* _window;
	SDL_GLContext _glContext;
	int _bufferWidth = 0;
	int _bufferHeight = 0;
	array<GLclampf, 4> _clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
};

