#pragma once
#include "../Engine/Core/Object.h"
#include <memory>

namespace AronEngine
{
    class Renderer;
    class BasicMesh;

    class Test02_Quad : public Object
    {
        DECLARE_OBJECT_TYPE(Test02_Quad)

    private:
        std::unique_ptr<BasicMesh> quadMesh;
        bool isInitialized;

    public:
        Test02_Quad();
        virtual ~Test02_Quad();

        bool Initialize(Renderer* renderer);
        void Update(float deltaTime);
        void Render(Renderer* renderer);
        void Shutdown();
    };
}