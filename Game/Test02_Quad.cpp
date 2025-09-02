#include "Test02_Quad.h"
#include "../Engine/Core/Renderer.h"
#include "../Engine/Rendering/BasicMesh.h"
#include "../Engine/Utils/Color.h"
#include "../Engine/Utils/Vector2.h"

namespace AronEngine
{
    Test02_Quad::Test02_Quad()
        : Object("Quad Test")
        , isInitialized(false)
    {
        quadMesh = std::make_unique<BasicMesh>();
    }

    Test02_Quad::~Test02_Quad()
    {
        Shutdown();
    }

    bool Test02_Quad::Initialize(Renderer* renderer)
    {
        if (!renderer)
        {
            DEBUG_LOG("Renderer is null");
            return false;
        }

        if (!quadMesh->InitializeQuad(renderer->GetD3D11Device()))
        {
            DEBUG_LOG("Failed to initialize quad mesh");
            return false;
        }

        DEBUG_LOG("Test02_Quad initialized successfully - NDC Quad with colors");
        isInitialized = true;
        return true;
    }

    void Test02_Quad::Update(float deltaTime)
    {
    }

    void Test02_Quad::Render(Renderer* renderer)
    {
        if (!isInitialized || !renderer)
            return;

        renderer->Begin3DRender();
        quadMesh->Render(renderer->GetD3D11DeviceContext());
        renderer->End3DRender();
    }

    void Test02_Quad::Shutdown()
    {
        if (isInitialized)
        {
            if (quadMesh)
            {
                quadMesh->Cleanup();
            }
            DEBUG_LOG("Test02_Quad shutting down");
            isInitialized = false;
        }
    }
}