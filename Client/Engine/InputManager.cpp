#include "pch.h"
#include "InputManager.h"

InputManager::~InputManager()
{
}

void InputManager::Init()
{
}

void InputManager::Update()
{
	for (auto& state : _keyboardState)
	{
		if (state == InputState::Down) state = InputState::Press;
		else if (state == InputState::Up) state = InputState::None;
	}

	for (auto& state : _mouseState)
	{
		if (state == InputState::Down) state = InputState::Press;
		else if (state == InputState::Up) state = InputState::None;
	}

	_mouseDelta = glm::vec2(0.f, 0.f);
	_mouseWheel = 0;
}

void InputManager::GetEvent(const SDL_Event& e)
{
	switch (e.type)
	{
		// Keyboard
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			SDL_Scancode scancode = e.key.keysym.scancode;
			InputState& state = _keyboardState[scancode];

			if (e.type == SDL_KEYDOWN)
			{
				if (state == InputState::Press || state == InputState::Down)
					state = InputState::Press;
				else
					state = InputState::Down;
			}
			else // SDL_KEYUP
			{
				if (state == InputState::Press || state == InputState::Down)
					state = InputState::Up;
				else
					state = InputState::None;
			}
			break;
		}

		// Mouse
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
			int button = e.button.button;
			if (button < SDL_BUTTON_LEFT || button > SDL_BUTTON_X2) break;
			InputState& state = _mouseState[button];

			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (state == InputState::Press || state == InputState::Down)
					state = InputState::Press;
				else
					state = InputState::Down;
			}
			else // SDL_MOUSEBUTTONUP
			{
				if (state == InputState::Press || state == InputState::Down)
					state = InputState::Up;
				else
					state = InputState::None;
			}
			break;
		}
		case SDL_MOUSEMOTION:
		{
			_mouseDelta += glm::vec2(e.motion.xrel, e.motion.yrel);
			_mousePosition = glm::vec2(e.motion.x, e.motion.y);
			break;
		}
		case SDL_MOUSEWHEEL:
		{
			_mouseWheel = static_cast<int8>(e.wheel.y);
			break;
		}
	}
}