#pragma once
#include "Component.h"
#include <DirectXMath.h>

namespace AronEngine
{
    class Camera3D : public Component
    {
        DECLARE_COMPONENT_TYPE(Camera3D)

    private:
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 target;
        DirectX::XMFLOAT3 up;
        float fov;
        float aspectRatio;
        float nearPlane;
        float farPlane;
        
        DirectX::XMMATRIX viewMatrix;
        DirectX::XMMATRIX projectionMatrix;
        bool isDirty;

    public:
        Camera3D();
        virtual ~Camera3D();

        void SetPosition(const DirectX::XMFLOAT3& pos) { position = pos; isDirty = true; }
        void SetTarget(const DirectX::XMFLOAT3& tgt) { target = tgt; isDirty = true; }
        void SetUp(const DirectX::XMFLOAT3& upVec) { up = upVec; isDirty = true; }
        void SetFOV(float fieldOfView) { fov = fieldOfView; isDirty = true; }
        void SetAspectRatio(float ratio) { aspectRatio = ratio; isDirty = true; }
        void SetNearPlane(float nearZ) { nearPlane = nearZ; isDirty = true; }
        void SetFarPlane(float farZ) { farPlane = farZ; isDirty = true; }

        const DirectX::XMFLOAT3& GetPosition() const { return position; }
        const DirectX::XMFLOAT3& GetTarget() const { return target; }
        const DirectX::XMFLOAT3& GetUp() const { return up; }
        float GetFOV() const { return fov; }
        float GetAspectRatio() const { return aspectRatio; }
        float GetNearPlane() const { return nearPlane; }
        float GetFarPlane() const { return farPlane; }

        const DirectX::XMMATRIX& GetViewMatrix();
        const DirectX::XMMATRIX& GetProjectionMatrix();
        
        void LookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

        virtual void Start() override {}
        virtual void Update(float deltaTime) override;

    private:
        void UpdateMatrices();
    };
}