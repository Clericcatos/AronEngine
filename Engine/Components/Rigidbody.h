#pragma once
#include "Component.h"
#include "../Utils/Vector2.h"

namespace AronEngine
{
    enum class RigidbodyType
    {
        Dynamic,    // Affected by physics
        Kinematic,  // Not affected by physics, but can move
        Static      // Never moves
    };

    class Rigidbody : public Component
    {
    private:
        Vector2 velocity;
        Vector2 acceleration;
        Vector2 force;
        
        float mass;
        float drag;
        float angularDrag;
        float angularVelocity;
        
        RigidbodyType bodyType;
        
        bool useGravity;
        bool freezeRotation;
        bool isKinematic;
        
        Vector2 gravityScale;
        
        // Physics constraints
        bool constrainX;
        bool constrainY;
        bool constrainRotation;

    public:
        Rigidbody();
        virtual ~Rigidbody() = default;

        virtual void Start() override;
        virtual void Update(float deltaTime) override;
        virtual void OnDestroy() override;

        // Velocity
        void SetVelocity(const Vector2& vel) { velocity = vel; }
        const Vector2& GetVelocity() const { return velocity; }
        void AddVelocity(const Vector2& vel) { velocity += vel; }
        
        // Forces
        void AddForce(const Vector2& forceVector);
        void AddForceAtPosition(const Vector2& forceVector, const Vector2& position);
        void AddImpulse(const Vector2& impulse);
        void AddTorque(float torque);
        
        // Mass and drag
        void SetMass(float newMass) { mass = (std::max)(0.1f, newMass); }
        float GetMass() const { return mass; }
        
        void SetDrag(float newDrag) { drag = (std::max)(0.0f, newDrag); }
        float GetDrag() const { return drag; }
        
        void SetAngularDrag(float newDrag) { angularDrag = (std::max)(0.0f, newDrag); }
        float GetAngularDrag() const { return angularDrag; }
        
        // Angular velocity
        void SetAngularVelocity(float angVel) { angularVelocity = angVel; }
        float GetAngularVelocity() const { return angularVelocity; }
        
        // Body type
        void SetBodyType(RigidbodyType type);
        RigidbodyType GetBodyType() const { return bodyType; }
        
        // Gravity
        void SetUseGravity(bool use) { useGravity = use; }
        bool GetUseGravity() const { return useGravity; }
        
        void SetGravityScale(const Vector2& scale) { gravityScale = scale; }
        const Vector2& GetGravityScale() const { return gravityScale; }
        
        // Constraints
        void SetFreezeRotation(bool freeze) { freezeRotation = freeze; constrainRotation = freeze; }
        bool GetFreezeRotation() const { return freezeRotation; }
        
        void SetConstrainX(bool constrain) { constrainX = constrain; }
        bool GetConstrainX() const { return constrainX; }
        
        void SetConstrainY(bool constrain) { constrainY = constrain; }
        bool GetConstrainY() const { return constrainY; }
        
        // Kinematic
        void SetKinematic(bool kinematic);
        bool IsKinematic() const { return isKinematic; }
        
        // Movement for kinematic bodies
        void MovePosition(const Vector2& position);
        void MoveRotation(float rotation);
        
        // Physics queries
        float GetKineticEnergy() const;
        Vector2 GetMomentum() const;
        
    private:
        void IntegrateForces(float deltaTime);
        void IntegrateVelocity(float deltaTime);
        void ApplyDrag(float deltaTime);
        void ApplyGravity(float deltaTime);
        
        static Vector2 GetGravity() { return Vector2(0, 9.81f * 50.0f); } // Scaled for pixels
    };
}