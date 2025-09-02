#pragma once
#include "../Core/Object.h"
#include <memory>

namespace AronEngine
{
    class Renderer;
    class BasicMesh;

    class Test01_Triangle : public Object
    {
        DECLARE_OBJECT_TYPE(Test01_Triangle)

    private:
        std::unique_ptr<BasicMesh> triangleMesh;
        bool isInitialized;

    public:
        Test01_Triangle();
        virtual ~Test01_Triangle();

        bool Initialize(Renderer* renderer);
        void Update(float deltaTime);
        void Render(Renderer* renderer);
        void Shutdown();
    };
}