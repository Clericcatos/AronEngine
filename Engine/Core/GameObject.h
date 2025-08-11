#pragma once
#include "Object.h"
#include "../Components/Transform.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>

namespace AronEngine
{
    class Component;
    class Scene;

    class GameObject : public Object
    {
        DECLARE_OBJECT_TYPE(GameObject)

    private:
        std::unique_ptr<Transform> transform;
        std::vector<std::unique_ptr<Component>> components;
        std::unordered_map<std::type_index, Component*> componentCache;
        
        GameObject* parent;
        std::vector<GameObject*> children;
        
        Scene* scene;
        int layer;
        std::string tag;

    public:
        GameObject();
        GameObject(const std::string& name);
        virtual ~GameObject();

        Transform* GetTransform() const { return transform.get(); }
        
        GameObject* GetParent() const { return parent; }
        void SetParent(GameObject* parent);
        
        const std::vector<GameObject*>& GetChildren() const { return children; }
        void AddChild(GameObject* child);
        void RemoveChild(GameObject* child);
        
        Scene* GetScene() const { return scene; }
        void SetScene(Scene* scene) { this->scene = scene; }
        
        int GetLayer() const { return layer; }
        void SetLayer(int layer) { this->layer = layer; }
        
        const std::string& GetTag() const { return tag; }
        void SetTag(const std::string& tag) { this->tag = tag; }

        template<typename T, typename... Args>
        T* AddComponent(Args&&... args)
        {
            static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
            
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            T* ptr = component.get();
            ptr->SetGameObject(this);
            
            componentCache[std::type_index(typeid(T))] = ptr;
            components.push_back(std::move(component));
            
            ptr->OnAwake();
            if (IsActive())
                ptr->OnEnable();
            
            return ptr;
        }

        template<typename T>
        T* GetComponent() const
        {
            auto it = componentCache.find(std::type_index(typeid(T)));
            if (it != componentCache.end())
            {
                return static_cast<T*>(it->second);
            }
            return nullptr;
        }

        template<typename T>
        std::vector<T*> GetComponents() const
        {
            std::vector<T*> result;
            for (const auto& component : components)
            {
                if (T* comp = dynamic_cast<T*>(component.get()))
                {
                    result.push_back(comp);
                }
            }
            return result;
        }

        template<typename T>
        void RemoveComponent()
        {
            auto it = componentCache.find(std::type_index(typeid(T)));
            if (it != componentCache.end())
            {
                Component* comp = it->second;
                componentCache.erase(it);
                
                components.erase(
                    std::remove_if(components.begin(), components.end(),
                        [comp](const std::unique_ptr<Component>& c) { return c.get() == comp; }),
                    components.end()
                );
            }
        }

        const std::vector<std::unique_ptr<Component>>& GetAllComponents() const { return components; }

        void Start();
        void Update(float deltaTime);
        void LateUpdate(float deltaTime);
        void FixedUpdate(float fixedDeltaTime);
        
        void OnEnable();
        void OnDisable();
        void OnDestroy();

        static GameObject* Instantiate(const std::string& name = "GameObject");
        static GameObject* Instantiate(GameObject* parent, const std::string& name = "GameObject");
        static void Destroy(GameObject* gameObject);
        static void DestroyImmediate(GameObject* gameObject);
    };
}