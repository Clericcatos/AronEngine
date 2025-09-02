#pragma once
#include "../Core/framework.h"
#include <DirectXMath.h>

namespace AronEngine
{
    struct Vertex3D
    {
        DirectX::XMFLOAT3 position;
    };

    class BasicMesh
    {
    private:
        ComPtr<ID3D11Buffer> vertexBuffer;
        ComPtr<ID3D11VertexShader> vertexShader;
        ComPtr<ID3D11PixelShader> pixelShader;
        ComPtr<ID3D11InputLayout> inputLayout;
        UINT vertexCount;
        UINT stride;

    public:
        BasicMesh();
        ~BasicMesh();

        bool InitializeTriangle(ID3D11Device* device);
        void Render(ID3D11DeviceContext* context);
        void Cleanup();

    private:
        bool CreateShaders(ID3D11Device* device);
        bool CreateVertexBuffer(ID3D11Device* device);
    };
}