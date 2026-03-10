#pragma once
#include <GLFW/glfw3.h>

namespace MGF3D
{
    enum class KeyCode : int32
    {
        Unknown = 0,

        // Alphanumeric (문자 및 숫자)
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, // 숫자키 (D는 Digit/Default)

        // Function Keys (펑션키)
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25,

        // Control & Editing (제어 및 편집)
        Escape,
        Enter,
        Tab,
        Backspace,
        Insert,
        Delete,
        Pause,
        PrintScreen,
        ScrollLock,
        CapsLock,
        NumLock,
        Space,
        PageUp,
        PageDown,
        End,
        Home,
        Menu,

        // Navigation (방향키)
        Left,
        Up,
        Right,
        Down,

        // Modifier Keys (조합키)
        LeftShift,
        LeftControl,
        LeftAlt,
        LeftSuper,  // Windows 키 또는 Mac의 Command 키
        RightShift,
        RightControl,
        RightAlt,
        RightSuper,

        // Symbols (기호)
        Apostrophe,    // '
        Comma,         // ,
        Minus,         // -
        Period,        // .
        Slash,         // /
        Semicolon,     // ;
        Equal,         // =
        LeftBracket,   // [
        Backslash,     // \ 
                RightBracket,  // ]
        GraveAccent,   // ` (Tilde ~)

        // Numpad (키패드)
        Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
        Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
        NumpadDecimal,
        NumpadDivide,
        NumpadMultiply,
        NumpadSubtract,
        NumpadAdd,
        NumpadEnter,
        NumpadEqual,

        MAX
    };

    enum class MouseCode : int32
    {
        Unknown = 0,

        Left,
        Right,
        Middle,
        Button4,
        Button5,
        Button6,
        Button7,
        Button8,

        MAX
    };

    class GLFWInputMapper
    {
        MGF_DECLARE_UTILITIES(GLFWInputMapper)

    public:
        static int32 ToGLFW(KeyCode code) noexcept;
        static int32 ToGLFW(MouseCode code) noexcept;
        static KeyCode FromGLFWKey(int32 glfwKey) noexcept;
        static MouseCode FromGLFWMouse(int32 glfwButton) noexcept;
    };
}