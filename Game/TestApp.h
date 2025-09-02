#pragma once
#include "Core/EngineApp.h"
#include "Core/GameObject.h"
#include "Core/GameEngine.h"
#include "Utils/Vector2.h"
#include "Tests/Test01_Triangle.h"
#include "Test02_Quad.h"
#include <memory>

namespace AronEngine
{
    class Renderer;
    
    class TestApp : public EngineApp
    {
    private:
        // 3D Test
        bool show3DTest;
        std::unique_ptr<Test01_Triangle> triangleTest;
        
        // Quad Renderer Test
        std::unique_ptr<Test02_Quad> quadTest;
        bool showQuadTest;
        
    public:
        TestApp() = default;
        virtual ~TestApp() = default;

        virtual void OnInit() override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnRender() override;
        virtual void OnShutdown() override;
        
    };
}