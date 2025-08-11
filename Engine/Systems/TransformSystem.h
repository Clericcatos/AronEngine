#pragma once
#include "../Core/framework.h"

namespace AronEngine
{
    class TransformSystem
    {
    public:
        TransformSystem() = default;
        ~TransformSystem() = default;

        void Update(float deltaTime);
    };
}