#include "framework.h"
#include "UIButton.h"
#include "../../Core/Renderer.h"
#include "../../Core/EngineApp.h"
#include "../../Core/Input.h"

namespace AronEngine
{
    UIButton::UIButton()
        : UIElement()
        , text(L"Button")
        , fontName(L"Arial")
        , fontSize(16.0f)
        , normalColor(0.8f, 0.8f, 0.8f, 1.0f)
        , highlightedColor(0.9f, 0.9f, 0.9f, 1.0f)
        , pressedColor(0.6f, 0.6f, 0.6f, 1.0f)
        , disabledColor(0.5f, 0.5f, 0.5f, 0.5f)
        , textColor(Color::Black)
        , currentState(ButtonState::Normal)
        , isPressed(false)
        , isHovered(false)
        , onClickCallback(nullptr)
    {
        // Default button size
        sizeDelta = Vector2(120, 40);
    }

    UIButton::~UIButton()
    {
    }

    void UIButton::OnRender()
    {
        if (!visible) return;
        
        auto renderer = EngineApp::GetInstance().GetRenderer();
        if (!renderer) return;

        UpdateState();
        
        Vector2 screenPos = GetScreenPosition();
        Vector2 actualSize = sizeDelta;
        
        // Apply pivot offset for rendering
        Vector2 pivotOffset = Vector2(
            (pivot.x - 0.5f) * actualSize.x,
            (pivot.y - 0.5f) * actualSize.y
        );
        
        Vector2 renderPos = screenPos - actualSize * 0.5f + pivotOffset;
        
        // Draw button background
        Color bgColor = GetCurrentColor();
        bgColor.a *= alpha;
        renderer->FillRectangle(renderPos, actualSize, bgColor);
        
        // Draw button border
        Color borderColor = Color::Gray;
        borderColor.a *= alpha;
        renderer->DrawRectangle(renderPos, actualSize, borderColor, 2.0f);
        
        // Draw text
        if (!text.empty())
        {
            Vector2 textPos = screenPos; // Center of button
            Color textColorWithAlpha = textColor;
            textColorWithAlpha.a *= alpha;
            
            renderer->DrawText(text, textPos, textColorWithAlpha, fontName, fontSize);
        }
    }

    void UIButton::OnClick()
    {
        if (!interactable || currentState == ButtonState::Disabled) return;
        
        DEBUG_LOG("Button clicked: " + std::string(text.begin(), text.end()));
        
        if (onClickCallback)
        {
            onClickCallback();
        }
    }

    void UIButton::OnHover()
    {
        if (!interactable || currentState == ButtonState::Disabled) return;
        
        isHovered = true;
        DEBUG_LOG("Button hovered");
    }

    void UIButton::OnUnhover()
    {
        isHovered = false;
        DEBUG_LOG("Button unhovered");
    }

    void UIButton::SetEnabled(bool enabled)
    {
        interactable = enabled;
        if (!enabled)
        {
            currentState = ButtonState::Disabled;
            isPressed = false;
            isHovered = false;
        }
    }

    Color UIButton::GetCurrentColor() const
    {
        switch (currentState)
        {
        case ButtonState::Highlighted:
            return highlightedColor;
        case ButtonState::Pressed:
            return pressedColor;
        case ButtonState::Disabled:
            return disabledColor;
        case ButtonState::Normal:
        default:
            return normalColor;
        }
    }

    void UIButton::UpdateState()
    {
        if (!interactable)
        {
            currentState = ButtonState::Disabled;
            return;
        }

        auto input = EngineApp::GetInstance().GetInput();
        if (!input) return;
        
        Vector2 mousePos = input->GetMousePosition();
        bool mouseInside = IsPointInside(mousePos);
        bool mousePressed = input->GetMouseButtonDown(VK_LBUTTON);
        bool mouseReleased = input->GetMouseButtonUp(VK_LBUTTON);
        
        // Handle mouse interactions
        if (mouseInside)
        {
            if (!isHovered)
            {
                OnHover();
            }
            
            if (mousePressed)
            {
                isPressed = true;
                currentState = ButtonState::Pressed;
            }
            else if (mouseReleased && isPressed)
            {
                OnClick();
                isPressed = false;
                currentState = ButtonState::Highlighted;
            }
            else if (!isPressed)
            {
                currentState = ButtonState::Highlighted;
            }
        }
        else
        {
            if (isHovered)
            {
                OnUnhover();
            }
            
            if (mouseReleased)
            {
                isPressed = false;
            }
            
            currentState = ButtonState::Normal;
        }
    }
}