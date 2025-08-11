#pragma once
#include "../Core/framework.h"
#include <vector>
#include <memory>

namespace AronEngine
{
    class Renderer;
    class SpriteRenderer;
    class GameObject;

    class RenderSystem
    {
    private:
        std::vector<SpriteRenderer*> spriteRenderers;

    public:
        RenderSystem() = default;
        ~RenderSystem() = default;

        void Update(float deltaTime);
        void Render(Renderer* renderer);
        
        void RegisterSpriteRenderer(SpriteRenderer* spriteRenderer);
        void UnregisterSpriteRenderer(SpriteRenderer* spriteRenderer);
        
    private:
        void RenderSprites(Renderer* renderer);
        void SortRenderables();
    };
}