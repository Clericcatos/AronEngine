#pragma once
#include "../Core/framework.h"

namespace AronEngine
{
    class ScriptSystem
    {
    public:
        ScriptSystem() = default;
        ~ScriptSystem() = default;

        void Update(float deltaTime);
        void LateUpdate(float deltaTime);
        void FixedUpdate(float fixedDeltaTime);
    };
}