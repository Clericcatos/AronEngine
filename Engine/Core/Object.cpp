#include "framework.h"
#include "Object.h"

namespace AronEngine
{
    uint32_t Object::s_nextInstanceID = 1;

    Object::Object()
        : instanceID(s_nextInstanceID++)
        , name("Object")
        , isActive(true)
    {
    }

    Object::Object(const std::string& name)
        : instanceID(s_nextInstanceID++)
        , name(name)
        , isActive(true)
    {
    }
}