#pragma once
#include "Component.h"
#include "../Utils/Vector2.h"
#include "../Utils/Color.h"

namespace AronEngine
{
    enum class ColliderType
    {
        Box,
        Circle,
        Polygon
    };

    struct CollisionInfo
    {
        bool isColliding;
        Vector2 normal;
        float penetrationDepth;
        Vector2 contactPoint;
        
        CollisionInfo() : isColliding(false), penetrationDepth(0.0f) {}
    };

    class Collider : public Component
    {
    protected:
        Vector2 offset;
        bool isTrigger;
        bool isEnabled;
        ColliderType type;
        
    public:
        Collider(const std::string& name, ColliderType colliderType);
        virtual ~Collider() = default;

        virtual void Start() override;
        virtual void Update(float deltaTime) override;
        virtual void OnDestroy() override;

        void SetOffset(const Vector2& newOffset) { offset = newOffset; }
        const Vector2& GetOffset() const { return offset; }
        
        void SetTrigger(bool trigger) { isTrigger = trigger; }
        bool IsTrigger() const { return isTrigger; }
        
        void SetEnabled(bool enabled) { isEnabled = enabled; }
        bool IsEnabled() const { return isEnabled; }
        
        ColliderType GetColliderType() const { return type; }
        
        virtual Vector2 GetWorldCenter() const;
        virtual bool ContainsPoint(const Vector2& worldPoint) const = 0;
        virtual CollisionInfo CheckCollision(const Collider* other) const = 0;
        virtual void DrawDebug(class Renderer* renderer, const Color& color = Color::Green) const = 0;
        
    protected:
        Vector2 GetWorldPosition() const;
    };

    class CircleCollider;

    class BoxCollider : public Collider
    {
        friend class CircleCollider;
        
    private:
        Vector2 size;
        
    public:
        BoxCollider();
        virtual ~BoxCollider() = default;

        void SetSize(const Vector2& newSize) { size = newSize; }
        const Vector2& GetSize() const { return size; }
        
        Vector2 GetWorldSize() const;
        
        virtual bool ContainsPoint(const Vector2& worldPoint) const override;
        virtual CollisionInfo CheckCollision(const Collider* other) const override;
        virtual void DrawDebug(class Renderer* renderer, const Color& color = Color::Green) const override;
        
    private:
        CollisionInfo CheckCollisionWithBox(const BoxCollider* other) const;
        CollisionInfo CheckCollisionWithCircle(const class CircleCollider* other) const;
    };

    class CircleCollider : public Collider
    {
        friend class BoxCollider;
        
    private:
        float radius;
        
    public:
        CircleCollider();
        virtual ~CircleCollider() = default;

        void SetRadius(float newRadius) { radius = (std::max)(0.0f, newRadius); }
        float GetRadius() const { return radius; }
        
        float GetWorldRadius() const;
        
        virtual bool ContainsPoint(const Vector2& worldPoint) const override;
        virtual CollisionInfo CheckCollision(const Collider* other) const override;
        virtual void DrawDebug(class Renderer* renderer, const Color& color = Color::Green) const override;
        
    private:
        CollisionInfo CheckCollisionWithBox(const BoxCollider* other) const;
        CollisionInfo CheckCollisionWithCircle(const CircleCollider* other) const;
    };
}