#include "framework.h"
#include "Camera3D.h"

namespace AronEngine
{
    Camera3D::Camera3D()
        : Component("Camera3D")
        , position{0.0f, 0.0f, -10.0f}
        , target{0.0f, 0.0f, 0.0f}
        , up{0.0f, 1.0f, 0.0f}
        , fov(DirectX::XM_PIDIV4)
        , aspectRatio(16.0f / 9.0f)
        , nearPlane(0.1f)
        , farPlane(1000.0f)
        , isDirty(true)
    {
        viewMatrix = DirectX::XMMatrixIdentity();
        projectionMatrix = DirectX::XMMatrixIdentity();
    }

    Camera3D::~Camera3D()
    {
    }

    void Camera3D::Update(float deltaTime)
    {
        if (isDirty)
        {
            UpdateMatrices();
            isDirty = false;
        }
    }

    const DirectX::XMMATRIX& Camera3D::GetViewMatrix()
    {
        if (isDirty)
        {
            UpdateMatrices();
            isDirty = false;
        }
        return viewMatrix;
    }

    const DirectX::XMMATRIX& Camera3D::GetProjectionMatrix()
    {
        if (isDirty)
        {
            UpdateMatrices();
            isDirty = false;
        }
        return projectionMatrix;
    }

    void Camera3D::LookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
    {
        position = eye;
        this->target = target;
        this->up = up;
        isDirty = true;
    }

    void Camera3D::UpdateMatrices()
    {
        DirectX::XMVECTOR eyeVec = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);
        
        viewMatrix = DirectX::XMMatrixLookAtLH(eyeVec, targetVec, upVec);
        projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
    }
}