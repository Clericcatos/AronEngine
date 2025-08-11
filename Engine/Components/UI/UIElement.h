#pragma once
#include "../Component.h"
#include "../../Utils/Vector2.h"
#include "../../Utils/Color.h"

namespace AronEngine
{
    class Canvas;

    enum class AnchorType
    {
        TopLeft, TopCenter, TopRight,
        MiddleLeft, MiddleCenter, MiddleRight,
        BottomLeft, BottomCenter, BottomRight,
        Stretch, Custom
    };

    class UIElement : public Component
    {
    protected:
        Canvas* parentCanvas;
        Vector2 anchoredPosition;
        Vector2 sizeDelta;
        AnchorType anchor;
        Vector2 pivot;          // 0,0 = bottom-left, 1,1 = top-right
        bool interactable;
        bool visible;
        float alpha;

    public:
        UIElement();
        virtual ~UIElement() override;

        // Component lifecycle
        void Start() override;
        void Update(float deltaTime) override;
        void OnDestroy() override;

        // Virtual methods for derived classes
        virtual void OnRender() = 0;
        virtual void OnClick() {}
        virtual void OnHover() {}
        virtual void OnUnhover() {}

        // Properties
        void SetAnchoredPosition(const Vector2& pos) { anchoredPosition = pos; }
        Vector2 GetAnchoredPosition() const { return anchoredPosition; }

        void SetSizeDelta(const Vector2& size) { sizeDelta = size; }
        Vector2 GetSizeDelta() const { return sizeDelta; }

        void SetAnchor(AnchorType anchorType) { anchor = anchorType; }
        AnchorType GetAnchor() const { return anchor; }

        void SetPivot(const Vector2& p) { pivot = p; }
        Vector2 GetPivot() const { return pivot; }

        void SetInteractable(bool interactive) { interactable = interactive; }
        bool GetInteractable() const { return interactable; }

        void SetVisible(bool vis) { visible = vis; }
        bool GetVisible() const { return visible; }

        void SetAlpha(float a) { alpha = std::max(0.0f, std::min(1.0f, a)); }
        float GetAlpha() const { return alpha; }

        // Position calculations
        Vector2 GetWorldPosition() const;
        Vector2 GetScreenPosition() const;
        bool IsPointInside(const Vector2& point) const;

    protected:
        Vector2 GetAnchorPosition() const;
        Canvas* GetCanvas() const { return parentCanvas; }
    };
}