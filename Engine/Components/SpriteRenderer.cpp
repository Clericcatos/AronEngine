#include "framework.h"
#include "SpriteRenderer.h"
#include "../Core/GameObject.h"
#include "../Core/TextureManager.h"
#include "Transform.h"

namespace AronEngine
{
    SpriteRenderer::SpriteRenderer()
        : Component("SpriteRenderer")
        , sprite(nullptr)
        , color(Color::White)
        , sortingOrder(0)
        , sortingLayerName("Default")
        , flipX(false)
        , flipY(false)
        , size(100, 100)
        , offset(0, 0)
    {
    }

    void SpriteRenderer::SetSprite(std::shared_ptr<Sprite> newSprite)
    {
        sprite = newSprite;
        if (sprite && sprite->texture)
        {
            size = sprite->size;
        }
    }

    bool SpriteRenderer::LoadSpriteFromFile(const std::string& filepath)
    {
        auto texture = TextureManager::GetInstance().LoadTexture(filepath);
        if (!texture) {
            DEBUG_LOG("Failed to load texture for sprite: " + filepath);
            return false;
        }

        sprite = std::make_shared<Sprite>(texture, filepath);
        if (sprite) {
            size = sprite->size;
            DEBUG_LOG("Sprite loaded successfully from: " + filepath);
            return true;
        }

        return false;
    }

    void SpriteRenderer::Start()
    {
        Component::Start();
    }

    void SpriteRenderer::Update(float deltaTime)
    {
        Component::Update(deltaTime);
    }

    void SpriteRenderer::OnDestroy()
    {
        sprite = nullptr;
        Component::OnDestroy();
    }

    D2D1_RECT_F SpriteRenderer::GetRenderRect() const
    {
        if (!gameObject || !sprite)
            return D2D1::RectF(0, 0, 0, 0);

        Transform* transform = gameObject->GetComponent<Transform>();
        if (!transform)
            return D2D1::RectF(0, 0, 0, 0);

        Vector2 worldPos = transform->GetWorldPosition();
        Vector2 scale = transform->GetScale();
        
        Vector2 adjustedSize = size;
        adjustedSize.x *= scale.x;
        adjustedSize.y *= scale.y;
        
        Vector2 pivotOffset = Vector2(
            adjustedSize.x * sprite->pivot.x,
            adjustedSize.y * sprite->pivot.y
        );
        
        Vector2 finalPos = worldPos + offset - pivotOffset;
        
        return D2D1::RectF(
            finalPos.x,
            finalPos.y,
            finalPos.x + adjustedSize.x,
            finalPos.y + adjustedSize.y
        );
    }

    D2D1_MATRIX_3X2_F SpriteRenderer::GetTransformMatrix() const
    {
        if (!gameObject)
            return D2D1::Matrix3x2F::Identity();

        Transform* transform = gameObject->GetComponent<Transform>();
        if (!transform)
            return D2D1::Matrix3x2F::Identity();

        Vector2 worldPos = transform->GetWorldPosition();
        Vector2 scale = transform->GetScale();
        float rotation = transform->GetRotation();
        
        D2D1_MATRIX_3X2_F scaleMatrix = D2D1::Matrix3x2F::Scale(
            flipX ? -scale.x : scale.x,
            flipY ? -scale.y : scale.y
        );
        
        D2D1_MATRIX_3X2_F rotationMatrix = D2D1::Matrix3x2F::Rotation(rotation);
        D2D1_MATRIX_3X2_F translationMatrix = D2D1::Matrix3x2F::Translation(worldPos.x + offset.x, worldPos.y + offset.y);
        
        return scaleMatrix * rotationMatrix * translationMatrix;
    }
}