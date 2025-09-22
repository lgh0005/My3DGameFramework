#pragma once

class Scene
{
public:
	Scene(const string& name);
	virtual ~Scene() = default;

public:
	virtual void Init();
	virtual void Update();

public:
	const string& GetName() { return _name; }

protected:
	string _name;
	SDL_Window* _window;
};

