#pragma once
#include "Component.h"
#include "../Utils/Vector2.h"
#include "../Utils/Matrix3x2.h"

namespace AronEngine
{
    class Transform : public Component
    {
        DECLARE_OBJECT_TYPE(Transform)

    private:
        Vector2 localPosition;
        float localRotation;
        Vector2 localScale;
        
        Transform* parent;
        std::vector<Transform*> children;
        
        mutable Matrix3x2 localToWorldMatrix;
        mutable Matrix3x2 worldToLocalMatrix;
        mutable bool matrixDirty;

    public:
        Transform();
        virtual ~Transform() = default;

        // Local space properties
        const Vector2& GetLocalPosition() const { return localPosition; }
        void SetLocalPosition(const Vector2& position);
        void SetLocalPosition(float x, float y) { SetLocalPosition(Vector2(x, y)); }

        float GetLocalRotation() const { return localRotation; }
        void SetLocalRotation(float rotation);

        const Vector2& GetLocalScale() const { return localScale; }
        void SetLocalScale(const Vector2& scale);
        void SetLocalScale(float scale) { SetLocalScale(Vector2(scale, scale)); }

        // World space properties
        Vector2 GetPosition() const;
        Vector2 GetWorldPosition() const { return GetPosition(); }
        void SetPosition(const Vector2& position);
        void SetPosition(float x, float y) { SetPosition(Vector2(x, y)); }

        float GetRotation() const;
        void SetRotation(float rotation);

        Vector2 GetLossyScale() const;
        Vector2 GetScale() const { return GetLossyScale(); }

        // Hierarchy
        Transform* GetParent() const { return parent; }
        void SetParent(Transform* parent);
        
        const std::vector<Transform*>& GetChildren() const { return children; }
        Transform* GetChild(int index) const;
        int GetChildCount() const { return static_cast<int>(children.size()); }

        // Transformation matrices
        const Matrix3x2& GetLocalToWorldMatrix() const;
        const Matrix3x2& GetWorldToLocalMatrix() const;

        // Utility methods
        void Translate(const Vector2& translation);
        void Rotate(float deltaRotation);
        void Scale(const Vector2& scale);

        Vector2 TransformPoint(const Vector2& point) const;
        Vector2 TransformVector(const Vector2& vector) const;
        Vector2 InverseTransformPoint(const Vector2& point) const;
        Vector2 InverseTransformVector(const Vector2& vector) const;

        // Direction vectors
        Vector2 GetRight() const;
        Vector2 GetUp() const;
        Vector2 GetForward() const;

        void LookAt(const Vector2& target);

    private:
        void MarkMatrixDirty();
        void UpdateMatrices() const;
        void AddChild(Transform* child);
        void RemoveChild(Transform* child);
    };
}