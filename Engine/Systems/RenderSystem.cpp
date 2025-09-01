#include "framework.h"
#include "RenderSystem.h"
#include "EffectSystem.h"
#include "../Core/Renderer.h"
#include "../Components/SpriteRenderer.h"
#include "../Components/AABB.h"
#include "../Core/GameObject.h"
#include "../Components/Transform.h"
#include <algorithm>

namespace AronEngine
{
    RenderSystem::RenderSystem()
        : cullingEnabled(false)
        , culledObjectCount(0)
        , totalObjectCount(0)
    {
    }

    void RenderSystem::Update(float deltaTime)
    {
        SortRenderables();
        
        if (effectSystem)
        {
            effectSystem->Update(deltaTime);
        }
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
        culledObjectCount = 0;
        totalObjectCount = 0;

        for (SpriteRenderer* spriteRenderer : spriteRenderers)
        {
            if (!spriteRenderer || !spriteRenderer->IsActive())
                continue;
                
            GameObject* gameObject = spriteRenderer->GetGameObject();
            if (!gameObject || !gameObject->IsActive())
                continue;
                
            totalObjectCount++;
            
            if (cullingEnabled && !IsInCameraBounds(spriteRenderer))
            {
                culledObjectCount++;
                continue;
            }
                
            std::shared_ptr<Sprite> sprite = spriteRenderer->GetSprite();
            if (!sprite || !sprite->texture)
                continue;
            
            D2D1_RECT_F renderRect = spriteRenderer->GetRenderRect();
            D2D1_MATRIX_3X2_F transformMatrix = spriteRenderer->GetTransformMatrix();
            
            renderer->SetTransform(transformMatrix);
            if (sprite->texture) {
                Vector2 pos(renderRect.left, renderRect.top);
                Vector2 size(renderRect.right - renderRect.left, renderRect.bottom - renderRect.top);
                renderer->DrawTexture(sprite->texture, pos, size, spriteRenderer->GetColor());
            }
            renderer->SetTransform(D2D1::Matrix3x2F::Identity());
        }
    }
    
    void RenderSystem::SetCameraBounds(float left, float right, float top, float bottom)
    {
        cameraBounds = CameraBounds(left, right, top, bottom);
    }

    bool RenderSystem::IsInCameraBounds(SpriteRenderer* spriteRenderer)
    {
        if (!spriteRenderer) return false;
        
        GameObject* gameObject = spriteRenderer->GetGameObject();
        if (!gameObject) return false;
        
        auto aabb = gameObject->GetComponent<AABB>();
        if (aabb)
        {
            return aabb->IsInCameraBounds(cameraBounds.bounds);
        }
        
        auto transform = gameObject->GetComponent<Transform>();
        if (transform)
        {
            Vector2 pos = transform->GetWorldPosition();
            Vector2 size = spriteRenderer->GetSize();
            
            BoundingBox objectBounds(pos.x, pos.y, size.x * 0.5f, size.y * 0.5f);
            return objectBounds.Overlaps(cameraBounds.bounds);
        }
        
        return true;
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