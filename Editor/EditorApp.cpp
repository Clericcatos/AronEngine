#include "EditorApp.h"
#include "Core/Input.h"
#include "Core/Renderer.h"
#include "Core/TextureManager.h"
#include "Core/EngineTime.h"
#include "Core/GameObject.h"
#include "Utils/Color.h"
#include "Components/Transform.h"
#include "Components/SpriteRenderer.h"
#include <iostream>
#include <windows.h>

namespace AronEngine
{
    void EditorApp::OnInit()
    {
        OutputDebugStringA("[EditorApp] OnInit called\n");
        DEBUG_LOG("Simple EditorApp initialization started");
        
        // 복잡한 에디터 시스템 없이 간단한 에디터만
        // 렌더러가 제대로 초기화되었는지 확인
        if (!GetRenderer())
        {
            DEBUG_LOG("ERROR: Renderer is null in OnInit");
            OutputDebugStringA("[EditorApp] ERROR: Renderer is null\n");
        }
        else
        {
            DEBUG_LOG("Renderer initialized successfully");
            OutputDebugStringA("[EditorApp] Renderer OK\n");
        }
        
        DEBUG_LOG("Simple EditorApp initialization completed");
        OutputDebugStringA("[EditorApp] OnInit completed\n");
    }

    void EditorApp::OnUpdate(float deltaTime)
    {
        static int frameCount = 0;
        frameCount++;
        
        if (frameCount % 60 == 0)  // 매 60프레임마다 로그
        {
            OutputDebugStringA(("[EditorApp] OnUpdate - Frame: " + std::to_string(frameCount) + "\n").c_str());
        }
        
        if (GetInput() && GetInput()->GetKeyDown(KeyCode::Escape))
        {
            OutputDebugStringA("[EditorApp] ESC pressed, quitting...\n");
            Quit();
        }
    }

    void EditorApp::OnRender()
    {
        static int renderCount = 0;
        renderCount++;
        
        Renderer* renderer = GetRenderer();
        if (!renderer)
        {
            OutputDebugStringA("[EditorApp] ERROR: Renderer is null in OnRender\n");
            return;
        }
        
        try
        {
            // Simple editor background
            renderer->Clear(Color(0.3f, 0.3f, 0.3f));
            
            // Check if text rendering is available
            bool canDrawText = true;
            try {
                renderer->DrawText(L"Test", Vector2(-1000, -1000), Color::White, L"Arial", 1.0f);
            } catch(...) {
                canDrawText = false;
            }
            
            if (canDrawText)
            {
                // Simple editor UI
                renderer->DrawText(L"AronEngine - Editor Mode", Vector2(20, 20), Color::White, L"Arial", 18.0f);
                renderer->DrawText(L"Simple Editor (No Crashes!)", Vector2(20, 50), Color::Green, L"Arial", 14.0f);
                renderer->DrawText(L"ESC: Exit", Vector2(20, 80), Color::Cyan, L"Arial", 12.0f);
                
                // Frame counter display
                std::wstring frameText = L"Frame: " + std::to_wstring(renderCount);
                renderer->DrawText(frameText.c_str(), Vector2(20, 110), Color::Yellow, L"Arial", 12.0f);
                
                // Panel labels
                renderer->DrawText(L"Hierarchy", Vector2(60, 160), Color::White, L"Arial", 12.0f);
                renderer->DrawText(L"Scene View", Vector2(310, 160), Color::White, L"Arial", 12.0f);
                renderer->DrawText(L"Inspector", Vector2(760, 160), Color::White, L"Arial", 12.0f);
            }
            
            // Draw some editor-like panels (rectangles only)
            renderer->DrawRectangle(Vector2(50, 150), Vector2(200, 300), Color::Gray, 2.0f);
            renderer->DrawRectangle(Vector2(300, 150), Vector2(400, 300), Color::Gray, 2.0f);
            renderer->DrawRectangle(Vector2(750, 150), Vector2(200, 300), Color::Gray, 2.0f);
            
            // Simple status indicator
            renderer->DrawRectangle(Vector2(10, 10), Vector2(5, 5), Color::Green, 0.0f);
        }
        catch (const std::exception& e)
        {
            OutputDebugStringA(("[EditorApp] Exception in OnRender: " + std::string(e.what()) + "\n").c_str());
        }
        catch (...)
        {
            OutputDebugStringA("[EditorApp] Unknown exception in OnRender\n");
        }
    }

    void EditorApp::OnShutdown()
    {
        DEBUG_LOG("Simple EditorApp shutting down");
    }

    void EditorApp::RenderSceneObject(GameObject* gameObject, Renderer* renderer)
    {
        // Simple placeholder for now
    }
}