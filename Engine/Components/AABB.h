#pragma once
#include "Component.h"
#include "../Utils/Vector2.h"
#include <string>

namespace AronEngine
{
    struct BoundingBox
    {
        float minX, maxX;
        float minY, maxY;
        float centerX, centerY;
        float halfW, halfH;

        BoundingBox() : minX(0), maxX(0), minY(0), maxY(0), centerX(0), centerY(0), halfW(0), halfH(0) {}
        
        BoundingBox(float centerX, float centerY, float halfW, float halfH)
            : centerX(centerX), centerY(centerY), halfW(halfW), halfH(halfH)
        {
            UpdateBounds();
        }

        void UpdateBounds()
        {
            minX = centerX - halfW;
            maxX = centerX + halfW;
            minY = centerY - halfH;
            maxY = centerY + halfH;
        }

        bool Overlaps(const BoundingBox& other) const
        {
            return !(maxX < other.minX || minX > other.maxX ||
                     maxY < other.minY || minY > other.maxY);
        }

        bool Contains(const Vector2& point) const
        {
            return point.x >= minX && point.x <= maxX && 
                   point.y >= minY && point.y <= maxY;
        }

        Vector2 GetCenter() const { return Vector2(centerX, centerY); }
        Vector2 GetSize() const { return Vector2(halfW * 2, halfH * 2); }
    };

    class AABB : public Component
    {
    private:
        BoundingBox bounds;
        Vector2 offset;
        Vector2 size;
        bool isTrigger;

    public:
        AABB();
        virtual ~AABB() override = default;

        void Start() override;
        void Update(float deltaTime) override;

        void SetSize(const Vector2& newSize);
        Vector2 GetSize() const { return size; }

        void SetOffset(const Vector2& newOffset);
        Vector2 GetOffset() const { return offset; }

        void SetIsTrigger(bool trigger) { isTrigger = trigger; }
        bool GetIsTrigger() const { return isTrigger; }

        const BoundingBox& GetBounds() const { return bounds; }
        
        bool Overlaps(const AABB& other) const;
        bool Contains(const Vector2& point) const;

        bool IsInCameraBounds(const BoundingBox& cameraBounds) const;

    private:
        void UpdateBounds();
    };
}