#pragma once
#include "Core/EngineApp.h"
#include "Utils/Vector2.h"
#include <memory>

namespace AronEngine
{
    class SimpleAudioClip;
    class ImGuiEditorSystem;
    class GameObject;
    class Renderer;
    
    class TestApp : public EngineApp
    {
    private:
        std::shared_ptr<SimpleAudioClip> testAudio;
        bool audioLoaded;
        
        // Editor system
        ImGuiEditorSystem* editorSystem;
        bool showEditor;
        
        // Simple game state
        Vector2 playerPos;
        Vector2 playerVel;
        bool isJumping;
        
    public:
        TestApp() = default;
        virtual ~TestApp() = default;

        virtual void OnInit() override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnRender() override;
        virtual void OnShutdown() override;
        
    private:
        void RenderGameObject(GameObject* gameObject, Renderer* renderer);
    };
}