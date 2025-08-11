#pragma once
#include "UIElement.h"
#include "../../Utils/Color.h"
#include <string>
#include <functional>

namespace AronEngine
{
    enum class ButtonState
    {
        Normal,
        Highlighted,
        Pressed,
        Disabled
    };

    class UIButton : public UIElement
    {
    private:
        std::wstring text;
        std::wstring fontName;
        float fontSize;
        
        // Visual states
        Color normalColor;
        Color highlightedColor;
        Color pressedColor;
        Color disabledColor;
        Color textColor;
        
        ButtonState currentState;
        bool isPressed;
        bool isHovered;
        
        // Event callback
        std::function<void()> onClickCallback;

    public:
        UIButton();
        virtual ~UIButton() override;

        // UIElement overrides
        void OnRender() override;
        void OnClick() override;
        void OnHover() override;
        void OnUnhover() override;

        // Properties
        void SetText(const std::wstring& buttonText) { text = buttonText; }
        const std::wstring& GetText() const { return text; }

        void SetFont(const std::wstring& font, float size) 
        { 
            fontName = font; 
            fontSize = size; 
        }

        // Color properties
        void SetNormalColor(const Color& color) { normalColor = color; }
        void SetHighlightedColor(const Color& color) { highlightedColor = color; }
        void SetPressedColor(const Color& color) { pressedColor = color; }
        void SetDisabledColor(const Color& color) { disabledColor = color; }
        void SetTextColor(const Color& color) { textColor = color; }

        // Event handling
        void SetOnClick(std::function<void()> callback) { onClickCallback = callback; }

        // State
        ButtonState GetState() const { return currentState; }
        void SetEnabled(bool enabled);

    private:
        Color GetCurrentColor() const;
        void UpdateState();
    };
}