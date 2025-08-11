#pragma once

namespace AronEngine
{
    template<typename T>
    class Singleton
    {
    protected:
        Singleton() = default;
        virtual ~Singleton() = default;
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;

    public:
        static T& GetInstance()
        {
            static T instance;
            return instance;
        }
    };
}