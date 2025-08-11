#pragma once
#include "framework.h"
#include "Object.h"
#include <memory>
#include <vector>
#include <string>

namespace AronEngine
{
    class GameObject;

    class Scene : public Object
    {
        DECLARE_OBJECT_TYPE(Scene)

    private:
        std::vector<std::unique_ptr<GameObject>> gameObjects;
        std::vector<GameObject*> gameObjectsToDestroy;
        bool isLoaded;
        std::string scenePath;

    public:
        Scene();
        Scene(const std::string& name);
        virtual ~Scene();

        GameObject* CreateGameObject(const std::string& name = "GameObject");
        GameObject* FindGameObject(const std::string& name) const;
        GameObject* FindGameObjectWithTag(const std::string& tag) const;
        std::vector<GameObject*> FindGameObjectsWithTag(const std::string& tag) const;

        void DestroyGameObject(GameObject* gameObject);
        void DestroyGameObjectImmediate(GameObject* gameObject);

        const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const { return gameObjects; }
        int GetGameObjectCount() const { return static_cast<int>(gameObjects.size()); }

        void Start();
        void Update(float deltaTime);
        void LateUpdate(float deltaTime);
        void FixedUpdate(float fixedDeltaTime);

        bool IsLoaded() const { return isLoaded; }
        void SetLoaded(bool loaded) { isLoaded = loaded; }

        const std::string& GetScenePath() const { return scenePath; }
        void SetScenePath(const std::string& path) { scenePath = path; }

        bool Save(const std::string& filePath);
        bool Load(const std::string& filePath);

    private:
        void ProcessDestroyQueue();
        GameObject* CreateGameObjectInternal(const std::string& name);
    };
}