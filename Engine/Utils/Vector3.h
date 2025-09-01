#pragma once
#include <DirectXMath.h>

namespace AronEngine
{
    struct Vector3
    {
        float x, y, z;

        Vector3() : x(0), y(0), z(0) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

        Vector3 operator+(const Vector3& other) const { return {x + other.x, y + other.y, z + other.z}; }
        Vector3 operator-(const Vector3& other) const { return {x - other.x, y - other.y, z - other.z}; }
        Vector3 operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }

        DirectX::XMVECTOR ToXM() const { return DirectX::XMVectorSet(x, y, z, 0.0f); }
        static Vector3 FromXM(DirectX::FXMVECTOR v) 
        { 
            DirectX::XMFLOAT3 f;
            DirectX::XMStoreFloat3(&f, v);
            return {f.x, f.y, f.z};
        }

        static const Vector3 Zero;
        static const Vector3 One;
        static const Vector3 Up;
        static const Vector3 Forward;
        static const Vector3 Right;
    };
}