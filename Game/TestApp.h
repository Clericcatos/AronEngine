#pragma once
#include "Core/EngineApp.h"
#include "Core/GameObject.h"
#include "Core/GameEngine.h"
#include "Utils/Vector2.h"
#include <memory>

namespace AronEngine
{
    class AudioClip;
    class LightUnityEditor;
    class Renderer;
    
    class TestApp : public EngineApp
    {
    private:
        std::shared_ptr<AudioClip> testAudioClip;
        std::unique_ptr<GameObject> testObject;
        std::shared_ptr<GameObject> playerObject;
        std::shared_ptr<GameObject> explosionTrigger;
        
        // Editor system
        LightUnityEditor* editorSystem;
        bool showEditor;
        
        // Simple game state
        Vector2 playerPos;
        Vector2 playerVel;
        bool isJumping;
        
        // Demo state
        float effectTimer;
        bool spacePressed;
        
    public:
        TestApp() = default;
        virtual ~TestApp() = default;

        virtual void OnInit() override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnRender() override;
        virtual void OnShutdown() override;
        
    private:
        void RenderSceneObject(GameObject* gameObject, Renderer* renderer);
        void CreateDemoObjects();
        void HandleDemoInput();
        void UpdateDemoEffects(float deltaTime);
    };
}