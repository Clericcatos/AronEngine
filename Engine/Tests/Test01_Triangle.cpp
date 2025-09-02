#include "../Core/framework.h"
#include "Test01_Triangle.h"
#include "../Core/Renderer.h"
#include "../Rendering/BasicMesh.h"

namespace AronEngine
{
    Test01_Triangle::Test01_Triangle()
        : Object("Triangle Test")
        , isInitialized(false)
    {
        triangleMesh = std::make_unique<BasicMesh>();
    }

    Test01_Triangle::~Test01_Triangle()
    {
        Shutdown();
    }

    bool Test01_Triangle::Initialize(Renderer* renderer)
    {
        if (!triangleMesh->InitializeTriangle(renderer->GetD3D11Device()))
            return false;

        isInitialized = true;
        return true;
    }

    void Test01_Triangle::Update(float deltaTime)
    {
    }

    void Test01_Triangle::Render(Renderer* renderer)
    {
        if (!isInitialized)
            return;

        renderer->Begin3DRender();
        triangleMesh->Render(renderer->GetD3D11DeviceContext());
        renderer->End3DRender();
    }

    void Test01_Triangle::Shutdown()
    {
        if (triangleMesh)
        {
            triangleMesh->Cleanup();
        }
        isInitialized = false;
    }
}