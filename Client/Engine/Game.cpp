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
	GRAPHICS.Init();
    GRAPHICS.SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
}

void Game::Launch(const string& sceneName)
{
    // Load first scene
    SCENE.LoadScene(sceneName);

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

            // ESC 키로 종료
            if (_event.type == SDL_KEYDOWN)
            {
                if (_event.key.keysym.sym == SDLK_ESCAPE)
                    _running = false;
            }

            // Input event polling
            INPUT.GetEvent(_event);
        }

        // Update Logic
        Update();

        // TODO : Render Logics
    }
}

void Game::Update()
{
    // Update Managers
    SCENE.Update();
    INPUT.Update();
}