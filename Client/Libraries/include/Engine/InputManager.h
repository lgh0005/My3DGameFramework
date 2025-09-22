#pragma once
#include "ISingleton.h"

enum class InputState
{
	None,
	Press,
	Down,
	Up,
	END
};

class InputManager : public ISingleton<InputManager>
{
	DECLARE_SINGLE(InputManager);

public:
	virtual ~InputManager() override;

public:
	virtual void Init() override;
	virtual void Update() override;

public:
	void GetEvent(const SDL_Event& e);

	// Input : Keyboard
	bool GetKey(Inputs::Key key) { return InputState::Press == GetKeyState(key); }
	bool GetKeyDown(Inputs::Key key) { return InputState::Down == GetKeyState(key); }
	bool GetKeyUp(Inputs::Key key) { return InputState::Up == GetKeyState(key); }
	bool GetKeymodDown(Inputs::Keymod keymod) { return (SDL_GetModState() & static_cast<SDL_Keymod>(keymod)) != 0; }

	// Input : Mouse
	bool MouseHold(Inputs::Mouse button) { return InputState::Press == GetMouseState(button); }
	bool MouseClick(Inputs::Mouse button) { return InputState::Down == GetMouseState(button); }
	bool MouseUp(Inputs::Mouse button) { return InputState::Up == GetMouseState(button); }
	int8 GetMouseWheel() const { return _mouseWheel; }
	glm::vec2 GetMouseDelta() const { return _mouseDelta; }

private:
	InputState GetKeyState(Inputs::Key key) { return _keyboardState[static_cast<uint16>(key)]; }
	InputState GetMouseState(Inputs::Mouse mouse) { return _mouseState[static_cast<uint8>(mouse)]; }
	// TODO : Mouse Wheel

private:
	array<InputState, SDL_NUM_SCANCODES> _keyboardState{};
	SDL_Keymod _keymodState = KMOD_NONE;

	array<InputState, Inputs::MOUSE_COUNT> _mouseState{};

	glm::vec2 _mousePosition;
	glm::vec2 _mouseDelta;
	int8 _mouseWheel = 0;
};

