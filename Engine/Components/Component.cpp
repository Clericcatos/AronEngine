#include "framework.h"
#include "Component.h"
#include "../Core/GameObject.h"

namespace AronEngine
{
    Component::Component()
        : Object("Component")
        , gameObject(nullptr)
        , enabled(true)
    {
    }
    
    Component::Component(const std::string& name)
        : Object(name)
        , gameObject(nullptr)
        , enabled(true)
    {
    }

    Transform* Component::GetTransform() const
    {
        return gameObject ? gameObject->GetTransform() : nullptr;
    }

    void Component::SetEnabled(bool enabled)
    {
        if (this->enabled != enabled)
        {
            this->enabled = enabled;
            
            if (enabled)
                OnEnable();
            else
                OnDisable();
        }
    }
}