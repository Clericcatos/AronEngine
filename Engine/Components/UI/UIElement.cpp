#include "framework.h"
#include "UIElement.h"
#include "Canvas.h"
#include "../../Core/GameObject.h"

namespace AronEngine
{
    UIElement::UIElement()
        : Component()
        , parentCanvas(nullptr)
        , anchoredPosition(0, 0)
        , sizeDelta(100, 50)
        , anchor(AnchorType::MiddleCenter)
        , pivot(0.5f, 0.5f)
        , interactable(true)
        , visible(true)
        , alpha(1.0f)
    {
    }

    UIElement::~UIElement()
    {
    }

    void UIElement::Start()
    {
        Component::Start();
        
        // Find parent canvas
        if (auto gameObject = GetGameObject())
        {
            parentCanvas = gameObject->GetComponent<Canvas>();
            if (!parentCanvas)
            {
                // Look for canvas in parent GameObjects
                // For now, just log that no canvas was found
                DEBUG_LOG("UIElement: No Canvas found in GameObject hierarchy");
            }
        }
    }

    void UIElement::Update(float deltaTime)
    {
        Component::Update(deltaTime);
        
        if (visible)
        {
            OnRender();
        }
    }

    void UIElement::OnDestroy()
    {
        if (parentCanvas)
        {
            // Remove self from canvas if possible
            // parentCanvas->RemoveUIElement(shared_from_this());
        }
        Component::OnDestroy();
    }

    Vector2 UIElement::GetWorldPosition() const
    {
        Vector2 anchorPos = GetAnchorPosition();
        return anchorPos + anchoredPosition;
    }

    Vector2 UIElement::GetScreenPosition() const
    {
        if (parentCanvas)
        {
            return parentCanvas->WorldToScreenPoint(GetWorldPosition());
        }
        return GetWorldPosition();
    }

    bool UIElement::IsPointInside(const Vector2& point) const
    {
        if (!visible || !interactable) return false;
        
        Vector2 worldPos = GetWorldPosition();
        Vector2 halfSize = sizeDelta * 0.5f;
        
        // Apply pivot offset
        Vector2 pivotOffset = Vector2(
            (pivot.x - 0.5f) * sizeDelta.x,
            (pivot.y - 0.5f) * sizeDelta.y
        );
        
        Vector2 minBounds = worldPos - halfSize + pivotOffset;
        Vector2 maxBounds = worldPos + halfSize + pivotOffset;
        
        return (point.x >= minBounds.x && point.x <= maxBounds.x &&
                point.y >= minBounds.y && point.y <= maxBounds.y);
    }

    Vector2 UIElement::GetAnchorPosition() const
    {
        if (!parentCanvas) return Vector2(0, 0);
        
        // Get screen dimensions from canvas
        Vector2 screenSize = parentCanvas->GetReferenceResolution();
        
        switch (anchor)
        {
        case AnchorType::TopLeft:
            return Vector2(0, screenSize.y);
        case AnchorType::TopCenter:
            return Vector2(screenSize.x * 0.5f, screenSize.y);
        case AnchorType::TopRight:
            return Vector2(screenSize.x, screenSize.y);
        case AnchorType::MiddleLeft:
            return Vector2(0, screenSize.y * 0.5f);
        case AnchorType::MiddleCenter:
            return Vector2(screenSize.x * 0.5f, screenSize.y * 0.5f);
        case AnchorType::MiddleRight:
            return Vector2(screenSize.x, screenSize.y * 0.5f);
        case AnchorType::BottomLeft:
            return Vector2(0, 0);
        case AnchorType::BottomCenter:
            return Vector2(screenSize.x * 0.5f, 0);
        case AnchorType::BottomRight:
            return Vector2(screenSize.x, 0);
        case AnchorType::Stretch:
        case AnchorType::Custom:
        default:
            return Vector2(screenSize.x * 0.5f, screenSize.y * 0.5f);
        }
    }
}