#pragma once
#include "Component.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace AronEngine
{
    struct Mesh
    {
        ComPtr<ID3D11Buffer> vertexBuffer;
        ComPtr<ID3D11Buffer> indexBuffer;
        UINT vertexCount;
        UINT indexCount;
        UINT stride;
    };

    class MeshRenderer : public Component
    {
        DECLARE_COMPONENT_TYPE(MeshRenderer)

    private:
        std::shared_ptr<Mesh> mesh;
        ComPtr<ID3D11VertexShader> vertexShader;
        ComPtr<ID3D11PixelShader> pixelShader;
        ComPtr<ID3D11InputLayout> inputLayout;
        ComPtr<ID3D11Buffer> constantBuffer;
        
        struct ConstantBufferData
        {
            DirectX::XMMATRIX world;
            DirectX::XMMATRIX view;
            DirectX::XMMATRIX projection;
        };

    public:
        MeshRenderer();
        virtual ~MeshRenderer();

        void SetMesh(std::shared_ptr<Mesh> newMesh) { mesh = newMesh; }
        std::shared_ptr<Mesh> GetMesh() const { return mesh; }

        bool InitializeShaders(ID3D11Device* device);
        void Render(ID3D11DeviceContext* context, const DirectX::XMMATRIX& world, 
                   const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);

        virtual void Start() override {}
        virtual void Update(float deltaTime) override {}
    };
}