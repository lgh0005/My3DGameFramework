#include "pch.h"
#include "Game.h"

unique_ptr<Game> GAME = make_unique<Game>();

void Game::Init()
{
	// Init SDL
	SDL_Init(SDL_INIT_VIDEO);

	// Init Font
	TTF_Init();

	// Init Managers
	RENDER.Init();
    RENDER.SetClearColor({ 0.2f, 0.0f, 0.0f, 1.0f });
}

void Game::Launch()
{
    // Event Looping
    while (_running)
    {
        // Update TimeManager
        TIME.Update();

        // Event Polling
        while (SDL_PollEvent(&_event))
        {
            if (_event.type == SDL_QUIT)
                _running = false;

            // Input event polling
            INPUT.GetEvent(_event);
        }

        // Update Logic
        Update();

        // Render game
        RENDER.Render();
    }
}

void Game::Update()
{
    // Update Managers
    INPUT.Update();
}