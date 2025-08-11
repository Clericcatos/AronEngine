#pragma once
#include "framework.h"
#include <typeindex>

namespace AronEngine
{
    class Object
    {
    private:
        static uint32_t s_nextInstanceID;
        uint32_t instanceID;
        std::string name;
        bool isActive;

    public:
        Object();
        Object(const std::string& name);
        virtual ~Object() = default;

        uint32_t GetInstanceID() const { return instanceID; }
        const std::string& GetName() const { return name; }
        void SetName(const std::string& name) { this->name = name; }

        bool IsActive() const { return isActive; }
        void SetActive(bool active) { isActive = active; }

        virtual std::type_index GetType() const = 0;
        
        template<typename T>
        bool Is() const
        {
            return GetType() == std::type_index(typeid(T));
        }

        template<typename T>
        T* As()
        {
            if (Is<T>())
                return static_cast<T*>(this);
            return nullptr;
        }

        template<typename T>
        const T* As() const
        {
            if (Is<T>())
                return static_cast<const T*>(this);
            return nullptr;
        }
    };

    #define DECLARE_OBJECT_TYPE(className) \
        virtual std::type_index GetType() const override { return std::type_index(typeid(className)); }
}