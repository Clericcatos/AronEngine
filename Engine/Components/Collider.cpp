#include "framework.h"
#include "Collider.h"
#include "Transform.h"
#include "../Core/GameObject.h"
#include "../Core/Renderer.h"
#include <cmath>

namespace AronEngine
{
    // Base Collider implementation
    Collider::Collider(const std::string& name, ColliderType colliderType)
        : Component(name)
        , offset(0, 0)
        , isTrigger(false)
        , isEnabled(true)
        , type(colliderType)
    {
    }

    void Collider::Start()
    {
        Component::Start();
    }

    void Collider::Update(float deltaTime)
    {
        Component::Update(deltaTime);
    }

    void Collider::OnDestroy()
    {
        Component::OnDestroy();
    }

    Vector2 Collider::GetWorldCenter() const
    {
        return GetWorldPosition() + offset;
    }

    Vector2 Collider::GetWorldPosition() const
    {
        if (!gameObject)
            return Vector2::Zero;
            
        Transform* transform = gameObject->GetComponent<Transform>();
        return transform ? transform->GetWorldPosition() : Vector2::Zero;
    }

    // BoxCollider implementation
    BoxCollider::BoxCollider()
        : Collider("BoxCollider", ColliderType::Box)
        , size(1.0f, 1.0f)
    {
    }

    Vector2 BoxCollider::GetWorldSize() const
    {
        if (!gameObject)
            return size;
            
        Transform* transform = gameObject->GetComponent<Transform>();
        if (!transform)
            return size;
            
        Vector2 scale = transform->GetScale();
        return Vector2(size.x * scale.x, size.y * scale.y);
    }

    bool BoxCollider::ContainsPoint(const Vector2& worldPoint) const
    {
        if (!isEnabled)
            return false;
            
        Vector2 center = GetWorldCenter();
        Vector2 worldSize = GetWorldSize();
        Vector2 halfSize = worldSize * 0.5f;
        
        return (worldPoint.x >= center.x - halfSize.x &&
                worldPoint.x <= center.x + halfSize.x &&
                worldPoint.y >= center.y - halfSize.y &&
                worldPoint.y <= center.y + halfSize.y);
    }

    CollisionInfo BoxCollider::CheckCollision(const Collider* other) const
    {
        if (!other || !isEnabled || !other->IsEnabled())
            return CollisionInfo();
            
        switch (other->GetColliderType())
        {
            case ColliderType::Box:
                return CheckCollisionWithBox(static_cast<const BoxCollider*>(other));
            case ColliderType::Circle:
                return CheckCollisionWithCircle(static_cast<const CircleCollider*>(other));
            default:
                return CollisionInfo();
        }
    }

    CollisionInfo BoxCollider::CheckCollisionWithBox(const BoxCollider* other) const
    {
        CollisionInfo info;
        
        Vector2 centerA = GetWorldCenter();
        Vector2 centerB = other->GetWorldCenter();
        Vector2 sizeA = GetWorldSize();
        Vector2 sizeB = other->GetWorldSize();
        
        Vector2 halfSizeA = sizeA * 0.5f;
        Vector2 halfSizeB = sizeB * 0.5f;
        
        Vector2 distance = centerB - centerA;
        Vector2 absDistance = Vector2(std::abs(distance.x), std::abs(distance.y));
        
        Vector2 overlap = (halfSizeA + halfSizeB) - absDistance;
        
        if (overlap.x > 0 && overlap.y > 0)
        {
            info.isColliding = true;
            
            if (overlap.x < overlap.y)
            {
                info.normal = Vector2(distance.x > 0 ? 1.0f : -1.0f, 0.0f);
                info.penetrationDepth = overlap.x;
                info.contactPoint = centerA + Vector2(halfSizeA.x * info.normal.x, 0.0f);
            }
            else
            {
                info.normal = Vector2(0.0f, distance.y > 0 ? 1.0f : -1.0f);
                info.penetrationDepth = overlap.y;
                info.contactPoint = centerA + Vector2(0.0f, halfSizeA.y * info.normal.y);
            }
        }
        
        return info;
    }

    CollisionInfo BoxCollider::CheckCollisionWithCircle(const CircleCollider* other) const
    {
        CollisionInfo info;
        
        Vector2 boxCenter = GetWorldCenter();
        Vector2 circleCenter = other->GetWorldCenter();
        Vector2 boxSize = GetWorldSize();
        float circleRadius = other->GetWorldRadius();
        
        Vector2 halfSize = boxSize * 0.5f;
        Vector2 distance = circleCenter - boxCenter;
        
        Vector2 clamped = Vector2(
            (std::max)(-halfSize.x, (std::min)(halfSize.x, distance.x)),
            (std::max)(-halfSize.y, (std::min)(halfSize.y, distance.y))
        );
        
        Vector2 closest = boxCenter + clamped;
        distance = circleCenter - closest;
        
        float distanceSquared = distance.LengthSquared();
        float radiusSquared = circleRadius * circleRadius;
        
        if (distanceSquared < radiusSquared)
        {
            info.isColliding = true;
            float distanceLength = std::sqrt(distanceSquared);
            
            if (distanceLength > 0)
            {
                info.normal = distance / distanceLength;
                info.penetrationDepth = circleRadius - distanceLength;
                info.contactPoint = closest;
            }
            else
            {
                info.normal = Vector2(1, 0);
                info.penetrationDepth = circleRadius;
                info.contactPoint = boxCenter;
            }
        }
        
        return info;
    }

    void BoxCollider::DrawDebug(Renderer* renderer, const Color& color) const
    {
        if (!renderer || !isEnabled)
            return;
            
        Vector2 center = GetWorldCenter();
        Vector2 worldSize = GetWorldSize();
        
        renderer->DrawRectangle(center - worldSize * 0.5f, worldSize, color, 2.0f);
    }

    // CircleCollider implementation
    CircleCollider::CircleCollider()
        : Collider("CircleCollider", ColliderType::Circle)
        , radius(0.5f)
    {
    }

    float CircleCollider::GetWorldRadius() const
    {
        if (!gameObject)
            return radius;
            
        Transform* transform = gameObject->GetComponent<Transform>();
        if (!transform)
            return radius;
            
        Vector2 scale = transform->GetScale();
        return radius * (std::max)(scale.x, scale.y);
    }

    bool CircleCollider::ContainsPoint(const Vector2& worldPoint) const
    {
        if (!isEnabled)
            return false;
            
        Vector2 center = GetWorldCenter();
        float worldRadius = GetWorldRadius();
        
        return (worldPoint - center).LengthSquared() <= worldRadius * worldRadius;
    }

    CollisionInfo CircleCollider::CheckCollision(const Collider* other) const
    {
        if (!other || !isEnabled || !other->IsEnabled())
            return CollisionInfo();
            
        switch (other->GetColliderType())
        {
            case ColliderType::Box:
                return CheckCollisionWithBox(static_cast<const BoxCollider*>(other));
            case ColliderType::Circle:
                return CheckCollisionWithCircle(static_cast<const CircleCollider*>(other));
            default:
                return CollisionInfo();
        }
    }

    CollisionInfo CircleCollider::CheckCollisionWithBox(const BoxCollider* other) const
    {
        CollisionInfo info = other->CheckCollisionWithCircle(this);
        info.normal = -info.normal;
        return info;
    }

    CollisionInfo CircleCollider::CheckCollisionWithCircle(const CircleCollider* other) const
    {
        CollisionInfo info;
        
        Vector2 centerA = GetWorldCenter();
        Vector2 centerB = other->GetWorldCenter();
        float radiusA = GetWorldRadius();
        float radiusB = other->GetWorldRadius();
        
        Vector2 distance = centerB - centerA;
        float distanceLength = distance.Length();
        float totalRadius = radiusA + radiusB;
        
        if (distanceLength < totalRadius)
        {
            info.isColliding = true;
            info.penetrationDepth = totalRadius - distanceLength;
            
            if (distanceLength > 0)
            {
                info.normal = distance / distanceLength;
                info.contactPoint = centerA + info.normal * radiusA;
            }
            else
            {
                info.normal = Vector2(1, 0);
                info.contactPoint = centerA;
            }
        }
        
        return info;
    }

    void CircleCollider::DrawDebug(Renderer* renderer, const Color& color) const
    {
        if (!renderer || !isEnabled)
            return;
            
        Vector2 center = GetWorldCenter();
        float worldRadius = GetWorldRadius();
        
        renderer->DrawCircle(center, worldRadius, color, 2.0f);
    }
}