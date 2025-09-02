#include "TestApp.h"
#include "Core/Input.h"
#include "Core/Renderer.h"
#include "Utils/Color.h"

namespace AronEngine
{
    void TestApp::OnInit()
    {
        DEBUG_LOG("Game TestApp initialization started");
        
        // Initialize 3D test
        show3DTest = false;
        showQuadTest = true;
        
        // Initialize triangle test
        triangleTest = std::make_unique<Test01_Triangle>();
        if (!triangleTest->Initialize(GetRenderer()))
        {
            DEBUG_LOG("Failed to initialize triangle test");
        }
        else
        {
            DEBUG_LOG("Triangle test initialized successfully");
        }
        
        // Initialize quad test
        quadTest = std::make_unique<Test02_Quad>();
        if (!quadTest->Initialize(GetRenderer()))
        {
            DEBUG_LOG("Failed to initialize quad test");
        }
        else
        {
            DEBUG_LOG("Quad test initialized successfully");
        }
    }

    void TestApp::OnUpdate(float deltaTime)
    {
        if (GetInput()->GetKeyDown(KeyCode::Escape))
        {
            Quit();
        }

        if (GetInput()->GetKeyDown(KeyCode::T))
        {
            show3DTest = !show3DTest;
            if (show3DTest) showQuadTest = false;
        }
    }

    void TestApp::OnRender()
    {
        Renderer* renderer = GetRenderer();
        
        if (show3DTest)
        {
            if (triangleTest)
            {
                triangleTest->Render(renderer);
                return;
            }
        }
        else if (showQuadTest)
        {
            if (quadTest)
            {
                quadTest->Render(renderer);
                return;
            }
        }
        
        renderer->Clear(Color(0.2f, 0.2f, 0.2f));
        
        renderer->DrawText(L"AronEngine", Vector2(20, 20), Color::Yellow, L"Arial", 16.0f);
        renderer->DrawText(L"T: Triangle", Vector2(20, 50), Color::Cyan, L"Arial", 12.0f);
        renderer->DrawText(L"ESC: Exit", Vector2(20, 70), Color::Cyan, L"Arial", 12.0f);
        
        renderer->FillRectangle(Vector2(100, 120), Vector2(50, 50), Color::Green);
    }

    void TestApp::OnShutdown()
    {
        DEBUG_LOG("Game TestApp shutting down");
        
        if (triangleTest)
        {
            triangleTest.reset();
        }
        
        if (quadTest)
        {
            quadTest.reset();
        }
    }

}