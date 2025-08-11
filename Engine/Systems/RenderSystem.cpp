#include "framework.h"
#include "RenderSystem.h"
#include "../Core/Renderer.h"
#include "../Components/SpriteRenderer.h"
#include "../Core/GameObject.h"
#include "../Components/Transform.h"
#include <algorithm>

namespace AronEngine
{
    void RenderSystem::Update(float deltaTime)
    {
        SortRenderables();
    }

    void RenderSystem::Render(Renderer* renderer)
    {
        if (!renderer)
            return;
            
        RenderSprites(renderer);
    }
    
    void RenderSystem::RegisterSpriteRenderer(SpriteRenderer* spriteRenderer)
    {
        if (spriteRenderer)
        {
            spriteRenderers.push_back(spriteRenderer);
        }
    }
    
    void RenderSystem::UnregisterSpriteRenderer(SpriteRenderer* spriteRenderer)
    {
        auto it = std::find(spriteRenderers.begin(), spriteRenderers.end(), spriteRenderer);
        if (it != spriteRenderers.end())
        {
            spriteRenderers.erase(it);
        }
    }
    
    void RenderSystem::RenderSprites(Renderer* renderer)
    {
        for (SpriteRenderer* spriteRenderer : spriteRenderers)
        {
            if (!spriteRenderer || !spriteRenderer->IsActive())
                continue;
                
            GameObject* gameObject = spriteRenderer->GetGameObject();
            if (!gameObject || !gameObject->IsActive())
                continue;
                
            std::shared_ptr<Sprite> sprite = spriteRenderer->GetSprite();
            if (!sprite || !sprite->texture)
                continue;
            
            D2D1_RECT_F renderRect = spriteRenderer->GetRenderRect();
            D2D1_MATRIX_3X2_F transformMatrix = spriteRenderer->GetTransformMatrix();
            
            renderer->SetTransform(transformMatrix);
            // 새로운 텍스처 시스템 사용
            if (sprite->texture) {
                Vector2 pos(renderRect.left, renderRect.top);
                Vector2 size(renderRect.right - renderRect.left, renderRect.bottom - renderRect.top);
                renderer->DrawTexture(sprite->texture, pos, size, spriteRenderer->GetColor());
            }
            renderer->SetTransform(D2D1::Matrix3x2F::Identity());
        }
    }
    
    void RenderSystem::SortRenderables()
    {
        std::sort(spriteRenderers.begin(), spriteRenderers.end(),
            [](const SpriteRenderer* a, const SpriteRenderer* b) {
                if (a->GetSortingLayer() != b->GetSortingLayer())
                {
                    return a->GetSortingLayer() < b->GetSortingLayer();
                }
                return a->GetSortingOrder() < b->GetSortingOrder();
            });
    }
}