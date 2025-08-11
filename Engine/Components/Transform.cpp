#include "framework.h"
#include "Transform.h"
#include "../Core/GameObject.h"

namespace AronEngine
{
    Transform::Transform()
        : Component()
        , localPosition(Vector2::Zero)
        , localRotation(0.0f)
        , localScale(Vector2::One)
        , parent(nullptr)
        , matrixDirty(true)
    {
        SetName("Transform");
    }

    void Transform::SetLocalPosition(const Vector2& position)
    {
        if (localPosition != position)
        {
            localPosition = position;
            MarkMatrixDirty();
        }
    }

    void Transform::SetLocalRotation(float rotation)
    {
        if (localRotation != rotation)
        {
            localRotation = rotation;
            MarkMatrixDirty();
        }
    }

    void Transform::SetLocalScale(const Vector2& scale)
    {
        if (localScale != scale)
        {
            localScale = scale;
            MarkMatrixDirty();
        }
    }

    Vector2 Transform::GetPosition() const
    {
        if (parent)
        {
            return parent->GetLocalToWorldMatrix().TransformPoint(localPosition);
        }
        return localPosition;
    }

    void Transform::SetPosition(const Vector2& position)
    {
        if (parent)
        {
            Vector2 localPos = parent->GetWorldToLocalMatrix().TransformPoint(position);
            SetLocalPosition(localPos);
        }
        else
        {
            SetLocalPosition(position);
        }
    }

    float Transform::GetRotation() const
    {
        if (parent)
        {
            return parent->GetRotation() + localRotation;
        }
        return localRotation;
    }

    void Transform::SetRotation(float rotation)
    {
        if (parent)
        {
            SetLocalRotation(rotation - parent->GetRotation());
        }
        else
        {
            SetLocalRotation(rotation);
        }
    }

    Vector2 Transform::GetLossyScale() const
    {
        if (parent)
        {
            Vector2 parentScale = parent->GetLossyScale();
            return Vector2(localScale.x * parentScale.x, localScale.y * parentScale.y);
        }
        return localScale;
    }

    void Transform::SetParent(Transform* newParent)
    {
        if (parent == newParent)
            return;

        Vector2 worldPos = GetPosition();
        float worldRot = GetRotation();
        Vector2 worldScale = GetLossyScale();

        if (parent)
        {
            parent->RemoveChild(this);
        }

        parent = newParent;

        if (parent)
        {
            parent->AddChild(this);
            
            SetPosition(worldPos);
            SetRotation(worldRot);
        }
        else
        {
            SetLocalPosition(worldPos);
            SetLocalRotation(worldRot);
        }

        MarkMatrixDirty();
    }

    Transform* Transform::GetChild(int index) const
    {
        if (index >= 0 && index < static_cast<int>(children.size()))
        {
            return children[index];
        }
        return nullptr;
    }

    const Matrix3x2& Transform::GetLocalToWorldMatrix() const
    {
        if (matrixDirty)
        {
            UpdateMatrices();
        }
        return localToWorldMatrix;
    }

    const Matrix3x2& Transform::GetWorldToLocalMatrix() const
    {
        if (matrixDirty)
        {
            UpdateMatrices();
        }
        return worldToLocalMatrix;
    }

    void Transform::Translate(const Vector2& translation)
    {
        SetLocalPosition(localPosition + translation);
    }

    void Transform::Rotate(float deltaRotation)
    {
        SetLocalRotation(localRotation + deltaRotation);
    }

    void Transform::Scale(const Vector2& scale)
    {
        SetLocalScale(Vector2(localScale.x * scale.x, localScale.y * scale.y));
    }

    Vector2 Transform::TransformPoint(const Vector2& point) const
    {
        return GetLocalToWorldMatrix().TransformPoint(point);
    }

    Vector2 Transform::TransformVector(const Vector2& vector) const
    {
        return GetLocalToWorldMatrix().TransformVector(vector);
    }

    Vector2 Transform::InverseTransformPoint(const Vector2& point) const
    {
        return GetWorldToLocalMatrix().TransformPoint(point);
    }

    Vector2 Transform::InverseTransformVector(const Vector2& vector) const
    {
        return GetWorldToLocalMatrix().TransformVector(vector);
    }

    Vector2 Transform::GetRight() const
    {
        return Vector2::Rotate(Vector2::Right, localRotation);
    }

    Vector2 Transform::GetUp() const
    {
        return Vector2::Rotate(Vector2::Up, localRotation);
    }

    Vector2 Transform::GetForward() const
    {
        return GetUp();
    }

    void Transform::LookAt(const Vector2& target)
    {
        Vector2 direction = (target - GetPosition()).Normalized();
        float angle = Vector2::Angle(Vector2::Up, direction);
        if (direction.x < 0)
            angle = -angle;
        SetRotation(angle);
    }

    void Transform::MarkMatrixDirty()
    {
        matrixDirty = true;
        
        for (auto* child : children)
        {
            child->MarkMatrixDirty();
        }
    }

    void Transform::UpdateMatrices() const
    {
        Matrix3x2 local = Matrix3x2::TRS(localPosition, localRotation, localScale);
        
        if (parent)
        {
            localToWorldMatrix = parent->GetLocalToWorldMatrix() * local;
        }
        else
        {
            localToWorldMatrix = local;
        }
        
        worldToLocalMatrix = localToWorldMatrix.Inverse();
        matrixDirty = false;
    }

    void Transform::AddChild(Transform* child)
    {
        if (!child || child == this)
            return;

        auto it = std::find(children.begin(), children.end(), child);
        if (it == children.end())
        {
            children.push_back(child);
        }
    }

    void Transform::RemoveChild(Transform* child)
    {
        children.erase(
            std::remove(children.begin(), children.end(), child),
            children.end()
        );
    }
}