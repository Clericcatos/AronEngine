#pragma once
#include "../Component.h"
#include "../../Utils/Vector2.h"
#include <vector>
#include <memory>

namespace AronEngine
{
    class UIElement;

    enum class CanvasRenderMode
    {
        ScreenSpaceOverlay,     // UI renders on top of everything
        ScreenSpaceCamera,      // UI renders relative to camera
        WorldSpace              // UI exists in 3D world
    };

    class Canvas : public Component
    {
    private:
        CanvasRenderMode renderMode;
        int sortingOrder;
        Vector2 referenceResolution;
        float scaleFactor;
        
        // Child UI elements
        std::vector<std::weak_ptr<UIElement>> uiElements;

    public:
        Canvas();
        virtual ~Canvas() override;

        // Component lifecycle
        void Start() override;
        void Update(float deltaTime) override;
        void OnDestroy() override;

        // Properties
        void SetRenderMode(CanvasRenderMode mode) { renderMode = mode; }
        CanvasRenderMode GetRenderMode() const { return renderMode; }

        void SetSortingOrder(int order) { sortingOrder = order; }
        int GetSortingOrder() const { return sortingOrder; }

        void SetReferenceResolution(const Vector2& resolution) { referenceResolution = resolution; }
        Vector2 GetReferenceResolution() const { return referenceResolution; }

        float GetScaleFactor() const { return scaleFactor; }

        // UI Element management
        void AddUIElement(std::shared_ptr<UIElement> element);
        void RemoveUIElement(std::shared_ptr<UIElement> element);
        std::vector<std::shared_ptr<UIElement>> GetUIElements() const;

        // Screen space conversion
        Vector2 WorldToScreenPoint(const Vector2& worldPos) const;
        Vector2 ScreenToWorldPoint(const Vector2& screenPos) const;

    private:
        void UpdateScaleFactor();
    };
}