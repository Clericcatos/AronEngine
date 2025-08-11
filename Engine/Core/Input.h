#pragma once
#include "framework.h"
#include "../Utils/Vector2.h"

namespace AronEngine
{
    enum class KeyCode
    {
        None = 0,
        
        // Mouse buttons
        MouseLeft = VK_LBUTTON,
        MouseRight = VK_RBUTTON,
        MouseMiddle = VK_MBUTTON,
        
        // Keyboard keys
        A = 'A', B = 'B', C = 'C', D = 'D', E = 'E', F = 'F', G = 'G', H = 'H',
        I = 'I', J = 'J', K = 'K', L = 'L', M = 'M', N = 'N', O = 'O', P = 'P',
        Q = 'Q', R = 'R', S = 'S', T = 'T', U = 'U', V = 'V', W = 'W', X = 'X',
        Y = 'Y', Z = 'Z',
        
        Alpha0 = '0', Alpha1 = '1', Alpha2 = '2', Alpha3 = '3', Alpha4 = '4',
        Alpha5 = '5', Alpha6 = '6', Alpha7 = '7', Alpha8 = '8', Alpha9 = '9',
        
        Space = VK_SPACE,
        Enter = VK_RETURN,
        Escape = VK_ESCAPE,
        Tab = VK_TAB,
        Backspace = VK_BACK,
        Delete = VK_DELETE,
        
        LeftShift = VK_LSHIFT,
        RightShift = VK_RSHIFT,
        LeftControl = VK_LCONTROL,
        RightControl = VK_RCONTROL,
        LeftAlt = VK_LMENU,
        RightAlt = VK_RMENU,
        
        Up = VK_UP,
        Down = VK_DOWN,
        Left = VK_LEFT,
        Right = VK_RIGHT,
        
        F1 = VK_F1, F2 = VK_F2, F3 = VK_F3, F4 = VK_F4, F5 = VK_F5,
        F6 = VK_F6, F7 = VK_F7, F8 = VK_F8, F9 = VK_F9, F10 = VK_F10,
        F11 = VK_F11, F12 = VK_F12,
    };

    class Input
    {
    private:
        HWND hWnd;
        std::unordered_map<int, bool> currentKeyState;
        std::unordered_map<int, bool> previousKeyState;
        Vector2 mousePosition;
        Vector2 previousMousePosition;
        Vector2 mouseDelta;
        float mouseWheel;

    public:
        Input();
        ~Input() = default;

        bool Initialize(HWND hWnd);
        void Update();
        void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

        bool GetKey(KeyCode key) const;
        bool GetKeyDown(KeyCode key) const;
        bool GetKeyUp(KeyCode key) const;

        bool GetMouseButton(int button) const;
        bool GetMouseButtonDown(int button) const;
        bool GetMouseButtonUp(int button) const;

        Vector2 GetMousePosition() const { return mousePosition; }
        Vector2 GetMouseDelta() const { return mouseDelta; }
        float GetMouseWheel() const { return mouseWheel; }

        bool IsAnyKeyDown() const;
        bool IsAnyMouseButtonDown() const;

    private:
        void UpdateMousePosition();
    };
}