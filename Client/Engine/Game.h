#pragma once

class Game
{
public:
	Game() = default;
	~Game() = default;

public:
	void Init();
	void Launch();
	void Update();

private:
	SDL_Event _event;
	bool _running = true;
};

extern unique_ptr<Game> GAME;