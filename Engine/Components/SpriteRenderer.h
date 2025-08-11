#pragma once
#include "Component.h"
#include "../Utils/Color.h"
#include "../Utils/Vector2.h"
#include "../Core/TextureManager.h"
#include <memory>
#include <string>

namespace AronEngine
{
    struct Sprite
    {
        std::shared_ptr<Texture> texture;
        Vector2 pivot;
        Vector2 size;
        D2D1_RECT_F textureRect;
        std::string texturePath;
        
        Sprite() : pivot(0.5f, 0.5f), size(100, 100) 
        {
            textureRect = D2D1::RectF(0, 0, 1, 1);
        }
        
        Sprite(std::shared_ptr<Texture> tex, const std::string& path = "") 
            : texture(tex), pivot(0.5f, 0.5f), texturePath(path)
        {
            if (texture)
            {
                size = Vector2(static_cast<float>(texture->size.width), static_cast<float>(texture->size.height));
                textureRect = D2D1::RectF(0, 0, static_cast<float>(texture->size.width), static_cast<float>(texture->size.height));
            }
            else
            {
                size = Vector2(100, 100);
                textureRect = D2D1::RectF(0, 0, 100, 100);
            }
        }
    };

    class SpriteRenderer : public Component
    {
    private:
        std::shared_ptr<Sprite> sprite;
        Color color;
        int sortingOrder;
        std::string sortingLayerName;
        bool flipX;
        bool flipY;
        Vector2 size;
        Vector2 offset;

    public:
        SpriteRenderer();
        virtual ~SpriteRenderer() = default;

        // 스프라이트 설정
        void SetSprite(std::shared_ptr<Sprite> newSprite);
        std::shared_ptr<Sprite> GetSprite() const { return sprite; }
        
        // 텍스처 파일에서 스프라이트 로드
        bool LoadSpriteFromFile(const std::string& filepath);
        
        // 현재 사용 중인 텍스처 파일 경로
        std::string GetTexturePath() const { return sprite ? sprite->texturePath : ""; }
        
        void SetColor(const Color& newColor) { color = newColor; }
        const Color& GetColor() const { return color; }
        
        void SetSortingOrder(int order) { sortingOrder = order; }
        int GetSortingOrder() const { return sortingOrder; }
        
        void SetSortingLayer(const std::string& layerName) { sortingLayerName = layerName; }
        const std::string& GetSortingLayer() const { return sortingLayerName; }
        
        void SetFlipX(bool flip) { flipX = flip; }
        bool GetFlipX() const { return flipX; }
        
        void SetFlipY(bool flip) { flipY = flip; }
        bool GetFlipY() const { return flipY; }
        
        void SetSize(const Vector2& newSize) { size = newSize; }
        const Vector2& GetSize() const { return size; }
        
        void SetOffset(const Vector2& newOffset) { offset = newOffset; }
        const Vector2& GetOffset() const { return offset; }

        virtual void Start() override;
        virtual void Update(float deltaTime) override;
        virtual void OnDestroy() override;
        
        D2D1_RECT_F GetRenderRect() const;
        D2D1_MATRIX_3X2_F GetTransformMatrix() const;
    };
}