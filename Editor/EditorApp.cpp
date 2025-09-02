#include "EditorApp.h"
#include "Core/Input.h"
#include "Core/Renderer.h"
#include "Core/TextureManager.h"
#include "Core/EngineTime.h"
#include "Core/GameObject.h"
#include "Utils/Color.h"
#include "Components/Transform.h"
#include "Components/SpriteRenderer.h"

namespace AronEngine
{
    void EditorApp::OnInit()
    {
        DEBUG_LOG("Simple EditorApp initialization started");
        
        // 복잡한 에디터 시스템 없이 간단한 에디터만
        
        DEBUG_LOG("Simple EditorApp initialization completed");
    }

    void EditorApp::OnUpdate(float deltaTime)
    {
        if (GetInput() && GetInput()->GetKeyDown(KeyCode::Escape))
        {
            Quit();
        }
    }

    void EditorApp::OnRender()
    {
        Renderer* renderer = GetRenderer();
        
        // Simple editor background
        renderer->Clear(Color(0.3f, 0.3f, 0.3f));
        
        // Simple editor UI
        renderer->DrawText(L"AronEngine - Editor Mode", Vector2(20, 20), Color::White, L"Arial", 18.0f);
        renderer->DrawText(L"Simple Editor (No Crashes!)", Vector2(20, 50), Color::Green, L"Arial", 14.0f);
        renderer->DrawText(L"ESC: Exit", Vector2(20, 80), Color::Cyan, L"Arial", 12.0f);
        
        // Draw some editor-like panels
        renderer->DrawRectangle(Vector2(50, 120), Vector2(200, 300), Color::Gray, 2.0f);
        renderer->DrawText(L"Hierarchy", Vector2(60, 130), Color::White, L"Arial", 12.0f);
        
        renderer->DrawRectangle(Vector2(300, 120), Vector2(400, 300), Color::Gray, 2.0f);
        renderer->DrawText(L"Scene View", Vector2(310, 130), Color::White, L"Arial", 12.0f);
        
        renderer->DrawRectangle(Vector2(750, 120), Vector2(200, 300), Color::Gray, 2.0f);
        renderer->DrawText(L"Inspector", Vector2(760, 130), Color::White, L"Arial", 12.0f);
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