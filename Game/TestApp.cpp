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
        show3DTest = true;  // 기본으로 3D 테스트 켜기
        
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
    }

    void TestApp::OnUpdate(float deltaTime)
    {
        if (GetInput()->GetKeyDown(KeyCode::Escape))
        {
            Quit();
        }

        // Test 3D triangle rendering (press T key)
        if (GetInput()->GetKeyDown(KeyCode::T))
        {
            show3DTest = !show3DTest;
            DEBUG_LOG("T pressed! Triangle: " << (show3DTest ? "ON" : "OFF"));
        }
    }

    void TestApp::OnRender()
    {
        Renderer* renderer = GetRenderer();
        
        if (show3DTest)
        {
            // 3D 삼각형 렌더링만
            if (triangleTest)
            {
                triangleTest->Render(renderer);
                return; // 삼각형만 표시하고 리턴
            }
        }
        
        // 2D 게임 화면
        renderer->Clear(Color(0.2f, 0.2f, 0.2f));
        
        // Instructions
        renderer->DrawText(L"AronEngine - Game Mode", Vector2(20, 20), Color::Yellow, L"Arial", 16.0f);
        renderer->DrawText(L"T: Show Triangle", Vector2(20, 50), Color::Cyan, L"Arial", 12.0f);
        renderer->DrawText(L"ESC: Exit", Vector2(20, 70), Color::Cyan, L"Arial", 12.0f);
        
        // Simple test rectangle
        renderer->FillRectangle(Vector2(100, 100), Vector2(50, 50), Color::Green);
        
        // Status display
        renderer->DrawText(L"Press T for Triangle Test", Vector2(GetWidth() - 250, 50), Color::White, L"Arial", 14.0f);
    }

    void TestApp::OnShutdown()
    {
        DEBUG_LOG("Game TestApp shutting down");
        
        if (triangleTest)
        {
            triangleTest.reset();
        }
    }

}