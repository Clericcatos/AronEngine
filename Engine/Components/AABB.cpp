#include "framework.h"
#include "AABB.h"
#include "../Core/GameObject.h"
#include "Transform.h"

namespace AronEngine
{
    AABB::AABB()
        : Component("AABB")
        , offset(0, 0)
        , size(100, 100)
        , isTrigger(false)
    {
    }

    void AABB::Start()
    {
        Component::Start();
        UpdateBounds();
    }

    void AABB::Update(float deltaTime)
    {
        Component::Update(deltaTime);
        UpdateBounds();
    }

    void AABB::SetSize(const Vector2& newSize)
    {
        size = newSize;
        UpdateBounds();
    }

    void AABB::SetOffset(const Vector2& newOffset)
    {
        offset = newOffset;
        UpdateBounds();
    }

    bool AABB::Overlaps(const AABB& other) const
    {
        return bounds.Overlaps(other.bounds);
    }

    bool AABB::Contains(const Vector2& point) const
    {
        return bounds.Contains(point);
    }

    bool AABB::IsInCameraBounds(const BoundingBox& cameraBounds) const
    {
        return bounds.Overlaps(cameraBounds);
    }

    void AABB::UpdateBounds()
    {
        if (!gameObject) return;

        auto transform = gameObject->GetComponent<Transform>();
        if (!transform) return;

        Vector2 worldPos = transform->GetWorldPosition();
        Vector2 scale = transform->GetScale();
        
        Vector2 scaledSize = Vector2(size.x * scale.x, size.y * scale.y);
        Vector2 finalPos = worldPos + offset;

        bounds = BoundingBox(
            finalPos.x, 
            finalPos.y, 
            scaledSize.x * 0.5f, 
            scaledSize.y * 0.5f
        );
    }
}