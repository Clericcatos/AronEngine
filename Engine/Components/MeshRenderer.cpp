#include "framework.h"
#include "MeshRenderer.h"

namespace AronEngine
{
    MeshRenderer::MeshRenderer()
        : Component("MeshRenderer")
        , mesh(nullptr)
    {
    }

    MeshRenderer::~MeshRenderer()
    {
    }

    bool MeshRenderer::InitializeShaders(ID3D11Device* device)
    {
        return true;
    }

    void MeshRenderer::Render(ID3D11DeviceContext* context, const DirectX::XMMATRIX& world, 
                             const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
    {
        if (!mesh)
            return;

        UINT stride = mesh->stride;
        UINT offset = 0;
        
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        ConstantBufferData cbData;
        cbData.world = DirectX::XMMatrixTranspose(world);
        cbData.view = DirectX::XMMatrixTranspose(view);
        cbData.projection = DirectX::XMMatrixTranspose(projection);

        D3D11_MAPPED_SUBRESOURCE mapped;
        if (SUCCEEDED(context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        {
            memcpy(mapped.pData, &cbData, sizeof(ConstantBufferData));
            context->Unmap(constantBuffer.Get(), 0);
        }

        context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
        context->VSSetShader(vertexShader.Get(), nullptr, 0);
        context->PSSetShader(pixelShader.Get(), nullptr, 0);
        context->IASetInputLayout(inputLayout.Get());

        context->DrawIndexed(mesh->indexCount, 0, 0);
    }
}