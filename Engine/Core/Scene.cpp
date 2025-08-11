#include "framework.h"
#include "Scene.h"
#include "GameObject.h"
#include <algorithm>

namespace AronEngine
{
    Scene::Scene()
        : Object("Scene")
        , isLoaded(false)
    {
    }

    Scene::Scene(const std::string& name)
        : Object(name)
        , isLoaded(false)
    {
    }

    Scene::~Scene()
    {
        gameObjects.clear();
        gameObjectsToDestroy.clear();
    }

    GameObject* Scene::CreateGameObject(const std::string& name)
    {
        return CreateGameObjectInternal(name);
    }

    GameObject* Scene::FindGameObject(const std::string& name) const
    {
        for (const auto& go : gameObjects)
        {
            if (go->GetName() == name)
                return go.get();
        }
        return nullptr;
    }

    GameObject* Scene::FindGameObjectWithTag(const std::string& tag) const
    {
        for (const auto& go : gameObjects)
        {
            if (go->GetTag() == tag)
                return go.get();
        }
        return nullptr;
    }

    std::vector<GameObject*> Scene::FindGameObjectsWithTag(const std::string& tag) const
    {
        std::vector<GameObject*> result;
        for (const auto& go : gameObjects)
        {
            if (go->GetTag() == tag)
                result.push_back(go.get());
        }
        return result;
    }

    void Scene::DestroyGameObject(GameObject* gameObject)
    {
        if (!gameObject)
            return;

        auto it = std::find(gameObjectsToDestroy.begin(), gameObjectsToDestroy.end(), gameObject);
        if (it == gameObjectsToDestroy.end())
        {
            gameObjectsToDestroy.push_back(gameObject);
        }
    }

    void Scene::DestroyGameObjectImmediate(GameObject* gameObject)
    {
        if (!gameObject)
            return;

        gameObjects.erase(
            std::remove_if(gameObjects.begin(), gameObjects.end(),
                [gameObject](const std::unique_ptr<GameObject>& go) { return go.get() == gameObject; }),
            gameObjects.end()
        );

        auto it = std::find(gameObjectsToDestroy.begin(), gameObjectsToDestroy.end(), gameObject);
        if (it != gameObjectsToDestroy.end())
        {
            gameObjectsToDestroy.erase(it);
        }
    }

    void Scene::Start()
    {
        for (auto& go : gameObjects)
        {
            if (go->IsActive())
            {
                go->Start();
            }
        }
    }

    void Scene::Update(float deltaTime)
    {
        ProcessDestroyQueue();

        for (auto& go : gameObjects)
        {
            if (go->IsActive())
            {
                go->Update(deltaTime);
            }
        }
    }

    void Scene::LateUpdate(float deltaTime)
    {
        for (auto& go : gameObjects)
        {
            if (go->IsActive())
            {
                go->LateUpdate(deltaTime);
            }
        }
    }

    void Scene::FixedUpdate(float fixedDeltaTime)
    {
        for (auto& go : gameObjects)
        {
            if (go->IsActive())
            {
                go->FixedUpdate(fixedDeltaTime);
            }
        }
    }

    bool Scene::Save(const std::string& filePath)
    {
        scenePath = filePath;
        return true;
    }

    bool Scene::Load(const std::string& filePath)
    {
        scenePath = filePath;
        isLoaded = true;
        return true;
    }

    void Scene::ProcessDestroyQueue()
    {
        if (gameObjectsToDestroy.empty())
            return;

        for (auto* go : gameObjectsToDestroy)
        {
            DestroyGameObjectImmediate(go);
        }
        gameObjectsToDestroy.clear();
    }

    GameObject* Scene::CreateGameObjectInternal(const std::string& name)
    {
        auto gameObject = std::make_unique<GameObject>(name);
        gameObject->SetScene(this);
        
        GameObject* ptr = gameObject.get();
        gameObjects.push_back(std::move(gameObject));
        
        return ptr;
    }
}