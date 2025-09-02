#pragma once
#include "Core/EngineApp.h"

namespace AronEngine
{
    class GameObject;
    class Renderer;
    
    class EditorApp : public EngineApp
    {
    private:
        
    public:
        EditorApp() = default;
        virtual ~EditorApp() = default;

        virtual void OnInit() override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnRender() override;
        virtual void OnShutdown() override;
        
    private:
        void RenderSceneObject(GameObject* gameObject, Renderer* renderer);
    };
}