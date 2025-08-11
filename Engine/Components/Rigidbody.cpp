#include "framework.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "../Core/GameObject.h"

namespace AronEngine
{
    Rigidbody::Rigidbody()
        : Component("Rigidbody")
        , velocity(0, 0)
        , acceleration(0, 0)
        , force(0, 0)
        , mass(1.0f)
        , drag(0.0f)
        , angularDrag(0.05f)
        , angularVelocity(0.0f)
        , bodyType(RigidbodyType::Dynamic)
        , useGravity(true)
        , freezeRotation(false)
        , isKinematic(false)
        , gravityScale(1.0f, 1.0f)
        , constrainX(false)
        , constrainY(false)
        , constrainRotation(false)
    {
    }

    void Rigidbody::Start()
    {
        Component::Start();
    }

    void Rigidbody::Update(float deltaTime)
    {
        Component::Update(deltaTime);
        
        if (bodyType == RigidbodyType::Static)
            return;
            
        if (bodyType == RigidbodyType::Dynamic && !isKinematic)
        {
            ApplyGravity(deltaTime);
            IntegrateForces(deltaTime);
            ApplyDrag(deltaTime);
        }
        
        IntegrateVelocity(deltaTime);
    }

    void Rigidbody::OnDestroy()
    {
        Component::OnDestroy();
    }

    void Rigidbody::AddForce(const Vector2& forceVector)
    {
        if (bodyType == RigidbodyType::Dynamic && !isKinematic)
        {
            force += forceVector;
        }
    }

    void Rigidbody::AddForceAtPosition(const Vector2& forceVector, const Vector2& position)
    {
        AddForce(forceVector);
        
        if (!constrainRotation && !freezeRotation)
        {
            Transform* transform = gameObject ? gameObject->GetComponent<Transform>() : nullptr;
            if (transform)
            {
                Vector2 worldPos = transform->GetWorldPosition();
                Vector2 arm = position - worldPos;
                float torque = arm.Cross(forceVector);
                AddTorque(torque);
            }
        }
    }

    void Rigidbody::AddImpulse(const Vector2& impulse)
    {
        if (bodyType == RigidbodyType::Dynamic && !isKinematic)
        {
            velocity += impulse / mass;
        }
    }

    void Rigidbody::AddTorque(float torque)
    {
        if (bodyType == RigidbodyType::Dynamic && !isKinematic && !constrainRotation && !freezeRotation)
        {
            angularVelocity += torque / mass;
        }
    }

    void Rigidbody::SetBodyType(RigidbodyType type)
    {
        bodyType = type;
        
        if (type == RigidbodyType::Static)
        {
            velocity = Vector2::Zero;
            angularVelocity = 0.0f;
            force = Vector2::Zero;
            acceleration = Vector2::Zero;
        }
        else if (type == RigidbodyType::Kinematic)
        {
            isKinematic = true;
        }
        else
        {
            isKinematic = false;
        }
    }

    void Rigidbody::SetKinematic(bool kinematic)
    {
        isKinematic = kinematic;
        
        if (kinematic)
        {
            force = Vector2::Zero;
            acceleration = Vector2::Zero;
        }
    }

    void Rigidbody::MovePosition(const Vector2& position)
    {
        if (bodyType != RigidbodyType::Static)
        {
            Transform* transform = gameObject ? gameObject->GetComponent<Transform>() : nullptr;
            if (transform)
            {
                transform->SetPosition(position);
            }
        }
    }

    void Rigidbody::MoveRotation(float rotation)
    {
        if (bodyType != RigidbodyType::Static && !constrainRotation && !freezeRotation)
        {
            Transform* transform = gameObject ? gameObject->GetComponent<Transform>() : nullptr;
            if (transform)
            {
                transform->SetRotation(rotation);
            }
        }
    }

    float Rigidbody::GetKineticEnergy() const
    {
        return 0.5f * mass * velocity.LengthSquared();
    }

    Vector2 Rigidbody::GetMomentum() const
    {
        return velocity * mass;
    }

    void Rigidbody::IntegrateForces(float deltaTime)
    {
        if (mass <= 0.0f)
            return;
            
        acceleration = force / mass;
        velocity += acceleration * deltaTime;
        
        force = Vector2::Zero;
    }

    void Rigidbody::IntegrateVelocity(float deltaTime)
    {
        Transform* transform = gameObject ? gameObject->GetComponent<Transform>() : nullptr;
        if (!transform)
            return;
            
        Vector2 currentPos = transform->GetPosition();
        float currentRot = transform->GetRotation();
        
        Vector2 newVelocity = velocity;
        if (constrainX) newVelocity.x = 0;
        if (constrainY) newVelocity.y = 0;
        
        Vector2 newPos = currentPos + newVelocity * deltaTime;
        transform->SetPosition(newPos);
        
        if (!constrainRotation && !freezeRotation)
        {
            float newRot = currentRot + angularVelocity * deltaTime;
            transform->SetRotation(newRot);
        }
    }

    void Rigidbody::ApplyDrag(float deltaTime)
    {
        if (drag > 0.0f)
        {
            float dragFactor = 1.0f / (1.0f + drag * deltaTime);
            velocity *= dragFactor;
        }
        
        if (angularDrag > 0.0f && !constrainRotation && !freezeRotation)
        {
            float angularDragFactor = 1.0f / (1.0f + angularDrag * deltaTime);
            angularVelocity *= angularDragFactor;
        }
    }

    void Rigidbody::ApplyGravity(float deltaTime)
    {
        if (useGravity && bodyType == RigidbodyType::Dynamic && !isKinematic)
        {
            Vector2 gravity = GetGravity();
            gravity.x *= gravityScale.x;
            gravity.y *= gravityScale.y;
            
            velocity += gravity * deltaTime;
        }
    }
}