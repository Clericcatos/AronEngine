#include "framework.h"
#include "Canvas.h"
#include "UIElement.h"
#include "../../Core/Renderer.h"
#include "../../Core/EngineApp.h"

namespace AronEngine
{
    Canvas::Canvas()
        : Component()
        , renderMode(CanvasRenderMode::ScreenSpaceOverlay)
        , sortingOrder(0)
        , referenceResolution(1920, 1080)
        , scaleFactor(1.0f)
    {
    }

    Canvas::~Canvas()
    {
        uiElements.clear();
    }

    void Canvas::Start()
    {
        Component::Start();
        UpdateScaleFactor();
        DEBUG_LOG("Canvas started with render mode: " + std::to_string((int)renderMode));
    }

    void Canvas::Update(float deltaTime)
    {
        Component::Update(deltaTime);
        
        // Update scale factor if screen resolution changed
        UpdateScaleFactor();
        
        // Clean up expired UI elements
        uiElements.erase(
            std::remove_if(uiElements.begin(), uiElements.end(),
                [](const std::weak_ptr<UIElement>& weak_elem) {
                    return weak_elem.expired();
                }),
            uiElements.end()
        );
    }

    void Canvas::OnDestroy()
    {
        uiElements.clear();
        Component::OnDestroy();
    }

    void Canvas::AddUIElement(std::shared_ptr<UIElement> element)
    {
        if (element)
        {
            uiElements.push_back(element);
            DEBUG_LOG("UI Element added to Canvas");
        }
    }

    void Canvas::RemoveUIElement(std::shared_ptr<UIElement> element)
    {
        if (!element) return;

        uiElements.erase(
            std::remove_if(uiElements.begin(), uiElements.end(),
                [element](const std::weak_ptr<UIElement>& weak_elem) {
                    auto shared_elem = weak_elem.lock();
                    return shared_elem == element;
                }),
            uiElements.end()
        );
        
        DEBUG_LOG("UI Element removed from Canvas");
    }

    std::vector<std::shared_ptr<UIElement>> Canvas::GetUIElements() const
    {
        std::vector<std::shared_ptr<UIElement>> result;
        
        for (const auto& weak_elem : uiElements)
        {
            if (auto elem = weak_elem.lock())
            {
                result.push_back(elem);
            }
        }
        
        return result;
    }

    Vector2 Canvas::WorldToScreenPoint(const Vector2& worldPos) const
    {
        // For 2D, world position is essentially screen position
        return worldPos * scaleFactor;
    }

    Vector2 Canvas::ScreenToWorldPoint(const Vector2& screenPos) const
    {
        // Convert screen coordinates to world coordinates
        return screenPos / scaleFactor;
    }

    void Canvas::UpdateScaleFactor()
    {
        // Get current screen resolution from renderer
        auto renderer = EngineApp::GetInstance().GetRenderer();
        if (!renderer) return;

        float screenWidth = (float)renderer->GetWidth();
        float screenHeight = (float)renderer->GetHeight();
        
        // Calculate scale factor based on reference resolution
        float scaleX = screenWidth / referenceResolution.x;
        float scaleY = screenHeight / referenceResolution.y;
        
        // Use the smaller scale to maintain aspect ratio
        scaleFactor = std::min(scaleX, scaleY);
    }
}