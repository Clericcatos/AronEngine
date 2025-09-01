#pragma once
#include "../Core/framework.h"
#include "../Components/AABB.h"
#include <vector>
#include <memory>

namespace AronEngine
{
    class Renderer;
    class SpriteRenderer;
    class GameObject;
    class EffectSystem;

    struct CameraBounds
    {
        BoundingBox bounds;
        float left, right, top, bottom;
        
        CameraBounds() : left(0), right(1024), top(0), bottom(768) 
        {
            UpdateBounds();
        }
        
        CameraBounds(float l, float r, float t, float b) : left(l), right(r), top(t), bottom(b)
        {
            UpdateBounds();
        }
        
        void UpdateBounds()
        {
            float centerX = (left + right) * 0.5f;
            float centerY = (top + bottom) * 0.5f;
            float halfW = (right - left) * 0.5f;
            float halfH = (bottom - top) * 0.5f;
            bounds = BoundingBox(centerX, centerY, halfW, halfH);
        }
    };

    class RenderSystem
    {
    private:
        std::vector<SpriteRenderer*> spriteRenderers;
        std::shared_ptr<EffectSystem> effectSystem;
        CameraBounds cameraBounds;
        bool cullingEnabled;
        int culledObjectCount;
        int totalObjectCount;

    public:
        RenderSystem();
        ~RenderSystem() = default;

        void Update(float deltaTime);
        void Render(Renderer* renderer);
        
        void RegisterSpriteRenderer(SpriteRenderer* spriteRenderer);
        void UnregisterSpriteRenderer(SpriteRenderer* spriteRenderer);
        
        void SetEffectSystem(std::shared_ptr<EffectSystem> effects) { effectSystem = effects; }
        
        void SetCameraBounds(float left, float right, float top, float bottom);
        void SetCullingEnabled(bool enabled) { cullingEnabled = enabled; }
        bool IsCullingEnabled() const { return cullingEnabled; }
        
        int GetCulledObjectCount() const { return culledObjectCount; }
        int GetTotalObjectCount() const { return totalObjectCount; }
        float GetCullingRatio() const { return totalObjectCount > 0 ? (float)culledObjectCount / totalObjectCount : 0.0f; }
        
    private:
        void RenderSprites(Renderer* renderer);
        void SortRenderables();
        bool IsInCameraBounds(SpriteRenderer* spriteRenderer);
    };
}