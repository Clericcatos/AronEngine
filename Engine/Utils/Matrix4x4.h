#pragma once
#include <DirectXMath.h>
#include "Vector3.h"

namespace AronEngine
{
    struct Matrix4x4
    {
        DirectX::XMMATRIX m;

        Matrix4x4() : m(DirectX::XMMatrixIdentity()) {}
        Matrix4x4(const DirectX::XMMATRIX& matrix) : m(matrix) {}

        static Matrix4x4 Identity() { return Matrix4x4(DirectX::XMMatrixIdentity()); }
        static Matrix4x4 Translation(const Vector3& pos) { return Matrix4x4(DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z)); }
        static Matrix4x4 Scaling(const Vector3& scale) { return Matrix4x4(DirectX::XMMatrixScaling(scale.x, scale.y, scale.z)); }
        static Matrix4x4 RotationY(float radians) { return Matrix4x4(DirectX::XMMatrixRotationY(radians)); }
        static Matrix4x4 Perspective(float fov, float aspect, float nearZ, float farZ) 
        { 
            return Matrix4x4(DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ)); 
        }
        static Matrix4x4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
        {
            return Matrix4x4(DirectX::XMMatrixLookAtLH(eye.ToXM(), target.ToXM(), up.ToXM()));
        }

        Matrix4x4 operator*(const Matrix4x4& other) const { return Matrix4x4(DirectX::XMMatrixMultiply(m, other.m)); }
        operator DirectX::XMMATRIX() const { return m; }
    };
}