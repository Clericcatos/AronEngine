#include "../Core/framework.h"
#include "BasicMesh.h"
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace AronEngine
{
    BasicMesh::BasicMesh()
        : vertexCount(0), indexCount(0), stride(sizeof(Vertex3D))
    {
    }

    BasicMesh::~BasicMesh()
    {
        Cleanup();
    }

    bool BasicMesh::InitializeTriangle(ID3D11Device* device)
    {
        if (!CreateShaders(device))
            return false;
            
        if (!CreateVertexBuffer(device))
            return false;

        return true;
    }

    void BasicMesh::Render(ID3D11DeviceContext* context)
    {
        UINT offset = 0;
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetInputLayout(inputLayout.Get());
        context->VSSetShader(vertexShader.Get(), nullptr, 0);
        context->PSSetShader(pixelShader.Get(), nullptr, 0);
        
        if (indexBuffer)
        {
            context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            context->DrawIndexed(indexCount, 0, 0);
        }
        else
        {
            context->Draw(vertexCount, 0);
        }
    }

    void BasicMesh::Cleanup()
    {
        vertexBuffer.Reset();
        indexBuffer.Reset();
        vertexShader.Reset();
        pixelShader.Reset();
        inputLayout.Reset();
    }

    bool BasicMesh::CreateShaders(ID3D11Device* device)
    {
        const char* vertexShaderSource = R"(
            struct VS_INPUT
            {
                float3 pos : POSITION;
                float4 color : COLOR;
            };
            
            struct VS_OUTPUT
            {
                float4 pos : SV_POSITION;
                float4 color : COLOR;
            };
            
            VS_OUTPUT main(VS_INPUT input)
            {
                VS_OUTPUT output;
                output.pos = float4(input.pos, 1.0f);
                output.color = input.color;
                return output;
            }
        )";

        const char* pixelShaderSource = R"(
            float4 main(float4 pos : SV_POSITION, float4 color : COLOR) : SV_TARGET
            {
                return color;
            }
        )";

        ComPtr<ID3DBlob> vsBlob, psBlob, errorBlob;
        
        HRESULT hr = D3DCompile(vertexShaderSource, strlen(vertexShaderSource), nullptr, nullptr, nullptr,
                               "main", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
        if (FAILED(hr)) return false;

        hr = D3DCompile(pixelShaderSource, strlen(pixelShaderSource), nullptr, nullptr, nullptr,
                       "main", "ps_5_0", 0, 0, &psBlob, &errorBlob);
        if (FAILED(hr)) return false;

        hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
        if (FAILED(hr)) return false;

        hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
        if (FAILED(hr)) return false;

        D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        hr = device->CreateInputLayout(inputDesc, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
        return SUCCEEDED(hr);
    }

    bool BasicMesh::CreateVertexBuffer(ID3D11Device* device)
    {
        Vertex3D vertices[] = {
            { DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
            { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
            { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }
        };

        vertexCount = 3;

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = sizeof(vertices);
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertices;

        return SUCCEEDED(device->CreateBuffer(&bufferDesc, &initData, &vertexBuffer));
    }

    bool BasicMesh::InitializeQuad(ID3D11Device* device)
    {
        if (!CreateShaders(device))
            return false;
            
        if (!CreateBuffers(device))
            return false;

        return true;
    }

    bool BasicMesh::CreateBuffers(ID3D11Device* device)
    {
        // 정점 4개로 사각형 만들기, NDC 좌표 -1~1 범위 사용
        Vertex3D vertices[] = {
            { DirectX::XMFLOAT3(-0.3f,  0.4f, 0.0f), DirectX::XMFLOAT4(0.9f, 0.4f, 0.7f, 1.0f) },
            { DirectX::XMFLOAT3( 0.3f,  0.4f, 0.0f), DirectX::XMFLOAT4(0.4f, 0.8f, 0.9f, 1.0f) },
            { DirectX::XMFLOAT3( 0.3f, -0.4f, 0.0f), DirectX::XMFLOAT4(0.6f, 0.9f, 0.5f, 1.0f) },
            { DirectX::XMFLOAT3(-0.3f, -0.4f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.7f, 0.3f, 1.0f) }
        };

        vertexCount = 4;

        UINT indices[] = {
            0, 1, 2,
            0, 2, 3
        };

        indexCount = 6;

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = sizeof(vertices);
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertices;

        HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);
        if (FAILED(hr)) return false;

        bufferDesc.ByteWidth = sizeof(indices);
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        initData.pSysMem = indices;

        hr = device->CreateBuffer(&bufferDesc, &initData, &indexBuffer);
        return SUCCEEDED(hr);
    }
}