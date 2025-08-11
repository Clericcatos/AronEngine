#include "framework.h"
#include "Input.h"

namespace AronEngine
{
    Input::Input()
        : hWnd(nullptr)
        , mousePosition(0, 0)
        , previousMousePosition(0, 0)
        , mouseDelta(0, 0)
        , mouseWheel(0.0f)
    {
    }

    bool Input::Initialize(HWND hWnd)
    {
        this->hWnd = hWnd;
        return true;
    }

    void Input::Update()
    {
        previousKeyState = currentKeyState;
        previousMousePosition = mousePosition;
        
        UpdateMousePosition();
        mouseDelta = mousePosition - previousMousePosition;
        mouseWheel = 0.0f;
    }

    void Input::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_KEYDOWN:
            currentKeyState[wParam] = true;
            break;
            
        case WM_KEYUP:
            currentKeyState[wParam] = false;
            break;
            
        case WM_LBUTTONDOWN:
            currentKeyState[VK_LBUTTON] = true;
            break;
            
        case WM_LBUTTONUP:
            currentKeyState[VK_LBUTTON] = false;
            break;
            
        case WM_RBUTTONDOWN:
            currentKeyState[VK_RBUTTON] = true;
            break;
            
        case WM_RBUTTONUP:
            currentKeyState[VK_RBUTTON] = false;
            break;
            
        case WM_MBUTTONDOWN:
            currentKeyState[VK_MBUTTON] = true;
            break;
            
        case WM_MBUTTONUP:
            currentKeyState[VK_MBUTTON] = false;
            break;
            
        case WM_MOUSEMOVE:
            mousePosition.x = static_cast<float>(LOWORD(lParam));
            mousePosition.y = static_cast<float>(HIWORD(lParam));
            break;
            
        case WM_MOUSEWHEEL:
            mouseWheel = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
            break;
        }
    }

    bool Input::GetKey(KeyCode key) const
    {
        auto it = currentKeyState.find(static_cast<int>(key));
        return it != currentKeyState.end() && it->second;
    }

    bool Input::GetKeyDown(KeyCode key) const
    {
        int keyInt = static_cast<int>(key);
        auto curr = currentKeyState.find(keyInt);
        auto prev = previousKeyState.find(keyInt);
        
        bool currentPressed = (curr != currentKeyState.end() && curr->second);
        bool previousPressed = (prev != previousKeyState.end() && prev->second);
        
        return currentPressed && !previousPressed;
    }

    bool Input::GetKeyUp(KeyCode key) const
    {
        int keyInt = static_cast<int>(key);
        auto curr = currentKeyState.find(keyInt);
        auto prev = previousKeyState.find(keyInt);
        
        bool currentPressed = (curr != currentKeyState.end() && curr->second);
        bool previousPressed = (prev != previousKeyState.end() && prev->second);
        
        return !currentPressed && previousPressed;
    }

    bool Input::GetMouseButton(int button) const
    {
        return GetKey(static_cast<KeyCode>(button));
    }

    bool Input::GetMouseButtonDown(int button) const
    {
        return GetKeyDown(static_cast<KeyCode>(button));
    }

    bool Input::GetMouseButtonUp(int button) const
    {
        return GetKeyUp(static_cast<KeyCode>(button));
    }

    bool Input::IsAnyKeyDown() const
    {
        for (const auto& [key, pressed] : currentKeyState)
        {
            if (pressed && key != VK_LBUTTON && key != VK_RBUTTON && key != VK_MBUTTON)
            {
                auto prev = previousKeyState.find(key);
                if (prev == previousKeyState.end() || !prev->second)
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool Input::IsAnyMouseButtonDown() const
    {
        return GetMouseButtonDown(0) || GetMouseButtonDown(1) || GetMouseButtonDown(2);
    }

    void Input::UpdateMousePosition()
    {
        POINT pt;
        if (GetCursorPos(&pt) && ScreenToClient(hWnd, &pt))
        {
            mousePosition.x = static_cast<float>(pt.x);
            mousePosition.y = static_cast<float>(pt.y);
        }
    }
}