#pragma once
#include "../Core/Object.h"

namespace AronEngine
{
    class GameObject;
    class Transform;

    class Component : public Object
    {
        DECLARE_OBJECT_TYPE(Component)

    protected:
        GameObject* gameObject;
        bool enabled;

    public:
        Component();
        Component(const std::string& name);
        virtual ~Component() = default;

        GameObject* GetGameObject() const { return gameObject; }
        void SetGameObject(GameObject* go) { gameObject = go; }
        
        Transform* GetTransform() const;

        bool IsEnabled() const { return enabled && IsActive(); }
        void SetEnabled(bool enabled);

        virtual void Start() {}
        virtual void Update(float deltaTime) {}
        virtual void OnDestroy() {}
        
        virtual void OnAwake() {}
        virtual void OnEnable() {}
        virtual void OnStart() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnLateUpdate(float deltaTime) {}
        virtual void OnFixedUpdate(float fixedDeltaTime) {}
        virtual void OnDisable() {}

        virtual void OnDrawGizmos() {}
        virtual void OnDrawGizmosSelected() {}
    };
}