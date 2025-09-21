#pragma once

namespace Inputs
{
    enum class Key : uint16
    {
        Unknown = SDL_SCANCODE_UNKNOWN,
        W = SDL_SCANCODE_W,
        A = SDL_SCANCODE_A,
        S = SDL_SCANCODE_S,
        D = SDL_SCANCODE_D,
        Right = SDL_SCANCODE_RIGHT,
        Left = SDL_SCANCODE_LEFT,
        Down = SDL_SCANCODE_DOWN,
        Up = SDL_SCANCODE_UP,
    };

    enum class Keymod : uint16
    {
        None = KMOD_NONE,
        LShift = KMOD_LSHIFT,
        RShift = KMOD_RSHIFT,
        LCtrl = KMOD_LCTRL,
        RCtrl = KMOD_RCTRL,
        LAlt = KMOD_LALT,
        RAlt = KMOD_RALT,
        LGui = KMOD_LGUI,
        RGui = KMOD_RGUI,
        Num = KMOD_NUM,
        Caps = KMOD_CAPS,
        Mode = KMOD_MODE,
        Scroll = KMOD_SCROLL,

        // combinated flags
        Ctrl = KMOD_CTRL,
        Shift = KMOD_SHIFT,
        Alt = KMOD_ALT,
        Gui = KMOD_GUI
    };

    enum class Mouse : uint8
    {
        Unknown = 0,
        Left = SDL_BUTTON_LEFT,
        Middle = SDL_BUTTON_MIDDLE,
        Right = SDL_BUTTON_RIGHT,
        X1 = SDL_BUTTON_X1,
        X2 = SDL_BUTTON_X2,
        END
    };

    // constants
    constexpr int32 MOUSE_COUNT = static_cast<int32>(Mouse::END);
}